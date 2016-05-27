#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <vector>

std::vector<std::string> list_files(std::string dir);

std::string read_file(std::string filename);

template <typename T>
T read_file(std::string filename, size_t* read_bytes = NULL) {
	//std::cout << filename << std::endl;
	FILE* f = fopen(filename.c_str(), "r");
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);

	char* contents = new char[size+1];

	rewind(f);
	auto rb = fread(contents, sizeof(char), size, f);
	if (read_bytes != NULL) *read_bytes = rb;
	contents[size] = '\0';
	auto retval = T(contents);
	delete[] contents;
	return retval;
}
template <typename T> class ArrayDeleter {
public:
    void operator () (T* d) const
    { delete [] d; }
};

size_t trim_out(char *out, size_t len, const char *str);

#endif