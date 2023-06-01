#pragma once

#include <string>

using namespace std;

int mainProc();
int BindSo(string path, string fn, string apkfn, string cfgfn);

string getPathFromFullPath(string fullname);