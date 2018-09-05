#include <CLI/CLI.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <textencode/common.hpp>
#include <textencode/fd.hpp>
#include <unistd.h>
#include <unordered_map>

using textencode::EncodingType;

const std::unordered_map<std::string, EncodingType> type_map = {
	{"bin", EncodingType::Binary},
	{"binary", EncodingType::Binary},
	{"base16", EncodingType::Base16},
	{"hex", EncodingType::Base16},
	{"base32", EncodingType::Base32},
	{"nix32", EncodingType::Nix32},
	{"base64", EncodingType::Base64},
};

std::string validateEncoding(const std::string &opt)
{
	if (type_map.find(opt) != type_map.end())
		return "";
	return opt + " is not a valid encoding type";
}

int main(int argc, char *argv[])
{
	CLI::App app{"Text Encoding Converter"};
	std::string to_str, from_str;
	app.add_option("-t,--to", to_str, "The type to convert to")
		->required()
		->check(validateEncoding);
	app.add_option("-f,--from", from_str, "The type to convert from")
		->required()
		->check(validateEncoding);
	CLI11_PARSE(app, argc, argv);

	try {
		textencode::transcode(STDIN_FILENO, type_map.at(from_str),
				STDOUT_FILENO, type_map.at(to_str));
		return 0;
	} catch(const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 1;
}
