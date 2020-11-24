#pragma warning (disable : 4996)
#pragma warning (disable : 4146)

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

#include "evglib.h"


using namespace evg;



template <typename T>
class ContiguousRange
{
public:
	T begin;
	T end;
};

template <int begin, int end, typename CountT = int>
class RangeT
{
public:


	Range()
};

template<typename T>
class ContiguousIterator
{
public:
	T* data;
	T* end;
};




class EvgCodeIterator
{
public:
	Char* point;

	EvgCodeIterator(Char* _point) : point(_point) {}

	operator Char* () { return point; }

	void next(const Char* const end)
	{
		point += size(end);
	}

	UInt size(const Char* const end)
	{
		if (!(*point & 0b10000000))
			return 1;
		else if (*point & 0b11110000)
			return 4;
		else if (*point & 0b11100000)
			return 3;
		else if (*point & 0b11000000)
			return 2;
		else
			return 0;
	}

	UnicodeChar get(const Char* const end)
	{
		UnicodeChar ret = 0;

		if (!(*point & 0b10000000))
			ret = *point;
		else if ((*point & 0b11110000) && (point + 4 <= end))
			ret = *(UnicodeChar*)point;
		else if ((*point & 0b11100000) && (point + 3 <= end))
			std::copy(point, point + 3, &ret);
		else if ((*point & 0b11000000) && (point + 2 <= end))
			std::copy(point, point + 2, &ret);
		else
			return std::numeric_limits<UnicodeChar>::max();

		return ret;
	}

	Char* wholeIdentifier(const Char* const end)
	{

	}

	bool nextIs(const Char* const end, std::string_view compare)
	{
		for (Size i = 0; (i < compare.size()) && (i < end - point) ; ++i)
		{
			if (compare[i] != point[i])
				return false;
		}

		return true;
	}

	bool itrUntil(const Char* const end, std::string_view compare)
	{
		for (; point < end; ++point)
		{
			if (nextIs(end, compare))
			{
				point += compare.size() - 1;
				return true;
			}
		}

		return false;
	}

	bool nextNonNum(const Char* const end)
	{
		for (; point < end; ++point)
		{
			if (!isdigit(*point))
			{
				return true;
			}
		}

		return false;
	}
};

enum LineEnding : UInt
{
	unknown = -1,
	n = 1,
	rn = 2,
	r = 3,
};

class ASTElement
{
public:
	StringView text;
};

class LiteralIntegerAST : public ASTElement
{
public:
	boost::multiprecision::cpp_int val;
};

class LiteralFloatAST : public ASTElement
{
public:
	boost::multiprecision::cpp_bin_float_100 val;
};

class FunctionAST : public ASTElement
{
public:
	
};

class ClassAST : public ASTElement
{
public:

};

class TextFile
{
public:
	File* file;

	LineEnding newline = LineEnding::rn;

	// Create with filesystem path
	TextFile(Path path) : file(new DiskFileInMem(path)) 
	{
		newline = detectNewline();
	}

	LineEnding detectNewline()
	{
		for (var&& c : *file) //make *file into file
		{

		}
	}
};







template<typename T, typename BufSize = Size>
class VectorView
{
public:
	T* data;
	BufSize size;


};







class CodeFile
{
public:
	TextFile file;

	std::list<ASTElement*> roots;

	CodeFile(Path path) : file(path) {}
};

// Library
class Module
{
public:
	Path root;

	// Paths relative to module root mapped to code
	std::map<Path, CodeFile> files;

	// Read a file without a definition
	void parse(Path startFile)
	{
		var&& firstFile = files.insert({ startFile.filename(), CodeFile(startFile) });

		EvgCodeIterator itr = firstFile.first->second.file.file.begin();

		while (itr != firstFile.first->second.file.file.end())
		{

		}
	}
};

class EvgCompiler
{
public:
	std::map<ImString, Module> modules;

	Module* primaryModule;

	// Create a native binary using a single path
	void compileToNative(Path startFile)
	{
		primaryModule = new Module;
		primaryModule->root = startFile.parentPath();

		primaryModule->parse(startFile.filename());
	}

	// Use a single module to create and run a program
	void compileAndRun(Path startFile)
	{
		primaryModule = new Module;
		primaryModule->root = startFile.parentPath();

		primaryModule->parse(startFile.filename());

	}
};




ProgramEnv::ExitCode main(Int argc, Char** argv) // Wrapper
{
	thisProgram.setArgs(argc, argv);
	ProgramEnv::ExitCode code = EvgMain();
	thisProgram.callAtExit();
	return code;
}

Int EvgMain()
{
	//EvgCompiler compiler;

	//compiler.compileAndRun("../tests/test.evg");


	/*
	std::ifstream fileRaw("../tests/test.evg", std::ifstream::in | std::ifstream::ate | std::ifstream::binary);
	Size size = fileRaw.tellg();
	fileRaw.seekg(std::ifstream::beg);

	std::basic_string<Char, std::char_traits<Char>, std::allocator<Char>> source;
	source.resize(size);
	fileRaw.read((char*)source.data(), size);

	CharIterator cItr = (Char*)&*source.begin();
	if ((source.size() >= 4) && (source[0] == 0x00) && (source[1] == 0x00) && (source[2] == 0xfe) && (source[3] == 0xff))
		std::cout << "UTF-32BE is not supported";
	else if ((source.size() >= 4) && (source[0] == 0xff) && (source[1] == 0xfe) && (source[2] == 0x00) && (source[3] == 0x00))
		std::cout << "UTF-32LE is not supported";
	else if ((source.size() >= 3) && (source[0] == 0xef) && (source[1] == 0xbb) && (source[2] == 0xbf))
	{
		cItr.point += 3;
	}
	else if ((source.size() >= 2) && (source[0] == 0xfe) && (source[1] == 0xff))
		std::cout << "UTF-16BE is not supported";
	else if ((source.size() >= 2) && (source[0] == 0xff) && (source[1] == 0xfe))
		std::cout << "UTF-16LE is not supported";

	for (Char* end = (Char*)&source.back() + 1; cItr < (Char*)&source.back() + 1; cItr.next((Char*)&source.back() + 1), end = (Char*)&source.back() + 1)
	{
		UnicodeChar c = cItr.get(end);

		switch (c)
		{
		case ' ': break;
		case '\n': break;
		case '\r': ++cItr.point; break;

		case '/':
		{
			if (cItr.nextIs(end, "//"))
			{
				cItr.itrUntil(end, "\r\n");
			}
			if (cItr.nextIs(end, "/*"))
			{
				//cItr.itrUntil(end, "//"); add back star for this one
			}
		}
		}

		if (isdigit(c))
		{
			 
		}
	}
	*/



	return EXIT_SUCCESS;
}