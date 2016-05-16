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

string operator "" _s (const char* p, size_t) {
	return string(p);
}