
#include <windows.h>

class DownloadFiles {
public:
	static int DownloadFile(int cmd, char* cmdfn, LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam, char* DownUploadBuf, int bufsize);
};
