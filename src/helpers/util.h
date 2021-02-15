#ifndef UTIL_H
#define UTIL_H

#include "helpers/string_helper"


namespace util {
	string dirname_s(const char *path);
	string dirname_s(string path);
	string basename_s(const char *path);

	string basename_s(string path);
	bool endswith(string const & value, string const & ending);
	string ReplaceString(string subject, const string& search,
	                          const string& replace);
}

#endif
