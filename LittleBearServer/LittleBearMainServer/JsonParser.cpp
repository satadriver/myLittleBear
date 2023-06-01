#include "jsonParser.h"
#include <string>

using namespace std;

string JsonParser::getStringValue(string data,string key) {
	string findkey = string("\"") + key + "\"";
	int pos = data.find(findkey);
	if (pos >= 0)
	{
		int nowpos = pos + findkey.length();
		int next = data.find(":",nowpos);
		if (next >= 0)
		{
			next++;
			int nnext = data.find("\"", next);
			if (nnext >= 0)
			{
				nnext++;
				int nnnext = data.find("\"", nnext);
				if (nnnext >= 0)
				{
					return data.substr(nnext, nnnext - nnext);
				}
			}
		}
	}

	return "";
}


string JsonParser::getBaseValue(string data, string key) {
	string findkey = string("\"") + key + "\"";
	int pos = data.find(findkey);
	if (pos >= 0)
	{
		int next = data.find(":", pos + findkey.length());
		if (next >= 0)
		{
			next++;
			int nnnext = data.find(",", next);
			if (nnnext >= 0)
			{
				return data.substr(next, nnnext - next);
			}

		}
	}

	return "";
}


string JsonParser::getOtherValue(string data, string key) {

	int pos = data.find(key);
	if (pos >= 0)
	{
		int nowpos = pos + key.length();
		int next = data.find(":", nowpos);
		if (next >= 0)
		{
			next++;
			int nnext = data.find("\"", next);
			if (nnext >= 0)
			{
				nnext++;
				int nnnext = data.find("\"", nnext);
				if (nnnext >= 0)
				{
					return data.substr(nnext, nnnext - nnext);
				}
			}
		}
	}

	return "";
}


vector<string> JsonParser::getJsonFromArray(string data) {
	string substr = data;
	vector <string> ret;
	ret.clear();
	while (1)
	{
		int pos = substr.find("{");
		if (pos >= 0)
		{
			//pos++;
			int nextpos = substr.find("}", pos);
			if (nextpos >= 0)
			{
				string value = substr.substr(pos, nextpos - pos + 1);
				ret.push_back(value);
				substr = substr.substr(nextpos + 1);
			}
			else {
				break;
			}
		}
		else {
			break;
		}
	}

	return ret;
}


vector<string> JsonParser::getArrayFromEnter(string substr) {

	vector <string> ret;
	ret.clear();

	while (1)
	{
		int pos = substr.find("\r\n\r\n");
		if (pos >= 0)
		{
			string value = substr.substr(0,pos);
			ret.push_back(value);
			substr = substr.substr(pos + 4);
		}
		else {
			break;
		}
	}

	return ret;
}