#include <map>
#include <vector>
#include <fstream>
#include <string_view>
#include <cctype>
#include <iostream>

enum Token
{
	tok_eof,

	tok_def,
	tok_extern,

	tok_identifier,
	Number,
};

std::string identifierStr;
double numVal;






int main(int argc, char* argv[])
{
	std::cout << "Evergreen v1\n";



	return EXIT_SUCCESS;
}