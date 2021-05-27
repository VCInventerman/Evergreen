#pragma once

#include "evergreen/common/types.h"

#include "evergreen/string/String.h"

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
		using Iterator = RandomContigIterator<CChar>;
		using RevIterator = RevRandomContigIterator<CChar>;


	public: // Access is discouraged
		String string_raw;
		std::filesystem::path cache_raw; // Some filesystem functions require a path formatted as the std version and with UTF-16 and a null terminator

	public:
		// Returns everything after the last /
		ImString filename()
		{
			Size lastSlash = string_raw.rfind('/');
			Size size = lastSlash - 1;
			ContiguousBufPtrEnd<Char> str(size);
			std::copy(string_raw.begin() + lastSlash + 1, string_raw.end(), str.begin());
			return String((Char*)(&*str.begin()));
		}

		// Returns everything up to, and including, the last slash
		//todo: better syntax for std::copy, like [begin,end) but in this case [begin,end]
		ImString parentPath()
		{
			auto lastSlash = string_raw.rfind('/');
			Size size = lastSlash + 1; // Capture last slash
			ContiguousBufPtrEnd<Char> str(size);
			std::copy(string_raw.begin() + lastSlash + 1, string_raw.end(), str.begin());
			return String((Char*)(&*str.begin()));
		}

		Bool exists()
		{
			if ((size() != 0) && emptyCache())
				updateCache();
			return std::filesystem::exists(cache_raw);
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

		Path() = default;
		Path(const String _raw) : string_raw(_raw) {}
		Path(const StringBuilder& _raw) : string_raw(_raw) {}
		Path(CChar* const _raw) : string_raw(_raw) {}
		Path(const Path& rhs) : string_raw(rhs.string_raw) {}
		Path(const Path&& rhs) : string_raw(std::move(rhs.string_raw)) {}

		Path operator+= (const Path& rhs)
		{
			string_raw += rhs.string_raw;
			invalidateCache();
		}

		void invalidateCache()
		{
			cache_raw = "";
		}

		void updateCache()
		{
			cache_raw = std::string(data(), size());
		}

		bool emptyCache()
		{
			return cache_raw.empty();
		}

		template<typename T>
		T operator_conv() const
		{
			return string_raw.operator_conv<T>();
		}

		EVG_CXX_CAST(CChar*)
		EVG_CXX_CAST_ADAPT(std::filesystem::path, CChar*)

		bool operator< (const Path& rhs) const { return string_raw < rhs.string_raw; }

		Path& operator= (const evg::Path& rhs) { this->string_raw = rhs.string_raw; }
		Path& operator= (const evg::StringBuilder& rhs) { this->string_raw = std::move(rhs.data()); }
		Path& operator= (CChar* rhs) { return *this = Path(rhs); }

		ContiguousBufPtrEnd<CChar> data() const { return string_raw.data(); }
		Iterator begin() const { return data().begin(); }
		Iterator end() const { return data().end(); }
		RevIterator rbegin() const { return data().rbegin(); }
		RevIterator rend() const { return data().rend(); }
		CChar& back() const { return *(end() - 1); }

		Size size() const { return string_raw.size(); }
	};


	class File
	{
	public:
		// constructor () -> ()
		// constructor (Path _path) -> ()

		virtual ~File() {};

		virtual String getName() = 0;

		virtual bool isLoaded() = 0;
		virtual bool load() = 0;
		virtual void unload() = 0;

		virtual size_t getSize() const = 0;

		virtual void seek(const size_t _cursor, const std::ios_base::seekdir dir) = 0;

		virtual void readBuf(ContiguousBufPtrEnd<char> buf) = 0;
		virtual char* readAll(size_t& _size) = 0;
		virtual void write(const char buf[], const size_t size) = 0;

		virtual Char* begin() = 0;
		virtual Char* end() = 0;

		template<typename InT = char, typename OutT = InT>
		OutT read()
		{
			char buf[sizeof(InT)];
			readBuf(buf);
			InT* in = (InT*)buf;
			return (OutT)(*in);
		}
	};


	class MemFile : public File
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
			size = std::filesystem::file_size(conv(path, std::filesystem::path));
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