
#include "main.h"
#include "Public.h"
#include <windows.h>
#include <iostream>

using namespace std;


class Commander {
public:
	static int SendCmdPacket(SOCKET s, int cmd);
	static int RecvCmdPacket(SOCKET s);
	static int SendCmdPacket(LPUNIQUECLIENTSYMBOL lpUnique, SOCKET s, char* lpdata, int datasize, int cmd);
	static int CommandWithoutParam(int cmd, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam);
	static int CommandWithString(int cmd, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam);
	static int CommandWith2String(int cmd, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam);
	static string makeCmdFileName(char* szCurrentDir, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique);
};
