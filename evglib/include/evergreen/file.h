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

		using value_type = char;
		using allocator_type = std::allocator<char>;
		using size_type = Size;
		using difference_type = Offset;
		using reference = char&;
		using const_reference = const char&;
		using iterator = RandomContigIterator<const char>;
		using const_iterator = RandomContigIterator<const char>;
		using reverse_iterator = RevRandomContigIterator<const char>;
		using const_reverse_iterator = RevRandomContigIterator<const char>;


	protected:
		String string_;
		StringBuilderBase<WChar> win32Cache_;

	public:
		// Returns everything after the last /
		ImString filename()
		{
			Size lastSlash = string_.rfind('/');

			if (lastSlash == string_.npos)
				return "";

			return StringBuilder(string_.begin() + lastSlash + 1, string_.end());
		}

		// Returns everything up to, but not including, the last slash
		//todo: better syntax for std::copy, like [begin,end) but in this case [begin,end]
		ImString parent()
		{
			auto lastSlash = string_.rfind('/');

			if (lastSlash == string_.npos)
				return "";

			return StringBuilder(string_.begin(), string_.begin() + lastSlash);
		}

#if defined(EVG_PLATFORM_WIN) && defined(EVG_COMPILE_AOT)
		bool exists()
		{
			DWORD attributes = GetFileAttributesW(getWin32());
			return attributes != INVALID_FILE_ATTRIBUTES;
		}

		bool isFile()
		{
			DWORD attributes = GetFileAttributesW(getWin32());
			return (attributes != INVALID_FILE_ATTRIBUTES) && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		bool isDir()
		{
			DWORD attributes = GetFileAttributesW(getWin32());
			return (attributes != INVALID_FILE_ATTRIBUTES) && (attributes & FILE_ATTRIBUTE_DIRECTORY);
		}
#elif defined(EVG_COMPILE_AOT)
		bool exists()
		{
			struct stat st;
			return bool(stat(string_.data(), &st));
		}

		bool isFile()
		{
			struct stat st;
			bool exist = stat(string_.data(), &st);
			return exist && ((st.st_mode & S_IFMT) == S_IFREG);
		}

		bool isDir()
		{
			struct stat st;
			bool exist = stat(string_.data(), &st);
			return exist && ((st.st_mode & S_IFMT) == S_IFDIR);
		}
#else
		bool exists();
#endif

		UtcTime modifyTime()
		{
#ifdef EVG_PLATFORM_WIN
			return std::chrono::clock_cast<std::chrono::system_clock, std::chrono::file_clock, std::chrono::system_clock::duration>(
				std::filesystem::last_write_time(std::filesystem::path(string_.data())));
#else
			//return std::filesystem::last_write_time(std::filesystem::path(string_.data()));
			return{};
#endif
		}

		const WChar* getWin32()
		{
			if (size() != win32Cache_.size())
			{
				utf8ToUtf16(string_, win32Cache_);
				win32Cache_.replaceAll('/', '\\');
			}

			return win32Cache_.data();
		}

		void resetWin32()
		{
			win32Cache_.clear();
		}

#if defined(EVG_PLATFORM_WIN)
		explicit operator const WChar* () { return getWin32(); }

		const WChar* native() { return getWin32(); }
#else
		explicit operator const char* () { return data(); }

		const char* native() { return data(); }
#endif


		Path() : string_() {}
		Path(const Path& rhs) : string_(rhs.string_) {}
		Path(Path&& rhs) : string_(std::move(rhs.string_)) {}
		Path(const String str) : string_(str) {}
		Path(const StringBuilder& str) : string_(str) {}
		Path(StringBuilder&& str) : string_(std::move(str)) {}
		Path(const char* const str) : string_(str) {}

		// Constructor accepting a wide string
		// Since this is intended for use with Win32, it converts back slashes into forward slashes
		Path(const WChar* const _str)
		{
			StringBuilder builder;
			utf16ToUtf8(ContiguousBufPtrEnd<const WChar>(_str, strlen(_str)), builder);
			builder.replaceAll('\\', '/');

			builder.ensureNullTerminated();
			string_ = std::move(builder);
		}


		Path& operator= (const Path& rhs) { this->string_ = rhs.string_; resetWin32(); return *this; }
		Path& operator= (Path&& rhs) 
		{ 
			this->string_ = std::move(rhs.string_); 
			resetWin32(); 
			return *this; 
		}
		Path& operator= (const String rhs) { resetWin32(); return *this = std::move(Path(rhs)); }
		Path& operator= (const evg::StringBuilder& rhs) { resetWin32(); return *this = std::move(Path(rhs)); }
		Path& operator= (evg::StringBuilder&& rhs) 
		{ 
			resetWin32(); 
			*this = std::move(Path(std::move(rhs))); 
			return *this;
		}
		Path& operator= (const char* const rhs) { resetWin32(); return *this = std::move(Path(rhs)); }
		Path& operator= (const WChar* const rhs) { resetWin32(); return *this = std::move(Path(rhs)); }


		~Path()
		{
			//if (string_.valid())
			//	logInfo("Destroying path ", *this);
		}



		Path operator+= (const Path& rhs)
		{
			string_ += rhs.string_;
			resetWin32();
		}

		/*template<typename T>
		T operator_conv() const
		{
			return string_.operator_conv<T>();
		}

		EVG_CXX_CAST(CChar*)*/
		//EVG_CXX_CAST_ADAPT(std::filesystem::path, CChar*)
		explicit operator ContiguousBufPtrEnd<const char>() const { return range(); }
		explicit operator std::string() const { return string_.data(); }

		friend std::ostream& operator<< (std::ostream& stream, const Path& rhs)
		{
			stream << rhs.string_;
			return stream;
		}

		bool operator< (const Path& rhs) const { return string_ < rhs.string_; }

		ContiguousBufPtrEnd<CChar> range() const { return string_.range(); }
		CChar* data() const { return string_.data(); }
		const_iterator begin() const { return range().begin(); }
		const_iterator end() const { return range().end(); }
		const_iterator cbegin() const { return range().cbegin(); }
		const_iterator cend() const { return range().cend(); }
		const_reverse_iterator rbegin() const { return range().rbegin(); }
		const_reverse_iterator rend() const { return range().rend(); }
		const_reverse_iterator crbegin() const { return range().crbegin(); }
		const_reverse_iterator crend() const { return range().crend(); }
		CChar& back() const { return *(end() - 1); }

		Size size() const { return string_.size(); }
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
		KNOWNFOLDERID refId = "{62AB5D82-FDC1-4DC3-A9DD-070D1D495D97}"_guid; // FOLDERID_ProgramData;
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
		return "/etc";
	}

	// Linux doesn't have one by default
	Path getPublicDataFolder()
	{
		// return "/var";
		return "";
	}

	Path getUserDataFolder()
	{
		return "";
	}
#else
	Path getAdminDataFolder();
	Path getPublicDataFolder();
	Path getUserDataFolder();
#endif

	Path getExecParentFolder();


	std::vector<char> simpleFileRead(Path path)
	{
		try
		{
			std::ifstream file(path.native(), std::ifstream::binary | std::ifstream::ate);
			auto n = file.tellg();
			std::vector<char> vec(file.tellg());
			file.seekg(0);
			file.read(vec.data(), vec.size());
			return vec;
		}
		catch (...)
		{
			logError("Simple file read failed at ", path);
			return {};
		}
	}

	/*inline Time getTime()
	{


		return Clock::now();
	}*/


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

		/*virtual char* begin() = 0;
		virtual char* end() = 0;

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
			return String((char*)"MemFile");
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

		char* begin()
		{
			return (char*)data;
		}
		char* end()
		{
			return (char*)data + size;
		}
	};*/



	// Iterate over the contents of a file
	// Does not support concurrency: todo: boost strand
	class IterFile
	{
	public:

#if defined(EVG_PLATFORM_WIN)
		enum class Permission : Int
		{
			Read = (Int)GENERIC_READ,
			Write = (Int)GENERIC_WRITE,
			Execute = (Int)GENERIC_EXECUTE,
			RW = Read | Write,
			All = (Int)GENERIC_ALL,
		};

		typedef void (*CompletionHandler)(void*, LPOVERLAPPED, const std::error_code&, Size);
#else
		enum class Permission : Int
		{
			Read = (Int)O_RDONLY,
			Write = (Int)O_WRONLY,
			RW = O_RDWR,
		};

#endif



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

		using value_type = char;
		using size_type = Size;
		using difference_type = Offset;
		using reference = char&;
		using const_reference = const char&;
		using iterator = Iterator;
		using const_iterator = const Iterator;
		using reverse_iterator = InvalidType;
		using const_reverse_iterator = InvalidType;


		constexpr static Size bufSize = 4096; // block size on my system




#if defined(EVG_PLATFORM_WIN)
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
				debugBreak();
			}


			Int32 share = ((Int)perm & (Int)GENERIC_WRITE) ? 0 : (Int)FILE_SHARE_READ; // Allow other processes to read from this file if we don't write to it
			Int32 openPolicy = ((Int)perm & (Int)GENERIC_WRITE) ? (Int)CREATE_NEW : (Int)OPEN_EXISTING; // Allow creation of new file if we are writing to it, otherwise do not

			fileHandle = CreateFileW(path.native(), (Int)perm, (Int)share, NULL, (Int)openPolicy,
				FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED /*| FILE_FLAG_NO_BUFFERING*/, NULL);

			HANDLE completionHandle = CreateIoCompletionPort(fileHandle, threads.native(), 0, 0); // Associate file with thread pool


			if ((fileHandle == INVALID_HANDLE_VALUE) || (completionHandle == INVALID_HANDLE_VALUE))
			{
				debugBreak();

				//todo: custom error type containing result from GetLastError()
			}


			DWORD highOrderSize = 0;
			fileSize = GetFileSize(fileHandle, &highOrderSize);
			fileSize |= Size(highOrderSize) << 32;
		}

		~IterFile()
		{
			VirtualFree(buf, 0, MEM_RELEASE);
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
#else
		aiocb control;
		long ready;

		AsyncFileOperation op;
		char* buf;
		Path path;
		Permission perm;
		int file;
		Size fileSize;


		std::coroutine_handle<> coHandle;


		IterFile(const Path _path, const Permission _perm = Permission::Read) : path(std::move(_path)), perm(_perm)
		{
			buf = (char*)mmap(0, bufSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_UNINITIALIZED, -1, 0);

			if (buf == nullptr)
			{
				debugBreak();
			}

			int flags = (Int)perm | O_LARGEFILE;
			file = open(path.data(), flags);

			struct stat st;
			stat(path.data(), &st);
			fileSize = st.st_size;
		}

		~IterFile()
		{
			munmap(buf, bufSize);

		}




		AsyncFileOperation loadBuf(const Size blockPos)
		{
			ready = 0;
			memset(&control, 0, sizeof(control));
			control.aio_fildes = file;
			control.aio_offset = blockPos;
			control.aio_buf = buf;
			control.aio_nbytes = bufSize;
			//control.aio_sigevent.sigev_notify = SIGEV_

			aio_read(&control);

			debugBreak();


			return op;
		}


		/*static void onCompletion(void*, LPOVERLAPPED overlap, const std::error_code& e, Size size)
		{
			IterFile* file = (IterFile*)overlap;

			threads.post([file, e]()
				{
					file->op.res = e.value();
					file->coHandle();
				});
		}*/
#endif // End linux implementation



	};
}
