#include <map>
#include <vector>
#include <fstream>
#include <string_view>
#include <cctype>
#include <iostream>

class CodeFile
{
	std::vector<std::string> lines; // Semicolon delimited lines

	CodeFile(const std::string& path)
	{
		std::ofstream stream(path, std::ios::binary | std::ios::in);
	}
};

bool isInteger(const std::string_view input)
{
	if (input.size() == 0)
		return false;
	
	if (!((input[0] == '-') || std::isdigit(input[0])))
		return false;

	for (size_t i = 1; i < input.size(); i++)
	{
		if (!std::isdigit(input[i]))
			return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	std::cout << "Evergreen v1\n";

	//CodeFile file("C:\\Users\\nickk\\source\\repos\\Evergreen\\tests\\hello.evg");

	bool t = isInteger("53");

	return EXIT_SUCCESS;
}