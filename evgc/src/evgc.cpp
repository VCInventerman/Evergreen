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

#define EVG_INSERT_MAIN
#include "evglib.h"

#include "EvgCodeIterator.h"

using namespace evg;

//todo: allow class member names to language keywords
//high-precison int and float for numbers with boost


/*

enum class Symbol
{
	End = 256,
	Identifier,
	Number,
	Function,
	Typedef,

	Semicolon,
	LParen,
	RParen,
	LCBracket,
	RCBracket,

	PostIncrement,
	PostDecrement,
	Call,
	Subscript,
	GetMember,
	GetMemberPtr,

	PreIncrement,
	PreDecrement,
	UnaryPlus,
	UnaryMinus,
	LogicalNOT,
	BitwiseNOT,
	SimpleCast,
	Defererence,
	AddressOf,

	Multiply,
	Divide,
	Remainder,

	Add,
	Sub,

	BitwiseLShift,
	BitwiseRShift,

	ThreeWayCompare,

	LessThan,
	GreaterThan,
	LessOrEqual,
	GreaterOrEqual,

	Equal,
	Inequal,

	BitwiseAND,

	BitwiseXOR,

	BitwiseOR,

	LogicalAND,

	LogicalOR,

	Ternary,
	Assign,
	AssignAdd,
	AssignSub,
	AssignMultiply,
	AssignDivide,
	AssignRemainder,
	AssignBitwiseLShift,
	AssignBitwiseRShift,
	AssignBitwiseAND,
	AssignBitwiseXOR,
	AssignBitwiseOR,

	Comma
};

std::map<Symbol, Int> opPrecedence = { {Symbol::Multiply, 13}, {Symbol::Divide, 13}, {Symbol::Add, 12}, {Symbol::Sub, 12} };


/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
	virtual ~ExprAST() = default;

	llvm::Value* codegen() { return nullptr; };// = 0;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST 
{
public:
	Int val;

	NumberExprAST() = default;
	NumberExprAST(Int _val) : val(_val) {}

	llvm::Value* codegen();
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST
{
public:
	std::string name;

	VariableExprAST() = default;
	VariableExprAST(const std::string& _name) : name(_name) {}

	llvm::Value* codegen();
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST
{
public:
	Symbol op;
	UPtr<ExprAST> lhs;
	UPtr<ExprAST> rhs;

	BinaryExprAST() = default;
	BinaryExprAST(Symbol _op, UPtr<ExprAST> _lhs, UPtr<ExprAST> _rhs) : op(_op), lhs(std::move(_lhs)), rhs(std::move(_rhs)) {}

	llvm::Value* codegen();
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST
{
public:
	std::string callee;
	std::vector<UPtr<ExprAST>> args;

	CallExprAST() = default;
	CallExprAST(const std::string& _callee, std::vector<UPtr<ExprAST>> _args) : callee(_callee), args(std::move(_args)) {}

	llvm::Value* codegen();
};

class FunctionParamAST
{
public:
	std::string type;
	std::string name;
	void* defaultVal;
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST 
{
public:
	std::string name;
	std::vector<FunctionParamAST> args;
	std::vector<FunctionParamAST> returns;
	UPtr<ExprAST> body;
	bool defined;

	FunctionAST() = default;
	FunctionAST(std::string& _name, std::vector<std::string> _args, UPtr<ExprAST> _body, bool _defined) : name(_name), args(std::move(_args)), body(std::move(_body)) {}

	llvm::Function* codegen();
};




class TextFile
{
public:
	MemFile* file;

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


template<typename First, typename ... T>
bool isIn(First&& first, T && ... t)
{
	return ((first == t) || ...);
}







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

	// Paths relative to module root : code
	std::map<Path, CodeFile> files;



	EvgCodeIterator itr;
	EvgCodeIterator end;

	Symbol curSym;
	std::string identifierStr; // Symbol::Identifer
	Int curNum; // Symbol::Number

	Int getBinOpPriority()
	{
		return opPrecedence[curSym];
	}

	// Read next symbol from file
	void getNext()
	{
		UnicodeChar c = *itr;

		while ((itr != end) && (*itr == ' ')) // Skip whitespace
		{
			++itr;
			c = *itr;
		}

		if (isalpha(c)) // Identifiers start with an alphabetic character (no emojis)
		{
			identifierStr = c;

			while (isalnum(c = *(++itr)))
			{
				identifierStr += c;
			}

			if (identifierStr == "fn")
				curSym = Symbol::Function;
			else if (identifierStr == "type")
				curSym = Symbol::Type;
			else
				curSym = Symbol::Identifier;
		}
		else if (isdigit(c) || (c == '.')) //todo: only floating-point numbers can begin with .
		{
			std::string numStr;
			do 
			{
				numStr += c;
				c = *(++itr);
			} while (isdigit(c) || c == '.');

			curNum = std::stoi(numStr);
			curSym = Symbol::Number;
		}
		else if (c == '/') {
			if (itr.next() == '/') // Comment until end of line
			{
				while ((itr != end) && (c != '\n' && c != '\r'))
				{
					++itr;
					c = *itr;
				}

				getNext();
			}
			else if (itr.next() == '*')
			{
				while ((itr != end) && (c != '*') && (itr.next() != '/'))
				{
					++itr;
					c = *itr;
				}

				getNext();
			}
		}
		else if (isIn(c, '+', '-')) // Process operator
		{

		}
		else if (c == '(')
		{
			curSym = Symbol::LParen;
		}
		else if (c == ')')
		{
			curSym = Symbol::RParen;
		}
		else if (c == ',')
		{
			curSym = Symbol::Comma;
		}
		else if (itr == end)
		{
			curSym = Symbol::End;
		}
	}

	UPtr<ExprAST> parseNumberExpr() {
		auto Result = std::make_unique<NumberExprAST>(curNum);
		getNext(); // consume the number
		return std::move(Result);
	}

	UPtr<ExprAST> parseParenExpr() {
		getNext(); // eat (.
		auto V = parseExpression();
		if (!V)
			return nullptr;

		if (curSym != Symbol::RParen)
		{
			std::cout << "expected ')'\n";
			return nullptr;
		}
			
		getNext(); // eat ).
		return V;
	}

	UPtr<ExprAST> parseIdentifierExpr() {
		std::string IdName = identifierStr;

		getNext(); // eat identifier.

		if (curSym != Symbol::LParen) // Simple variable ref.
			return std::make_unique<VariableExprAST>(IdName);

		// Call.
		getNext(); // eat (
		std::vector<UPtr<ExprAST>> Args;
		if (curSym != Symbol::RParen) {
			while (true) {
				if (auto Arg = parseExpression())
					Args.push_back(std::move(Arg));
				else
					return nullptr;

				if (curSym == Symbol::RParen)
					break;

				if (curSym != Symbol::Comma)
				{
					std::cout << "Expected ')' or ',' in argument list\n";
					return nullptr;
				}
				getNext();
			}
		}

		// Eat the ')'.
		getNext();

		return std::make_unique<CallExprAST>(IdName, std::move(Args));
	}

	UPtr<ExprAST> parsePrimary() 
	{
		switch (curSym) 
		{
		default:
		{
			std::cout << "unknown token when expecting an expression\n";
			return nullptr;
		}
		case Symbol::Identifier:
			return parseIdentifierExpr();
		case Symbol::Number:
			return parseNumberExpr();
		case Symbol::LParen:
			return parseParenExpr();
		}
	}

	/// binoprhs
	///   ::= ('+' primary)*
	UPtr<ExprAST> parseBinOpRHS(int ExprPrec, UPtr<ExprAST> LHS) 
	{
		// If this is a binop, find its precedence.
		while (true) {
			int TokPrec = getBinOpPriority();

			// If this is a binop that binds at least as tightly as the current binop,
			// consume it, otherwise we are done.
			if (TokPrec < ExprPrec)
				return LHS;

			// Okay, we know this is a binop.
			Symbol BinOp = curSym;
			getNext(); // eat binop

			// Parse the primary expression after the binary operator.
			auto RHS = parsePrimary();
			if (!RHS)
				return nullptr;

			// If BinOp binds less tightly with RHS than the operator after RHS, let
			// the pending operator take RHS as its LHS.
			int NextPrec = getBinOpPriority();
			if (TokPrec < NextPrec) {
				RHS = parseBinOpRHS(TokPrec + 1, std::move(RHS));
				if (!RHS)
					return nullptr;
			}

			// Merge LHS/RHS.
			LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
		}
	}

	/// expression
///   ::= primary binoprhs
///
	UPtr<ExprAST> parseExpression() {
		auto LHS = parsePrimary();
		if (!LHS)
			return nullptr;

		return parseBinOpRHS(0, std::move(LHS));
	}

	UPtr<PrototypeAST> parsePrototype() {





		
		while (getNext(), curSym == Symbol::Identifier)
			ArgNames.push_back(identifierStr);
		if (curSym != Symbol::LParen)
			return std::cout << "Expected ')' in prototype\n", nullptr;

		// success.
		getNext(); // eat ')'.

		return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
	}

	/// definition ::= 'def' prototype expression
	UPtr<FunctionAST> parseFunction() {
		getNext(); // get symbol after fn
		auto Proto = parsePrototype();

		if (curSym != Symbol::Identifier)
			return std::cout << "Expected function name in prototype\n", nullptr;

		auto func = std::make_unique<FunctionAST>();
		func->name = identifierStr;
		getNext();

		if (curSym == Symbol::LParen) // Function has parameters
		{
			getNext();
			if (curSym == Symbol::RParen) // No arguments
			{

			}
			else // Must be type of first parameter
			{
				func->args.push_back({});
				func->args.back().type = identifierStr;
				getNext();
				func->args.back().name = identifierStr;
				getNext();
				if (curSym == Symbol::Assign) //todo: default value
				{
					getNext();
					func->args.back().defaultVal == identifierStr.c_str();
					getNext();
				}

				while (curSym == Symbol::Comma)
				{
					func->args.push_back({});
					func->args.back().type = identifierStr;
					getNext();
					func->args.back().name = identifierStr;
					getNext();
				}

				func->args.push_back({});
				func->args.back().type = identifierStr;
				getNext();
				func->args.back().name = identifierStr;
				getNext(); // Consume )
				getNext();
			}

			if (curSym == Symbol::GetMemberPtr) // Has return value
			{
				getNext(); // Must be (
				func->returns.push_back({});
				func->returns.back().type = identifierStr;
				getNext();
				func->returns.back().name = identifierStr;
				getNext();
				if (curSym == Symbol::Assign) //todo: default value
				{
					getNext();
					func->returns.back().defaultVal == identifierStr.c_str();
					getNext();
				}

				while (curSym == Symbol::Comma)
				{
					func->returns.push_back({});
					func->returns.back().type = identifierStr;
					getNext();
					func->returns.back().name = identifierStr;
					getNext();
				}

				func->returns.push_back({});
				func->returns.back().type = identifierStr;
				getNext();
				func->returns.back().name = identifierStr;
				getNext(); // Must be )
				getNext();
			}
		}

		if (curSym == Symbol::LCBracket) // Defined
		{
			func->defined = true;
			getNext();
			parseExpression();
		}
		else if (curSym == Symbol::Semicolon)
		{
			func->defined = false;
		}

		return func;
	}

	void processLoop()
	{
		while (itr != end)
		{
			switch (curSym) {
			case Symbol::Semicolon:
				getNext();
				break;
			case Symbol::Function:
				parseFunction();
				break;
			case Symbol::End:
				return;
			default:
				//top level expression
				break;
			}
		}
	}











	void parse(Path startFile)
	{
		var&& firstFile = files.insert({ startFile.filename(), CodeFile(startFile) });

		EvgCodeIterator itr = (Char*)firstFile.first->second.file.file->data;
		EvgCodeIterator end = (Char*)firstFile.first->second.file.file->data + firstFile.first->second.file.file->size + 1;
		itr.ptr += 3; // Skip magic file encoding header

		getNext();
		processLoop();

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


*/

bool isPowerOfTen(UInt64 input) {
	return
		input == 1ULL
		|| input == 10ULL
		|| input == 100ULL
		|| input == 1000ULL
		|| input == 10000ULL
		|| input == 100000ULL
		|| input == 1000000ULL
		|| input == 10000000ULL
		|| input == 100000000ULL
		|| input == 1000000000ULL
		|| input == 10000000000ULL
		|| input == 100000000000ULL
		|| input == 1000000000000ULL
		|| input == 10000000000000ULL
		|| input == 100000000000000ULL
		|| input == 1000000000000000ULL
		|| input == 10000000000000000ULL
		|| input == 100000000000000000ULL
		|| input == 1000000000000000000ULL;
}

struct ipos
{
	int x, y, z;

	ipos(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

	inline bool operator==(const ipos& a) const
	{
		return (x == a.x && y == a.y && z == a.z);
	}
};

struct iposClusterHash
{
	std::size_t operator()(const ipos& pos) const
	{
		return (pos.x * 88339) ^ (pos.z * 91967) ^ (pos.z * 126323);
	}


	static inline ipos getEmptyKey() { return { ~0, ~0, ~0 }; }
	static inline ipos getTombstoneKey() { return { ~0 - 1,  ~0 - 1,  ~0 - 1 }; }

	static Size getHashValue(const ipos& pos)
	{
		return (pos.x * 88339) ^ (pos.z * 91967) ^ (pos.z * 126323);
	}

	static bool isEqual(const ipos& lhs, const ipos& rhs)
	{
		return lhs == rhs;
	}
};

ExitCode EvgMain()
{
	//EvgCompiler compiler;

	//compiler.compileAndRun((Char*)"../tests/test.evg");

	llvm::LLVMContext context;
	llvm::IRBuilder<> builder(context);
	//UPtr<llvm::Module> mod = std::make_unique<llvm::Module>("EvgCompiler", context);
	llvm::Module* mod = new llvm::Module("EvgCompiler", context);

	std::vector<llvm::Type*> params(2, llvm::Type::getInt32Ty(context));
	llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(context), params, false);

	llvm::Function* F = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "TestFunc", mod);





	mod->print(llvm::outs(), nullptr);

	return ExitSuccess;
}