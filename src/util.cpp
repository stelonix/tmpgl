#include "string"
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
}