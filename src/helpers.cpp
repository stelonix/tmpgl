#include "string"
#include "helpers.h"
#include <dirent.h>

std::vector<string> list_files(std::string dir) {
	auto dirp = opendir(dir.c_str());
	auto ret_val = std::vector<string>();
	dirent* dp;
	while ((dp = readdir(dirp)) != NULL) {
		auto name = string(dp->d_name);
		if (name != "." && name != "..")
			ret_val.push_back(dp->d_name);
	}
	closedir(dirp);
	return ret_val;
}

std::string read_file(string filename) {
	//std::cout << filename << std::endl;
	FILE* f = fopen(filename.c_str(), "r");
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);

	char* contents = new char[size+1];

	rewind(f);
	fread(contents, sizeof(char), size, f);
	contents[size] = '\0';
	auto retval = string(contents);
	delete[] contents;
	return retval;
}

string operator "" s (const char* p, size_t) {
	return string(p);
}