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

//todo: allow class member names to override language keywords






class EvgCodeIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = UnicodeChar;
	using difference_type = std::ptrdiff_t;
	using pointer = Char*;
	using reference = Char&;

	Char* ptr;

	//operator Char* () { return point; }
	//Char& operator* () { return *ptr; }

	//bool operator== (const EvgCodeIterator& rhs) const { return ptr == rhs.ptr; }
	//bool operator!= (const EvgCodeIterator& rhs) const { return !(*this == rhs); }


	EvgCodeIterator(Char* const _ptr) : ptr(_ptr) {}

	EvgCodeIterator& operator++ () { ptr += size(); return *this; }
	EvgCodeIterator operator++ (int) { EvgCodeIterator ret = *this; ++(*this); return ret; }

	EvgCodeIterator& operator-- () { --ptr; return *this; }
	EvgCodeIterator operator-- (int) { EvgCodeIterator ret = *this; --(*this); return ret; }

	bool operator==(EvgCodeIterator lhs) const { return ptr == lhs.ptr; }
	bool operator!=(EvgCodeIterator lhs) const { return !(*this == lhs); }

	I64 operator-(const EvgCodeIterator& rhs) const { return ptr - rhs.ptr; }
	I64 operator+(const EvgCodeIterator& rhs) const { return ptr + rhs.ptr; }

	UnicodeChar operator*() { return get(); }

	UInt size()
	{
		if (!(*ptr & 0b10000000))
			return 1;
		else if (*ptr & 0b11110000)
			return 4;
		else if (*ptr & 0b11100000)
			return 3;
		else if (*ptr & 0b11000000)
			return 2;
		else
			return 0;
	}

	UnicodeChar get()
	{
		UnicodeChar ret = 0;

		if (!(*ptr & 0b10000000))
			ret = *ptr;
		else if (*ptr & 0b11110000)
			ret = *(UnicodeChar*)ptr;
		else if (*ptr & 0b11100000)
			std::copy(ptr, ptr + 3, &ret);
		else if (*ptr & 0b11000000)
			std::copy(ptr, ptr + 2, &ret);
		else
			return std::numeric_limits<UnicodeChar>::max();

		return ret;
	}

	Char* wholeIdentifier(const Char* const end)
	{

	}

	bool nextIs(const Char* const end, std::string_view compare)
	{
		for (Size i = 0; (i < compare.size()) && (i < end - ptr) ; ++i)
		{
			if (compare[i] != ptr[i])
				return false;
		}

		return true;
	}

	bool itrUntil(const Char* const end, std::string_view compare)
	{
		for (; ptr < end; ++ptr)
		{
			if (nextIs(end, compare))
			{
				ptr += compare.size() - 1;
				return true;
			}
		}

		return false;
	}

	bool nextNonNum(const Char* const end)
	{
		for (; ptr < end; ++ptr)
		{
			if (!isdigit(*ptr))
			{
				return true;
			}
		}

		return false;
	}
};

enum class LineEnding : UInt
{
	unknown = -1,
	n = 1,
	rn = 2,
	r = 3,
};


/*
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

class LiteralStringAST : public ASTElement
{
public:
	ImString value;
};

class ExprAST {
public:
	virtual ~ExprAST() = default;

	//virtual Value* codegen() = 0;
};

class FunctionAST : public ASTElement
{
public:
	
};

class ClassMemberAST : public ASTElement
{
public:
	ClassAST* mtype;

	ExprAST* defaultVal; // Equivalent to using "mem = defaultVal" in constructor (copy or move)
};

class ClassAST : public ASTElement
{
public:
	ImString name;

	std::map<ImString, ClassMemberAST> members;
};
*/

/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
	virtual ~ExprAST() = default;

	virtual llvm::Value* codegen() = 0;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
	double Val;

public:
	NumberExprAST(double Val) : Val(Val) {}

	llvm::Value* codegen() override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
	std::string Name;

public:
	VariableExprAST(const std::string& Name) : Name(Name) {}

	llvm::Value* codegen() override;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
	char Op;
	std::unique_ptr<ExprAST> LHS, RHS;

public:
	BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
		std::unique_ptr<ExprAST> RHS)
		: Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

	llvm::Value* codegen() override;
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
	std::string Callee;
	std::vector<std::unique_ptr<ExprAST>> Args;

public:
	CallExprAST(const std::string& Callee,
		std::vector<std::unique_ptr<ExprAST>> Args)
		: Callee(Callee), Args(std::move(Args)) {}

	llvm::Value* codegen() override;
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
	std::string Name;
	std::vector<std::string> Args;

public:
	PrototypeAST(const std::string& Name, std::vector<std::string> Args)
		: Name(Name), Args(std::move(Args)) {}

	llvm::Function* codegen();
	const std::string& getName() const { return Name; }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
	std::unique_ptr<PrototypeAST> Proto;
	std::unique_ptr<ExprAST> Body;

public:
	FunctionAST(std::unique_ptr<PrototypeAST> Proto,
		std::unique_ptr<ExprAST> Body)
		: Proto(std::move(Proto)), Body(std::move(Body)) {}

	llvm::Function* codegen();
};


class TextFile
{
public:
	File* file;

	LineEnding newline = LineEnding::rn;

	// Create with filesystem path
	TextFile(Path path) : file(new MemFile(path))
	{
		newline = detectNewline();
	}

	LineEnding detectNewline()
	{
		for (anyvar c : *file) //make *file into file
		{
			
		}

		return LineEnding::rn;
	}
};










class CodeFile
{
public:
	TextFile file;

	std::list<ExprAST*> roots;

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

		EvgCodeIterator itr = firstFile.first->second.file.file->begin();

		while (itr != firstFile.first->second.file.file->end())
		{
			std::cout << *itr << '\n';
			++itr;
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

		primaryModule->parse(startFile);
	}

	// Use a single module to create and run a program
	void compileAndRun(Path startFile)
	{
		primaryModule = new Module;
		primaryModule->root = startFile.parentPath();

		primaryModule->parse(startFile);

	}
};




ProgramEnv::ExitCode EvgMain()
{
	EvgCompiler compiler;

	compiler.compileAndRun((Char*)"../tests/test.evg");


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



	return ProgramEnv::ExitCode::Success();
}