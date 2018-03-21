#include "helpers/string"
#include <string.h>
#include <libgen.h>

namespace util {
	string dirname_s(const char *path)
	{
		auto ptr = strdup(path);
			auto retval = string(dirname(ptr));
		free(ptr);
		return retval;
	}

	string dirname_s(string path)
	{
		return dirname_s(path.c_str());
	}

	string basename_s(const char *path)
	{
		auto ptr = strdup(path);
			auto retval = string(basename(ptr));
		free(ptr);

		return retval;
	}

	string basename_s(string path)
	{
		return basename_s(path.c_str());
	}

	bool endswith(string const & value, string const & ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	string ReplaceString(string subject, const string& search,
	                          const string& replace) {
	    size_t pos = 0;
	    while ((pos = subject.find(search, pos)) != string::npos) {
	         subject.replace(pos, search.length(), replace);
	         break;
	    }
	    return subject;
	}
}
