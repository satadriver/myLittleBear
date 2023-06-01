

int SetBootAutoRunInRegistryRun(HKEY hMainKey,int iCpuBits,char * lpexefilepath);
int CreateScheduleInCmd(int iInterval,char * szTaskName,char * szPeFileName,char * strUserName,char * strComputerName,int iSystemVersion,char * lpworkpath);