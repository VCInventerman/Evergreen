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
		String raw;

		// Returns everything after the last /
		ImString filename()
		{
			var lastSlash = raw.rfind('/');
			Size size = raw.end() - lastSlash - 1;
			ContiguousBufPtrEnd<Char> str(size);
			std::copy(lastSlash + 1, raw.end(), str.begin());
			return String((Char*)(&*str.begin()));
		}

		// Returns everything up to, and including, the last slash
		//todo: better syntax for std::copy, like [begin,end) but in this case [begin,end]
		ImString parentPath()
		{
			var lastSlash = raw.rfind('/');
			Size size = lastSlash - raw.begin() + 1; // Capture last slash
			ContiguousBufPtrEnd<Char> str(size);
			std::copy(raw.begin(), lastSlash + 1, str.begin());
			return String((Char*)(&*str.begin()));
		}

		Bool exists()
		{
			return std::filesystem::exists(raw.data());
		}

		Path() = default;
		Path(const String _raw) : raw(_raw) {}
		Path(CChar* const _raw) : raw(_raw) {}

		template<typename T>
		T operator_conv() const
		{
			return raw.operator_conv<T>();
		}

		EVG_CXX_CAST(CChar*)
		EVG_CXX_CAST_ADAPT(std::filesystem::path, CChar*)

		bool operator< (const Path& rhs) const { return raw < rhs.raw; }
	};


	class File
	{
	public:
		// constructor () -> ()
		// constructor (Path _path) -> ()

		virtual String getName() = 0;

		virtual bool isLoaded() = 0;
		virtual bool load() = 0;
		virtual void unload() = 0;

		virtual size_t getSize() const = 0;

		virtual void seek(const size_t _cursor, const std::ios_base::seekdir dir) = 0;

		virtual void read(char buf[], const size_t size) = 0;
		virtual char* readAll(size_t& _size) = 0;
		virtual void write(const char buf[], const size_t size) = 0;

		virtual Char* begin() = 0;
		virtual Char* end() = 0;

		virtual ~File() {};
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