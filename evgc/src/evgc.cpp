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

#include "EvgCodeIterator.h"

using namespace evg;

//todo: allow class member names to override language keywords


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

	virtual llvm::Value* codegen() {};// = 0;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
	LongLong Val;

public:
	NumberExprAST(Int Val) : Val(Val) {}

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

	// Paths relative to module root mapped to code
	std::map<Path, CodeFile> files;

	enum class Symbol
	{
		End = 256,
		Identifier,
		Number,
		Function,
		Type,

		Semicolon,
		LParen,
		RParen,

		PostIncrement,
		PostDecrement,
		Call,
		Subscript,
		GetMember,

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

	EvgCodeIterator itr; 
	EvgCodeIterator end;

	Symbol curSym;
	std::string identifierStr; // Symbol::Identifer
	LongLong curNum; // Symbol::Number

	ExprAST* prev;

	std::map<Symbol, Int> BinopPrecedence = { {Symbol::Add, 13} };
	std::map<Int, std::vector<std::string>> BinopPrecedenceOrder; // Priority level to ordered list of operators

	Int getBinOpPriority()
	{
		return BinopPrecedence[curSym];
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

			curNum = std::stoll(numStr);
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
		else if (isIn(c, '+')) // Process operator
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

	std::unique_ptr<ExprAST> parseNumberExpr() {
		auto Result = std::make_unique<NumberExprAST>(curNum);
		getNext(); // consume the number
		return std::move(Result);
	}

	std::unique_ptr<ExprAST> parseParenExpr() {
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

	std::unique_ptr<ExprAST> parseIdentifierExpr() {
		std::string IdName = identifierStr;

		getNext(); // eat identifier.

		if (curSym != Symbol::LParen) // Simple variable ref.
			return std::make_unique<VariableExprAST>(IdName);

		// Call.
		getNext(); // eat (
		std::vector<std::unique_ptr<ExprAST>> Args;
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

	std::unique_ptr<ExprAST> parsePrimary() 
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
	std::unique_ptr<ExprAST> parseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS) 
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
	std::unique_ptr<ExprAST> parseExpression() {
		auto LHS = parsePrimary();
		if (!LHS)
			return nullptr;

		return parseBinOpRHS(0, std::move(LHS));
	}

	std::unique_ptr<PrototypeAST> parsePrototype() {
		if (curSym != Symbol::Identifier)
			return std::cout << "Expected function name in prototype\n", nullptr;

		std::string FnName = identifierStr;
		getNext();

		if (curSym != Symbol::LParen)
			return std::cout << "Expected '(' in prototype\n", nullptr;

		std::vector<std::string> ArgNames;
		while (getNext(), curSym == Symbol::Identifier)
			ArgNames.push_back(identifierStr);
		if (curSym != Symbol::LParen)
			return std::cout << "Expected ')' in prototype\n", nullptr;

		// success.
		getNext(); // eat ')'.

		return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
	}

	/// definition ::= 'def' prototype expression
	std::unique_ptr<FunctionAST> parseDefinition() {
		getNext(); // eat def.
		auto Proto = parsePrototype();
		if (!Proto)
			return nullptr;

		if (auto E = parseExpression())
			return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
		return nullptr;
	}

	/// toplevelexpr ::= expression
	std::unique_ptr<FunctionAST> parseTopLevelExpr() {
		if (auto E = parseExpression()) {
			// Make an anonymous proto.
			auto Proto = std::make_unique<PrototypeAST>("__anon_expr",
				std::vector<std::string>());
			return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
		}
		return nullptr;
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
				parseDefinition();
				break;
			case Symbol::End:
				return;
			/*case tok_extern:
				HandleExtern();
				break;*/
			default:
				parseTopLevelExpr();
				break;
			}
		}
	}











	void parse(Path startFile)
	{
		var&& firstFile = files.insert({ startFile.filename(), CodeFile(startFile) });

		EvgCodeIterator itr = (Char*)firstFile.first->second.file.file->data;
		EvgCodeIterator end = (Char*)firstFile.first->second.file.file->data + firstFile.first->second.file.file->size;
		itr.ptr += 3; // Skip header

		getNext();


	}

	/*enum class cat
	{
		keyword,
		local
	};

	std::map<std::string, cat> identifiers = { };

	std::vector<ExprAST*> line; // Stage 1
	std::string identifier;



	NumberExprAST* parseNum(EvgCodeIterator& itr, const EvgCodeIterator& end)
	{
		std::string buf;
		do
		{
			buf += *itr;
			++itr;
		} while (itr != end && isdigit(*itr));

		return nullptr;//new NumberExprAST(std::stoi(buf));
	}

	// An identifier starts with an letter and ends before a space or symbol
	ExprAST* parseIdentifier(EvgCodeIterator& itr, const EvgCodeIterator& end)
	{
		std::string buf;
		do
		{
			buf += *itr;
			++itr;
		} while (itr != end && (isalpha(*itr) || isdigit(*itr)));

		auto&& is = identifiers.find(buf);

		return nullptr;
	}

	// An operator starts with a symbol and continues until it matches the longest possible operator it could be from the set
	ExprAST* parseOperator(EvgCodeIterator& itr, const EvgCodeIterator& end)
	{
		std::string buf;

		if (*itr == '+')
		{
			++itr;
			if (*itr == '=') // +=
			{

			}
			else
			{
				new BinaryExprAST(line.back(), parseTok(itr, end));
			}
		}

		do
		{
			buf += *itr;

			if (buf == "+")

			++itr;
		} while (itr != end && !(isalpha(*itr) || isdigit(*itr)));

		auto&& is = identifiers.find(buf);

		return nullptr;
	}

	void parseTok(EvgCodeIterator& itr, const EvgCodeIterator& end)
	{
		while (itr != end)
		{
			UnicodeChar c = *itr;

			if (isdigit(c))
			{
				line.push_back(parseNum(itr, end));
			}
			if (isalpha(c))
			{
				line.push_back(parseIdentifier(itr, end));
			}
			if (isIn(c, '+'))
			{
				line.push_back(parseOperator(itr, end));
			}
			if (c == ';')
			{


				DebugBreak();
			}
			else // Space, newline
			{
				++itr;
			}
		}
	}

	// Read a file without a definition
	void parse(Path startFile)
	{
		var&& firstFile = files.insert({ startFile.filename(), CodeFile(startFile) });

		EvgCodeIterator itr = (Char*)firstFile.first->second.file.file->data;
		EvgCodeIterator end = (Char*)firstFile.first->second.file.file->data + firstFile.first->second.file.file->size;
		itr.ptr += 3; // Skip header

		std::string sym;


		parseTok(itr, end);
	}*/
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