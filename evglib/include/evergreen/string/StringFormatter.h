#pragma once

#include "evergreen/common/ContiguousBuf.h"
#include "evergreen/containers/Vector.h"

namespace evg
{
	namespace ast
	{
		class Expression;

		class Scope
		{
		public:
			Expression* parent = nullptr;
			Vector<Expression*> members;

			Scope(Expression* _parent = nullptr) : parent(_parent) {}

			Expression* find(const String name);

			void sort();

			bool insert(Expression* const element)
			{
				if (contains(members, element))
					return false;
				else
				{
					members.push_back(element);
					return true;
				}
			}

		};

		class Expression // Base class
		{
		public:
			String name;
			Scope scope;

			virtual ~Expression() = default;

			//llvm::Value* codegen() { return nullptr; } // = 0;
		};

		Expression* Scope::find(const String name)
		{
			Scope* current = this;
			Vector<Expression*>::iterator ret;

			if ((parent == nullptr) && (members.size() == 0)) { return nullptr; }

			do
			{
				ret = std::lower_bound(members.begin(), members.end(), name,
				[](Expression* const lhs, const String& rhs) { return lhs->name < rhs; });

				if (ret != members.end()) { return *ret; }

				current = dynamic_cast<Scope*>((Expression*)current->parent);
			} while (current != nullptr);

			return nullptr;
		}

		void Scope::sort() 
		{
			std::sort(members.begin(), members.end(), [](Expression* lhs, Expression* rhs) { return lhs->name < rhs->name; });
		}
		

		class Number : public Expression
		{
		public:

		};

		class Variable : public Expression
		{
		public:

		};


	}

	template<typename T, typename BufT>
	Size findPairEnd(const T left, const T right, BufT&& buf)
	{
		Size closePos = 0; // Target left side is always 0
		Size counter = 1;

		while (counter > 0) 
		{
			char c = buf[++closePos];
			if (c == left) {
				++counter;
			}
			else if (c == right) {
				--counter;
			}
		}

		return closePos;
	}


	// Interpret the contents of a string as if it was code.
	// Keeps an internal state of relevant variables and allows Evergreen syntax, along with C style escape characters
	// Example: (in a YAML config file)
	// "{5 + baseLocation}, {pow(2, 4)}"  would map to: "55, 16"
	class StringF
	{
	public:


	public: // Access is discouraged
		ast::Scope* root;
		//std::map<String, String> variables;

	public:
		template<typename ... Ts>
		StringBuilder f(Ts const& ... args)
		{
			StringBuilder ret;
			ret.reserve((args.size()) ...);
			((ret += args), ...);
			format(ret);
		}

		// Only slices that are between pairs of { and } will be interpreted. Escape out brackets using \{ and \}
		StringBuilder format(const StringView base)
		{
			Size left = base.find('{');
			Size right = 0;

			if (left == StringView::npos)
				return base;

			right = findPairEnd('{', '}', base.slice(left));

			





			return "";
		}

		ast::Scope parse(const StringView code)
		{
			ast::Scope ret;

			code.begin();
		}

		StringBuilder eval(const ast::Scope& code)
		{
			code.members.cbegin();
		}
	};








	/*
	class StringFormatter
	{
	public:
		std::map<std::string_view, std::function<void(std::string& out)>> keys;

		static StringFormatter standard;

		void setStringKey(const std::string_view key, const std::string val)
		{
			keys[key] = [val](std::string& out) { out += val; };
		}

		void setFuncKey(const std::string_view key, std::function<void(std::string& out)> func)
		{
			keys[key] = func;
		}

		void processParam(const std::string_view param, std::string& out)
		{
			const auto itr = keys.find(param);

			if (itr != keys.cend())
				itr->second(out);
		}

		void format(const std::string_view in, std::string& out)
		{
			std::string key;
			size_t start = std::string::npos;

			for (size_t i = 0; i < in.size(); i++)
			{
				if ((i + 1 < in.size()) && (in[i] == '<'))
				{
					if (in[i + 1] == '<')
					{
						out += '<';
						++i;
					}
					else
						start = i + 1;
				}
				else if (in[i] == '>')
				{
					if ((i + 1 < in.size()) && (in[i + 1] == '<'))
					{
						out += '>';
						++i;
					}
					else if (start != std::string::npos)
					{
						processParam(std::string_view(in.data() + start, i - start), out);
						start = std::string::npos;
					}
					else
						out += '>';
				}
				else
				{
					if (start == std::string::npos)
						out += in[i];
				}
			}
		}

		std::string format(const std::string_view in)
		{
			std::string temp;
			format(in, temp);
			return temp;
		}
	};

	StringFormatter StringFormatter::standard;*/
}