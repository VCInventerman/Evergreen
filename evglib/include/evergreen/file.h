#pragma once

#include <evergreen/types.h>

#include <evergreen/string.h>
#include <evergreen/bit.h>

/*
A copy of a file in memory that can watch it and be committed back
An adapter to the OS file that can read() and write() regions
A memmapped file	MappedFile
*/

namespace evg
{

	// Paths are internally stored as UTF-8 unix paths
	//todo: force trailing slash on directories
	class Path
	{
	public:
		using This = Path;

		using value_type = Char;
		using allocator_type = std::allocator<Char>;
		using size_type = Size;
		using difference_type = Offset;
		using reference = Char&;
		using const_reference = const Char&;
		using iterator = RandomContigIterator<const Char>;
		using const_iterator = RandomContigIterator<const Char>;
		using reverse_iterator = RevRandomContigIterator<const Char>;
		using const_reverse_iterator = RevRandomContigIterator<const Char>;


	public: // Access is discouraged
		String string_raw;

	public:
		// Returns everything after the last /
		ImString filename()
		{
			Size lastSlash = string_raw.rfind('/');
			return StringBuilder(string_raw.begin() + lastSlash + 1, string_raw.end());
		}

		// Returns everything up to, but not including, the last slash
		//todo: better syntax for std::copy, like [begin,end) but in this case [begin,end]
		ImString parent()
		{
			auto lastSlash = string_raw.rfind('/');
			return StringBuilder(string_raw.begin(), string_raw.begin() + lastSlash);
		}

#if defined(EVG_PLATFORM_WIN) && defined(EVG_COMPILE_AOT)
		Bool exists()
		{
			DWORD attributes = GetFileAttributesW(string_raw.source->getAlternate());
			return attributes != INVALID_FILE_ATTRIBUTES;
		}

		Bool isFile()
		{
			DWORD attributes = GetFileAttributesW(string_raw.source->getAlternate());
			return (attributes != INVALID_FILE_ATTRIBUTES) && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		Bool isDirectory()
		{
			DWORD attributes = GetFileAttributesW(string_raw.source->getAlternate());
			return (attributes != INVALID_FILE_ATTRIBUTES) && (attributes & FILE_ATTRIBUTE_DIRECTORY);
		}
#elif defined(EVG_COMPILE_AOT)
		Bool exists()
		{


		}
#else
		Bool exists();
#endif

		const WChar* wide()
		{
			return string_raw.source->getAlternate();
		}

		template<typename T>
		Path& next(const T&& add)
		{
			if (back() == '/')
			{
				*this += add;
			}
			else
			{ //todo: unicode, safe
				StringBuilder newString = StringBuilder(size() - 1 + add.size());
			}

			return *this;
		}

		Path() : string_raw() {}
		Path(String _str) : string_raw(std::move(_str.replaceAll('\\', '/'))) {}
		Path(const StringBuilder& _str) : string_raw(std::move(StringBuilder(_str.data()).replaceAll('\\', '/'))) {}
		Path(const Char* const _str) : string_raw(std::move(StringBuilder(_str).replaceAll('\\', '/'))) {}
		Path(const WChar* const _str)
		{
			StringBuilder builder;
			utf16ToUtf8(ContiguousBufPtrEnd<const WChar>(_str, strlen(_str)), builder);
			builder.replaceAll('\\', '/');

			builder.ensureNullTerminated();
			string_raw.source = string_raw.defaultManager.insert(builder.data(), builder.data() + builder.size(), false);
			string_raw.source->addRef();

			builder.data_raw.data_raw = {};
			builder.data_raw.reserved_raw = nullptr;
		}
		Path(const Path& rhs) : string_raw(rhs.string_raw) {}
		Path(const Path&& rhs) : string_raw(std::move(rhs.string_raw)) {}

		Path operator+= (const Path& rhs)
		{
			string_raw += rhs.string_raw;
		}

		/*template<typename T>
		T operator_conv() const
		{
			return string_raw.operator_conv<T>();
		}

		EVG_CXX_CAST(CChar*)*/
		//EVG_CXX_CAST_ADAPT(std::filesystem::path, CChar*)
		operator ContiguousBufPtrEnd<const Char>() const { return range(); }
		explicit operator std::string() const { return string_raw.data(); }
		operator const WChar* () { return wide(); }

		friend std::ostream& operator<< (std::ostream& stream, const Path& rhs)
		{
			stream << rhs.string_raw;
			return stream;
		}

		bool operator< (const Path& rhs) const { return string_raw < rhs.string_raw; }

		Path& operator= (const Path& rhs) { this->string_raw = rhs.string_raw; return *this; }
		Path& operator= (Path&& rhs) { this->string_raw = rhs.string_raw; return *this; }
		Path& operator= (const evg::StringBuilder& rhs) { this->string_raw = String(rhs.data(), false); return *this; } //todo: move syntax, not just no copy
		Path& operator= (const Char* const rhs) { return *this = Path(rhs); }

		ContiguousBufPtrEnd<CChar> range() const { return string_raw.source->string.range(); }
		CChar* data() const { return string_raw.source->string.data(); }
		const_iterator begin() const { return range().begin(); }
		const_iterator end() const { return range().end(); }
		const_iterator cbegin() const { return range().cbegin(); }
		const_iterator cend() const { return range().cend(); }
		const_reverse_iterator rbegin() const { return range().rbegin(); }
		const_reverse_iterator rend() const { return range().rend(); }
		const_reverse_iterator crbegin() const { return range().crbegin(); }
		const_reverse_iterator crend() const { return range().crend(); }
		CChar& back() const { return *(end() - 1); }

		Size size() const { return string_raw.size(); }
	};

#if defined(EVG_PLATFORM_WIN) && defined(EVG_COMPILE_AOT)
	Path getAdminDataFolder()
	{
		KNOWNFOLDERID refId = { 0x905e63b6, 0xc1bf, 0x494e, {0xb2,0x9c, 0x65,0xb7,0x32,0xd3,0xd2,0x1a} }; // FOLDERID_ProgramFiles;
		PWSTR path = nullptr;
		HRESULT res1 = SHGetKnownFolderPath(refId, 0, NULL, &path);
		if (SUCCEEDED(res1))
		{
			DWORD fileAttributes = GetFileAttributes(path);

			if (fileAttributes != INVALID_FILE_ATTRIBUTES &&
				fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				Path ret = path;
				CoTaskMemFree(path);
				return ret;
			}
			else
			{
				CoTaskMemFree(path);
				return "";
			}
		}
		else
		{
			CoTaskMemFree(path);
			DWORD fileAttributes = GetFileAttributes(L"C:\\Program Files");

			if (fileAttributes != INVALID_FILE_ATTRIBUTES &&
				fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				return "C:/Program Files";
			}
			else
			{
				return "";
			}
		}
	}

	Path getPublicDataFolder()
	{
		KNOWNFOLDERID refId = { 0x559D40A3, 0xA036, 0x40FA, {0xAF,0x61, 0x84,0xCB,0x43,0x0A,0x4D,0x34} }; // FOLDERID_AppDataProgramData;
		PWSTR path = nullptr;
		HRESULT res1 = SHGetKnownFolderPath(refId, 0, NULL, &path);
		if (SUCCEEDED(res1))
		{
			DWORD fileAttributes = GetFileAttributes(path);

			if (fileAttributes != INVALID_FILE_ATTRIBUTES &&
				fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				Path ret = path;
				CoTaskMemFree(path);
				return ret;
			}
			else
			{
				CoTaskMemFree(path);
				return "";
			}
		}
		else
		{
			CoTaskMemFree(path);
			DWORD fileAttributes = GetFileAttributes(L"C:\\ProgramData");

			if (fileAttributes != INVALID_FILE_ATTRIBUTES &&
				fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				return "C:/ProgramData";
			}
			else
			{
				return "";
			}
		}
	}

	Path getUserDataFolder()
	{
		KNOWNFOLDERID refId = { 0x3EB685DB, 0x65F9, 0x4CF6, {0xA0,0x3A,0xE3,0xEF,0x65,0x72,0x9F,0x3D } }; // FOLDERID_RoamingAppData;
		PWSTR path = nullptr;
		HRESULT res1 = SHGetKnownFolderPath(refId, 0, NULL, &path);
		if (SUCCEEDED(res1))
		{
			DWORD fileAttributes = GetFileAttributes(path);

			if (fileAttributes != INVALID_FILE_ATTRIBUTES &&
				fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				return path;
			}
			else
			{
				return "";
			}
		}
		else
		{
			return "";
		}
	}
#elif defined(EVG_COMPILE_AOT)
	Path getAdminDataFolder()
	{

	}

	Path getPublicDataFolder()
	{

	}

	Path getUserDataFolder()
	{

	}
#else
	Path getAdminDataFolder();
	Path getPublicDataFolder();
	Path getUserDataFolder();
#endif


	Vector<char> simpleFileRead(Path path)
	{
		std::ifstream file(path, std::ifstream::binary | std::ifstream::ate);
		Vector<char> vec(file.tellg());
		file.seekg(0);
		file.read(vec.data(), vec.size());
		return vec;
	}




	/*class File
	{
	public:
		enum class Permission : Long
		{
			Read = GENERIC_READ,
			Write = GENERIC_WRITE,
			Execute = GENERIC_EXECUTE,
			RW = Read | Write,
			All = GENERIC_ALL,
		}

		// constructor () -> ()
		// constructor (Path _path) -> ()

		virtual ~File() = 0;

		virtual String getName() = 0;

		virtual bool isLoaded() = 0;
		virtual bool load() = 0;
		virtual void unload() = 0;

		virtual size_t getSize() const = 0;

		virtual void seek(const size_t _cursor, const std::ios_base::seekdir dir) = 0;*/

		//virtual void readBuf(ContiguousBufPtrEnd<char> buf) = 0;
		//virtual char* readAll(size_t& _size) = 0;
		//virtual void write(const char buf[], const size_t size) = 0;

		/*virtual Char* begin() = 0;
		virtual Char* end() = 0;

		template<typename InT = char, typename OutT = InT>
		OutT read()
		{
			char buf[sizeof(InT)];
			readBuf(buf);
			InT* in = (InT*)buf;
			return (OutT)(*in);
		}
	};*/


	/*class MemFile : public File
	{
	public:
		char* data = nullptr;
		size_t size = 0;
		size_t cursor = 0;

		Path path;
		std::ifstream file;

		MemFile() = default;
		MemFile(Path _path) : path(_path), file(_path, std::ios::binary)
		{
			load();
		}

		~MemFile()
		{
			delete data;
		}

		String getName()
		{
			return String((Char*)"MemFile");
		}

		bool isLoaded() { return true; }
		bool load()
		{
			size = std::filesystem::file_size(path.wide());
			data = (char*)malloc(size + 1);
			file.read(data, size);
			data[size] = '\0';

			return true;
		}
		void unload() {}

		size_t getSize() const { return size; }

		void seek(const size_t _cursor, const std::ios_base::seekdir dir)
		{
			switch (dir)
			{
			case std::ios::beg:
			{
				cursor = _cursor;
			}
			case std::ios::cur:
			{
				cursor += _cursor;
			}
			case std::ios::end:
			{
				cursor = size - _cursor;
			}
			}
		}

		void read(char _buf[], const size_t _size)
		{
			std::copy(data + cursor, data + cursor + _size, _buf);
		}
		char* readAll(size_t& _size)
		{
			_size = size;
			char* buf = new char[size];
			std::copy(data, data + size, buf);
			return buf;
		}
		void write(const char buf[], const size_t _size)
		{
			std::copy(buf, buf + _size, data + cursor);
		}

		Char* begin()
		{
			return (Char*)data;
		}
		Char* end()
		{
			return (Char*)data + size;
		}
	};*/



	// Iterate over the contents of a file
	// Does not support concurrency: todo: boost strand
	class IterFile
	{
	public:
		enum class Permission : Int
		{
			Read = (Int)GENERIC_READ,
			Write = (Int)GENERIC_WRITE,
			Execute = (Int)GENERIC_EXECUTE,
			RW = Read | Write,
			All = (Int)GENERIC_ALL,
		};

		typedef void (*CompletionHandler)(void*, LPOVERLAPPED, const std::error_code&, Size);

		class Iterator
		{
		public:
			using This = Iterator;

			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using difference_type = Offset;
			using pointer = char*;
			using reference = char&;


			Size blockPos = 0;
			Size pos = 0;


			Iterator(const Size _pos) : blockPos(_pos% bufSize), pos(_pos)
			{
				//co_await loadBuf(blockPos);
			}

		};

		struct AsyncFileOperation
		{
		public:
			using Op = std::function<void(std::coroutine_handle<>& handle)>;

			
			Op op;
			Int res;

			bool await_ready()
			{
				return false;
			}

			void await_suspend(std::coroutine_handle<> handle)
			{
				op(handle);
			}

			Int await_resume()
			{
				return res;
			}
		};



		using This = IterFile;

		using value_type = Char;
		using size_type = Size;
		using difference_type = Offset;
		using reference = Char&;
		using const_reference = const Char&;
		using iterator = Iterator;
		using const_iterator = const Iterator;
		using reverse_iterator = InvalidType;
		using const_reverse_iterator = InvalidType;


		constexpr static Size bufSize = 4096; // block size on my system

		OVERLAPPED overlap; // The first 4 members of this struct are fixed in order for compatibility with win32 and asio
		nullptr_t pad1 = nullptr;
		CompletionHandler completionHandler = &This::onCompletion;
		long ready;



		AsyncFileOperation op;

		char* buf;


		Path path;
		Permission perm;
		HANDLE fileHandle;
		Size fileSize;

		std::coroutine_handle<> coHandle;


		IterFile(const Path _path, const Permission _perm = Permission::Read) : path(std::move(_path)), perm(_perm)
		{
			buf = (char*)VirtualAlloc(nullptr, bufSize, MEM_COMMIT, PAGE_READWRITE);

			if (buf == nullptr)
			{
				DebugBreak();
			}


			Int32 share = ((Int)perm & (Int)GENERIC_WRITE) ? 0 : (Int)FILE_SHARE_READ; // Allow other processes to read from this file if we don't write to it
			Int32 openPolicy = ((Int)perm & (Int)GENERIC_WRITE) ? (Int)CREATE_NEW : (Int)OPEN_EXISTING; // Allow creation of new file if we are writing to it, otherwise do not

			fileHandle = CreateFileW(path, (Int)perm, (Int)share, NULL, (Int)openPolicy,
				FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED /*| FILE_FLAG_NO_BUFFERING*/, NULL);

			HANDLE test = CreateIoCompletionPort(fileHandle, threads.native(), 0, 0); // Associate file with thread pool
			

			if (fileHandle == INVALID_HANDLE_VALUE)
			{
				DebugBreak();

				//todo: custom error type containing result from GetLastError()
			}


			DWORD highOrderSize = 0;
			fileSize = GetFileSize(fileHandle, &highOrderSize);
			fileSize |= Size(highOrderSize) << 32;
		}

		~IterFile()
		{
			CloseHandle(fileHandle);
		}

		AsyncFileOperation loadBuf(const Size blockPos)
		{
			ready = 0;
			memset(&overlap, 0, sizeof(overlap));
			overlap.Offset = (UInt32)blockPos;
			overlap.OffsetHigh = (UInt32)(blockPos >> 32);

			op.op = ([this](std::coroutine_handle<> _coHandle)
				{
					coHandle = _coHandle;
					DWORD sizeRead = 0;
					ReadFile(fileHandle, buf, bufSize, &sizeRead, &overlap);
					InterlockedExchangeAcquire(&ready, 1);					
				});

			return op;
		}

		static void onCompletion(void*, LPOVERLAPPED overlap, const std::error_code& e, Size size)
		{
			IterFile* file = (IterFile*)overlap;

			threads.post([file, e]()
				{
					file->op.res = e.value();
					file->coHandle(); 
				});
		}

		iterator begin()
		{

		}

	};
}




/*
namespace evg
{
	// Paths are internally stored as UTF-8 unix paths
	class Path
	{
	public:
		StringIm raw;

		// Returns everything after the last /
		StringIm filename()
		{
			return {raw.rfind('/'), raw.end() }
		}

		StringIm parentPath()
		{

		}

		Path(const char* _raw) : raw(_raw) {}
	};

	class File
	{
	public:
		virtual cStringC getName() = 0;

		virtual bool isLoaded() = 0;
		virtual bool load() = 0;
		virtual void unload() = 0;

		virtual size_t getSize() const = 0;

		virtual void seek(const size_t _cursor, const std::ios_base::seekdir dir) = 0;

		virtual void read(char buf[], const size_t size) = 0;
		virtual char* readAll(size_t& _size) = 0;
		virtual void write(const char buf[], const size_t size) = 0;

		virtual ~File() = 0 {};
	};

	class MemFile : public File
	{
	public:
		char* data = nullptr;
		size_t size = 0;
		size_t cursor = 0;

		cstring_view getName()
		{
			return "MemFile";
		}

		bool isLoaded() { return true; }
		bool load() { return true; }
		void unload() {}

		size_t getSize() const { return size; }

		void seek(const size_t _cursor, const std::ios_base::seekdir dir)
		{
			switch (dir)
			{
			case std::ios::beg:
			{
				cursor = _cursor;
			}
			case std::ios::cur:
			{
				cursor += _cursor;
			}
			case std::ios::end:
			{
				cursor = size - _cursor;
			}
			}
		}

		void read(char buf[], const size_t size)
		{
			std::copy(data + cursor, data + cursor + size, buf);
		}
		char* readAll(size_t& _size)
		{
			_size = size;
			char* buf = new char[size];
			std::copy(data, data + size, buf);
			return buf;
		}
		void write(const char buf[], const size_t size)
		{
			std::copy(buf, buf + size, data + cursor);
		}

		~MemFile() {}
	};

	class DiskFile : public File
	{
	public:
		std::ifstream file;
		size_t size;
		Path path;

		static constexpr std::ios_base::openmode read_mode = std::ios_base::in | std::ios_base::binary | std::ios_base::ate;

		DiskFile() = default;
		DiskFile(const Path& _path) : path(_path) {}

		cstring_view getName()
		{
			return path.generic_u8string().c_str();
		}

		bool isLoaded() { return file.is_open(); }

		bool load()
		{
			if (isLoaded())
				return true;

			file.open(path, read_mode);

			loadSize();

			return isLoaded();
		}

		void unload()
		{
			file.close();
		}

		size_t getSize() const
		{
			return size;
		}

		void seek(const size_t _cursor, const std::ios_base::seekdir dir)
		{
			file.seekg(_cursor, dir);
		}


		void read(char buf[], const size_t size)
		{
			file.read(buf, size);
		}
		char* readAll(size_t& _size)
		{
			_size = size;
			char* buf = new char[size];
			read(buf, size);
			return buf;
		}
		void write(const char buf[], const size_t size) { throw std::exception("Attempting write to read-only view of disk file!"); }

		void loadSize()
		{
			size = file.tellg();
			file.seekg(0, std::ios::beg);
		}


		static char* readFileToBuf(const Path _path, size_t& size)
		{
			std::ifstream file(_path, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
			size = file.tellg();
			file.seekg(0, std::ios::beg);

			char* buf = new char[size];

			if (file.read(buf, size))
			{
				return buf;
			}
			else
			{
				return nullptr;
			}
		}

		~DiskFile() {}
	};

	class DiskFileView : public File
	{
	public:
		std::ifstream file;
		size_t size;
		Path path;

		static constexpr std::ios_base::openmode read_mode = std::ios_base::in | std::ios_base::binary | std::ios_base::ate;

		DiskFileView() = default;
		DiskFileView(const Path& _path) : path(_path) {}

		cstring_view getName()
		{
			return path.generic_u8string().c_str();
		}

		bool isLoaded() { return file.is_open(); }

		bool load()
		{
			if (isLoaded())
				return true;

			file.open(path, read_mode);

			loadSize();

			return isLoaded();
		}

		void unload()
		{
			file.close();
		}

		size_t getSize() const
		{
			return size;
		}

		void seek(const size_t _cursor, const std::ios_base::seekdir dir)
		{
			file.seekg(_cursor, dir);
		}


		void read(char buf[], const size_t size)
		{
			file.read(buf, size);
		}
		char* readAll(size_t& _size)
		{
			_size = size;
			char* buf = new char[size];
			read(buf, size);
			return buf;
		}
		void write(const char buf[], const size_t size) { throw std::exception("Attempting write to read-only view of disk file!"); }

		void loadSize()
		{
			size = file.tellg();
			file.seekg(0, std::ios::beg);
		}


		static char* readFileToBuf(const Path _path, size_t& size)
		{
			std::ifstream file(_path, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
			size = file.tellg();
			file.seekg(0, std::ios::beg);

			char* buf = new char[size];

			if (file.read(buf, size))
			{
				return buf;
			}
			else
			{
				return nullptr;
			}
		}

		~DiskFileView() {}
	};

	class DiskFileInMem : public File
	{
	public:
		std::ifstream file;
		size_t size;
		Path path;
		Byte* data;

		static constexpr std::ios_base::openmode read_mode = std::ios_base::in | std::ios_base::binary | std::ios_base::ate;

		DiskFileView() = default;
		DiskFileView(const Path& _path) : path(_path) {}

		cstring_view getName()
		{
			return path.generic_u8string().c_str();
		}

		bool isLoaded() { return file.is_open(); }

		bool load()
		{
			if (isLoaded())
				return true;

			file.open(path, read_mode);

			loadSize();

			return isLoaded();
		}

		void unload()
		{
			file.close();
		}

		size_t getSize() const
		{
			return size;
		}

		void seek(const size_t _cursor, const std::ios_base::seekdir dir)
		{
			file.seekg(_cursor, dir);
		}


		void read(char buf[], const size_t size)
		{
			file.read(buf, size);
		}
		char* readAll(size_t& _size)
		{
			_size = size;
			char* buf = new char[size];
			read(buf, size);
			return buf;
		}
		void write(const char buf[], const size_t size) { throw std::exception("Attempting write to read-only view of disk file!"); }

		void loadSize()
		{
			size = file.tellg();
			file.seekg(0, std::ios::beg);
		}


		static char* readFileToBuf(const Path _path, size_t& size)
		{
			std::ifstream file(_path, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
			size = file.tellg();
			file.seekg(0, std::ios::beg);

			char* buf = new char[size];

			if (file.read(buf, size))
			{
				return buf;
			}
			else
			{
				return nullptr;
			}
		}



		~DiskFileView() {}
	};


	class MemMapFile : public File
	{

	};
}
*/
