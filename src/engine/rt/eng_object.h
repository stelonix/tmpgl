#ifndef ENG_OBJECT_H
#define ENG_OBJECT_H
#include <functional>
#include <string>
using namespace std;

struct eng_object {
	int x, y, z;
	int w, h;
	string name;
	std::function<void(eng_object*, int, int)> click_function;
	eng_object(string name);
	eng_object();
	void click_event(int x, int y);
};
#endif
