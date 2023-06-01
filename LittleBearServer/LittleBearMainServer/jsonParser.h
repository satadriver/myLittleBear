#pragma once

#include <iostream>
#include <vector>

using namespace std;

class JsonParser {
public:
	static string getStringValue(string data, string key);

	static string JsonParser::getBaseValue(string data, string key);

	static vector<string> getJsonFromArray(string data);

	static vector<string> JsonParser::getArrayFromEnter(string substr);
	static string JsonParser::getOtherValue(string data, string key);
};