
#include <windows.h>

class UploadFiles {
public:
	static int UploadFile(int cmd, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam, char* lpUploadBuf, int bufsize);
};
