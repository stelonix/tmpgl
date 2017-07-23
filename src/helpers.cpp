#include "string"
#include <sstream>
#include <vector>
#include <dirent.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include "debug.h"
#include "helpers.h"


std::vector<string> list_files(std::string dir)
{
	auto dirp = opendir(dir.c_str());
	auto ret_val = std::vector<string>();
	dirent* dp;
	while ((dp = readdir(dirp)) != NULL)
	{
		auto name = string(dp->d_name);
		if (name != "." && name != "..")
			ret_val.push_back(dp->d_name);
	}
	closedir(dirp);
	return ret_val;
}

string operator "" _s (const char* p, size_t)
{
	return string(p);
}

size_t trim_out(char *out, size_t len, const char *str)
{
	if(len == 0)
	return 0;

	const char *end;
	size_t out_size;

	// Trim leading space
	while(isspace(*str)) str++;

	if(*str == 0)  // All spaces?
	{
		*out = 0;
		return 1;
	}

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;
	end++;

	// Set output size to minimum of trimmed string length and buffer size minus 1
	out_size = (end - str) < len-1 ? (end - str) : len-1;

	// Copy trimmed string and add null terminator
	memcpy(out, str, out_size);
	out[out_size] = 0;

	return out_size;
}

std::vector<string> split(const string &s, char delim) {
	std::stringstream ss(s);
	string item;
	std::vector<string> retval;
	while (std::getline(ss, item, delim)) {
		retval.push_back(item);
	}
	return retval;
}

void init_crt() {
	signal(SIGSEGV, handler);
	setvbuf(stdout, NULL, _IONBF, 0);
}


string trim(string const& str, string chars) {
	if(str.empty())
		return str;

	std::size_t firstScan = str.find_first_not_of(chars);
	std::size_t first     = firstScan == string::npos ? str.length() : firstScan;
	std::size_t last      = str.find_last_not_of(chars);
	return str.substr(first, last-first+1);
}


namespace helpers
{
	unsigned long next_p2(unsigned long v)
	{
		if (is_p2(v)) return v;
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		#if UINTPTR_MAX == 0xffffffffffffffff
		v |= v >> 32;
		#endif
		v++;
		return v;
	}

	bool is_p2(unsigned long v)
	{
		return (v > 0 && ((v & (v - 1)) == 0));
	}

}