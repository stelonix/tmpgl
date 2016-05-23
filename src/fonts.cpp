#include "include/json.hpp"
#include "debug.h"
#include "fonts.h"
#include "generators.h"

string font::to_json()
{/*
	json j;
	for (	auto kvp = info.ch.begin();
			kvp != info.ch.end();
			kvp++)
	{
		json ch;
		ch[kvp.first] = kvp.second;
	}*/
		return "";
}

int next_p2(int v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}


