#include "public.h"
#include <windows.h>
#include "api.h"
#include "initHelper.h"
#include "StrHelper.h"

HMODULE					lpThisDll = 0;
HMODULE					lpDllShell32 = 0;
HMODULE					lpDllNetApi32 = 0;
HMODULE					lpDllAvifil32 = 0;
HMODULE					lpDllAvicap32 = 0;
HINSTANCE				hInstMain = 0;
HMODULE					lpDllKernel32 = 0;
HMODULE					lpDllUser32 = 0;
HMODULE					lpDllGdi32 = 0;
HMODULE					lpDllWs2_32 = 0;
HMODULE					lpDlladvapi32 = 0;
HMODULE					lpDlliphlpapi = 0;
HMODULE					lpDllntdll = 0;
HMODULE					lpDllGdiPlus = 0;
HMODULE					lpDllOle32 = 0;
HMODULE					lpDllSetupApi = 0;
HMODULE					lpDllWinmm = 0;
HMODULE					lpDllPsapi = 0;
HMODULE					lpDllMsvfw32 = 0;
HMODULE					lpDllShlwapi = 0;

ptrSetCursorPos			lpSetCursorPos = 0;
ptrFindResourceA		lpFindResourceA = 0;
ptrSizeofResource		lpSizeofResource = 0;

ptrLoadResource			lpLoadResource = 0;
ptrLockResource			lpLockResource = 0;
PFN_Wow64DisableWow64FsRedirection	pfnWow64DisableWow64FsRedirection = 0;
PFN_Wow64RevertWow64FsRedirection	pfnWow64RevertWow64FsRedirection = 0;
ptrCoCreateInstance				lpCoCreateInstance = 0;
ptrCoUninitialize				lpCoUninitialize = 0;
ptrCoInitialize					lpCoInitialize = 0;
ptrSHChangeNotify				lpSHChangeNotify = 0;
ptrShellExecuteA				lpShellExecuteA = 0;
ptrPathStripPathA				pPathStripPathA = 0;
ptrGetCurrentDirectoryA			lpGetCurrentDirectoryA = 0;
// ptrIcmpCreateFile		lpIcmpCreateFile		= 0;
// ptrIcmpSendEcho2		lpIcmpSendEcho2			= 0;
// ptrIcmpSendEcho2Ex		lpIcmpSendEcho2Ex		= 0;
// ptrIcmpCloseHandle		lpIcmpCloseHandle		= 0;
// ptrIcmpParseReplies		lpIcmpParseReplies		= 0;
ptrAllocateAndInitializeSid	lpAllocateAndInitializeSid;
ptrEqualSid					lpEqualSid;
ptrFreeSid					lpFreeSid;
ptrGetTokenInformation	lpGetTokenInformation = 0;

ptrGetExitCodeProcess	lpGetExitCodeProcess = 0;
ptrCreatePipe			lpCreatePipe = 0;
ptrRemoveDirectoryA		lpRemoveDirectoryA = 0;
ptrGetDiskFreeSpaceExA	lpGetDiskFreeSpaceExA = 0;
ptrSetCurrentDirectoryA	lpSetCurrentDirectoryA = 0;
ptrGetUserNameA			lpGetUserNameA = 0;
ptrGetComputerNameA		lpGetComputerNameA = 0;
ptrGetProcAddress		lpGetProcAddress = 0;
ptrLoadLibraryA			lpLoadLibraryA = 0;
ptrFreeLibrary			lpFreeLibrary = 0;
ptrCreateFileA			lpCreateFileA = 0;
ptrCreateFileW			lpCreateFileW = 0;
ptrWriteFile			lpWriteFile = 0;
ptrCloaseHandle			lpCloseHandle = 0;
ptrVirtualAlloc			lpVirtualAlloc = 0;
ptrVirtualFree			lpVirtualFree = 0;
ptrGetModuleFileNameA	lpGetModuleFileNameA = 0;
ptrGetStartupInfoA		lpGetStartupInfoA = 0;
ptrCreateProcessA		lpCreateProcessA = 0;
ptrWinExec				lpWinExec = 0;
ptrGetSystemDirectoryA	lpGetSystemDirectoryA = 0;
ptrExitProcess			lpExitProcess = 0;
ptrGetModuleHandleA		lpGetModuleHandleA = 0;
ptrGetFileSize			lpGetFileSize = 0;
ptrSetFilePointer		lpSetFilePointer = 0;
ptrReadFile				lpReadFile = 0;
ptrCreateDirectoryA		lpCreateDirectoryA = 0;
ptrlstrlenA				lplstrlenA = 0;
ptrlstrlenW				lplstrlenW = 0;
ptrGlobalAlloc			lpGlobalAlloc = 0;
ptrGlobalFree			lpGlobalFree = 0;
ptrGetTickCount			lpGetTickCount = 0;
ptrGetLocalTime			lpGetLocalTime = 0;
ptrSleep				lpSleep = 0;
ptrGetVolumeInformationA lpGetVolumeInformationA = 0;
ptrGetCommandLineA		lpGetCommandLineA = 0;
ptrDeleteFileA			lpDeleteFileA = 0;
ptrGetLogicalDriveStringsA	lpGetLogicalDriveStringsA = 0;
ptrFindFirstFileA		lpFindFirstFileA = 0;
ptrFindNextFileA		lpFindNextFileA = 0;
ptrFindClose			lpFindClose = 0;
ptrGetCurrentProcess	lpGetCurrentProcess = 0;

ptrCreateToolhelp32Snapshot	lpCreateToolhelp32Snapshot = 0;
ptrProcess32First			lpProcess32First = 0;
ptrProcess32Next			lpProcess32Next = 0;

ptrGetDriveTypeA		lpGetDriveTypeA = 0;

ptrGetVersionExA		lpGetVersionExA = 0;
ptrGetSystemInfo		lpGetSystemInfo = 0;
ptrMoveFileExA			lpMoveFileExA = 0;

ptrRtlZeroMomery		lpRtlZeroMemory = 0;
ptrRtlMoveMomery		lpRtlMoveMemory = 0;
ptrRtlGetLastWin32Error	lpRtlGetLastWin32Error = 0;
ptrRtlCompareMomery		lpRtlCompareMemory = 0;

ptrWSAStartup			lpWSAStartup = 0;
ptrsend					lpsend = 0;
ptrrecv					lprecv = 0;
ptrclosesocket			lpclosesocket = 0;
ptrconnect				lpconnect = 0;
ptrinet_addr			lpinet_addr = 0;
ptrntohs				lpntohs = 0;
ptrgethostname			lpgethostname = 0;
ptrsocket				lpsocket = 0;
ptrgethostbyname		lpgethostbyname = 0;
ptrsetsockopt			lpsetsockopt = 0;

ptrRegCreateKeyA		lpRegCreateKeyA = 0;
ptrRegCloseKey			lpRegCloseKey = 0;
ptrRegQueryValueExA		lpRegQueryValueExA = 0;
ptrRegSetValueExA		lpRegSetValueExA = 0;
ptrRegOpenKeyExA		lpRegOpenKeyExA = 0;
ptrRegEnumKeyExA		lpRegEnumKeyExA = 0;
ptrRegDeleteKeyA		lpRegDeleteKeyA = 0;

ptrGetDC				lpGetDC = 0;
ptrReleaseDC			lpReleaseDC = 0;
ptrGetAsyncKeyState		lpGetAsyncKeyState = 0;
ptrGetKeyState			lpGetKeyState = 0;
ptrwsprintfA			lpwsprintfA = 0;
ptrwsprintfW			lpwsprintfW = 0;
ptrGetClassNameA		lpGetClassNameA = 0;
ptrFindWindowA			lpFindWindowA = 0;
ptrGetWindowRect		lpGetWindowRect = 0;
ptrGetDesktopWindow		lpGetDesktopWindow = 0;
ptrUpdateWindow			lpUpdateWindow = 0;
ptrFindWindowExA		lpFindWindowExA = 0;
ptrShellExecuteExW		lpShellExecuteExW = 0;
ptrStrCmpIW				lpStrCmpIW = 0;
ptrGetKeyboardState		lpGetKeyboardState = 0;
ptrGetCursorPos			lpGetCursorPos = 0;
ptrCreateDCA			lpCreateDCA = 0;
ptrCreateCompatibleBitmap lpCreateCompatibleBitmap = 0;
ptrCreateCompatibleDC	lpCreateCompatibleDC = 0;
ptrCreateDIBSection		lpCreateDIBSection = 0;
ptrSelectObject			lpSelectObject = 0;
ptrBitBlt				lpBitBlt = 0;
ptrDeleteDC				lpDeleteDC = 0;
ptrGetDeviceCaps		lpGetDeviceCaps = 0;
ptrSelectPalette		lpSelectPalette = 0;
ptrRealizePalette		lpRealizePalette = 0;
ptrGetStockObject		lpGetStockObject = 0;
ptrGetDIBits			lpGetDIBits = 0;
ptrDeleteObject			lpDeleteObject = 0;
ptrGetObjectA			lpGetObjectA = 0;

ptrGetAdaptersInfo		lpGetAdaptersInfo = 0;

ptrwcscmp				lpwcscmp = 0;
_ptrstrstr				_lpstrstr = 0;
_ptrHexStrStr			_lpHexStrStr = 0;

ptrGetImageEncodersSize	lpGetImageEncodersSize = 0;
ptrGetImageEncoders		lpGetImageEncoders = 0;
ptrGdiplusStartup		lpGdiplusStartup = 0;
ptrGdiplusShutdown		lpGdiplusShutdown = 0;
ptrGdipSaveImageToStream		lpGdipSaveImageToStream = 0;
ptrEncoderQuality				lpEncoderQuality = 0;
ptrGdipDisposeImage				lpGdipDisposeImage = 0;
ptrGdipCreateBitmapFromStream	lpGdipCreateBitmapFromStream = 0;
ptrGdipAlloc					lpGdipAlloc = 0;
ptrGdipCloneImage				lpGdipCloneImage = 0;
ptrGdipFree						lpGdipFree = 0;

_ptrCreateStreamOnHGlobal		_lpCreateStreamOnHGlobal = 0;

// ptrSetupDiEnumDeviceInfo			lpSetupDiEnumDeviceInfo= 0;	
// ptrSetupDiSetClassInstallParamsA	lpSetupDiSetClassInstallParamsA= 0;
// ptrSetupDiCallClassInstaller		lpSetupDiCallClassInstaller= 0;
// ptrSetupDiGetClassDevsA				lpSetupDiGetClassDevsA= 0;
// ptrSetupDiOpenClassRegKey			lpSetupDiOpenClassRegKey= 0;

ptrSetProcessDPIAware				lpSetProcessDPIAware = 0;
ptrIsProcessDPIAware				lpIsProcessDPIAware = 0;
ptrOpenClipboard					lpOpenClipboard = 0;
ptrCloseClipboard					lpCloseClipboard = 0;
ptrIsClipboardFormatAvailable		lpIsClipboardFormatAvailable = 0;
ptrGetClipboardData					lpGetClipboardData = 0;
ptrGlobalLock						lpGlobalLock = 0;
ptrGlobalUnlock						lpGlobalUnlock = 0;
ptrMultiByteToWideChar				lpMultiByteToWideChar = 0;
ptrWideCharToMultiByte				lpWideCharToMultiByte = 0;
ptrDeviceIoControl					lpDeviceIoControl = 0;
ptrReadDirectoryChangesW			lpReadDirectoryChangesW = 0;
ptrGetMessageA						lpGetMessageA = 0;

ptrCreateThread						lpCreateThread = 0;

ptrmmioAscend						lpmmioAscend = 0;
ptrmmioOpenA						lpmmioOpenA = 0;
ptrmmioClose						lpmmioClose = 0;
ptrmmioCreateChunk					lpmmioCreateChunk = 0;
ptrmmioWrite						lpmmioWrite = 0;
ptrwaveInOpen						lpwaveInOpen = 0;
ptrwaveInClose						lpwaveInClose = 0;
ptrwaveInPrepareHeader				lpwaveInPrepareHeader = 0;
ptrwaveInUnprepareHeader			lpwaveInUnprepareHeader = 0;
ptrwaveInAddBuffer					lpwaveInAddBuffer = 0;
ptrwaveInStart						lpwaveInStart = 0;
ptrwaveInStop						lpwaveInStop = 0;
ptrwaveInReset						lpwaveInReset = 0;
ptrSystemParametersInfoA			lpSystemParametersInfoA = 0;

ptrSendMessageA						lpSendMessageA = 0;
ptrSendMessageW 					lpSendMessageW = 0;
ptrcapCreateCaptureWindowA			lpcapCreateCaptureWindowA = 0;
ptrcapCreateCaptureWindowW			lpcapCreateCaptureWindowW = 0;

ptrCreateMutexA					lpCreateMutexA = 0;
// ptrIsWindowsVersionOrGreater	lpIsWindowsVersionOrGreater = 0;
// ptrIsWindowsXPOrGreater			lprIsWindowsXPOrGreater = 0;
// ptrIsWindowsVistaOrGreater		lpIsWindowsVistaOrGreater = 0;
// ptrIsWindows7OrGreater			lprIsWindows7OrGreater = 0;
// ptrIsWindows8OrGreater			lpIsWindows8OrGreater = 0;
// ptrIsWindowsServer				lpIsWindowsServer = 0;

ptrGlobalMemoryStatusEx			lpGlobalMemoryStatusEx = 0;
ptrVerifyVersionInfoW			lpVerifyVersionInfoW = 0;
ptrVerSetConditionMask			lpVerSetConditionMask = 0;

ptrRegCreateKeyExA				lpRegCreateKeyExA = 0;
ptrCopyFileA					lpCopyFileA = 0;

ptrEnumProcesses				lpEnumProcesses = 0;
ptrOpenProcess					lpOpenProcess = 0;
ptrEnumProcessModules			lpEnumProcessModules = 0;
ptrGetModuleBaseNameA			lpGetModuleBaseNameA = 0;
ptrCreateRemoteThread			lpCreateRemoteThread = 0;
ptrVirtualAllocEx				lpVirtualAllocEx = 0;
ptrWriteProcessMemory			lpWriteProcessMemory = 0;
ptrVirtualFreeEx				lpVirtualFreeEx = 0;
ptrWaitForSingleObject			lpWaitForSingleObject = 0;
ptrGetExitCodeThread			lpGetExitCodeThread = 0;
ptrWaitForMultipleObjects		lpWaitForMultipleObjects = 0;

ptrExitThread							lpExitThread = 0;
ptrGetForegroundWindow					lpGetForegroundWindow = 0;
ptrGetWindowTextA						lpGetWindowTextA = 0;
ptrCreateEventA							lpCreateEventA = 0;
ptrSetEvent								lpSetEvent = 0;

ptrICOpen								lpICOpen = 0;
ptrICClose								lpICClose = 0;
ptrICSendMessage						lpICSendMessage = 0;

ptrAVIMakeCompressedStream				lpAVIMakeCompressedStream = 0;
ptrAVIFileRelease						lpAVIFileRelease;
ptrAVIFileInit							lpAVIFileInit = 0;
ptrAVIStreamRelease						lpAVIStreamRelease = 0;
ptrAVIFileExit							lpAVIFileExit = 0;
ptrAVIFileOpenA							lpAVIFileOpenA = 0;
ptrSetRect								lpSetRect = 0;
ptrAVIFileCreateStreamA					lpAVIFileCreateStreamA = 0;
ptrAVIStreamSetFormat					lpAVIStreamSetFormat = 0;
ptrAVIStreamWrite						lpAVIStreamWrite = 0;

ptrNetScheduleJobAddW					lpNetScheduleJobAddW = 0;
ptrNetWkstaGetInfo						lpNetWkstaGetInfo = 0;
ptrNetApiBufferFree						lpNetApiBufferFree = 0;
ptrOpenProcessToken					lpOpenProcessToken = 0;
ptrLookupPrivilegeValueW				lpLookupPrivilegeValueW = 0;
ptrAdjustTokenPrivileges				lpAdjustTokenPrivileges = 0;
ptrShellExecuteExA						lpShellExecuteExA = 0;

ptrOpenServiceA			lpOpenServiceA = 0;
ptrCloseServiceHandle	lpCloseServiceHandle = 0;
ptrQueryServiceStatus	lpQueryServiceStatus = 0;
ptrControlService		lpControlService = 0;
ptrCreateServiceA		lpCreateServiceA = 0;
ptrOpenSCManagerA		lpOpenSCManagerA = 0;
ptrDeleteService		lpDeleteService = 0;
ptrStartServiceA		lpStartServiceA = 0;

ptrTerminateProcess				lpTerminateProcess = 0;
ptrGetModuleFileNameExA			lpGetModuleFileNameExA = 0;
ptrMoveFileA					lpMoveFileA = 0;
ptrGetCurrentProcessId			lpGetCurrentProcessId = 0;
ptrSetFileAttributesA			lpSetFileAttributesA = 0;

ptrlstrcmpiA							lplstrcmpiA = 0;
ptrlstrcpyA								lplstrcpyA = 0;
ptrlstrcatA								lplstrcatA = 0;
ptrlstrcmpA								lplstrcmpA = 0;
ptrDeleteFileW							lpDeleteFileW = 0;
ptrGetCommandLineW						lpGetCommandLineW = 0;
ptrCommandLineToArgvW					lpCommandLineToArgvW = 0;
ptrReleaseMutex							lpReleaseMutex = 0;

ptrRegisterClassExA						lpRegisterClassExA = 0;
ptrTranslateMessage						lpTranslateMessage = 0;
ptrDispatchMessageA						lpDispatchMessageA = 0;
ptrCreateWindowExA						lpCreateWindowExA = 0;
ptrDefWindowProcA						lpDefWindowProcA = 0;




int furtherApi()
{
	char szAVIFileRelease[] = { 'A','V','I','F','i','l','e','R','e','l','e','a','s','e',0 };
	char szAVIMakeCompressedStream[] = { 'A','V','I','M','a','k','e','C','o','m','p','r','e','s','s','e','d','S','t','r','e','a','m',0 };
	char szICOpen[] = { 'I','C','O','p','e','n',0 };
	char szICClose[] = { 'I','C','C','l','o','s','e',0 };
	char szICSendMessage[] = { 'I','C','S','e','n','d','M','e','s','s','a','g','e',0 };
	char szDllAvifil32[] = { 'A','v','i','f','i','l','3','2','.','d','l','l',0 };
	char szAVIFileInit[] = { 'A','V','I','F','i','l','e','I','n','i','t',0 };
	char szAVIStreamRelease[] = { 'A','V','I','S','t','r','e','a','m','R','e','l','e','a','s','e',0 };
	char szAVIFileExit[] = { 'A','V','I','F','i','l','e','E','x','i','t',0 };
	char szAVIFileOpenA[] = { 'A','V','I','F','i','l','e','O','p','e','n','A',0 };

	char szAVIFileCreateStreamA[] = { 'A','V','I','F','i','l','e','C','r','e','a','t','e','S','t','r','e','a','m','A',0 };
	char szAVIStreamSetFormat[] = { 'A','V','I','S','t','r','e','a','m','S','e','t','F','o','r','m','a','t',0 };
	char szAVIStreamWrite[] = { 'A','V','I','S','t','r','e','a','m','W','r','i','t','e',0 };

	char szmmioAscend[] = { 'm','m','i','o','A','s','c','e','n','d',0 };
	char szmmioOpenA[] = { 'm','m','i','o','O','p','e','n','A',0 };
	char szmmioClose[] = { 'm','m','i','o','C','l','o','s','e',0 };
	char szmmioCreateChunk[] = { 'm','m','i','o','C','r','e','a','t','e','C','h','u','n','k',0 };
	char szmmioWrite[] = { 'm','m','i','o','W','r','i','t','e',0 };
	char szwaveInOpen[] = { 'w','a','v','e','I','n','O','p','e','n',0 };
	char szwaveInClose[] = { 'w','a','v','e','I','n','C','l','o','s','e',0 };
	char szwaveInPrepareHeader[] = { 'w','a','v','e','I','n','P','r','e','p','a','r','e','H','e','a','d','e','r',0 };
	char szwaveInUnprepareHeader[] = { 'w','a','v','e','I','n','U','n','p','r','e','p','a','r','e','H','e','a','d','e','r',0 };
	char szwaveInAddBuffer[] = { 'w','a','v','e','I','n','A','d','d','B','u','f','f','e','r',0 };
	char szwaveInStart[] = { 'w','a','v','e','I','n','S','t','a','r','t',0 };
	char szwaveInStop[] = { 'w','a','v','e','I','n','S','t','o','p',0 };
	char szwaveInReset[] = { 'w','a','v','e','I','n','R','e','s','e','t',0 };

	char szcapCreateCaptureWindowA[] = { 'c','a','p','C','r','e','a','t','e','C','a','p','t','u','r','e','W','i','n','d','o','w','A',0 };
	char szcapCreateCaptureWindowW[] = { 'c','a','p','C','r','e','a','t','e','C','a','p','t','u','r','e','W','i','n','d','o','w','W',0 };

	char szGetImageEncodersSize[] = { 'G','d','i','p','G','e','t','I','m','a','g','e','E','n','c','o','d','e','r','s','S','i','z','e',0 };
	char szGetImageEncoders[] = { 'G','d','i','p','G','e','t','I','m','a','g','e','E','n','c','o','d','e','r','s',0 };
	char szGdiplusStartup[] = { 'G','d','i','p','l','u','s','S','t','a','r','t','u','p',0 };
	char szGdiplusShutdown[] = { 'G','d','i','p','l','u','s','S','h','u','t','d','o','w','n',0 };

	char szCreateCompatibleBitmap[] = { 'C','r','e','a','t','e','C','o','m','p','a','t','i','b','l','e','B','i','t','m','a','p',0 };
	char szCreateCompatibleDC[] = { 'C','r','e','a','t','e','C','o','m','p','a','t','i','b','l','e','D','C',0 };
	char szCreateDIBSection[] = { 'C','r','e','a','t','e','D','I','B','S','e','c','t','i','o','n',0 };
	char szSelectObject[] = { 'S','e','l','e','c','t','O','b','j','e','c','t',0 };
	char szGetObjectA[] = { 'G','e','t','O','b','j','e','c','t','A',0 };
	char szBitBlt[] = { 'B','i','t','B','l','t',0 };
	char szDeleteDC[] = { 'D','e','l','e','t','e','D','C',0 };
	char szCreateDCA[] = { 'C','r','e','a','t','e','D','C','A',0 };
	char szGetDeviceCaps[] = { 'G','e','t','D','e','v','i','c','e','C','a','p','s',0 };
	char szSelectPalette[] = { 'S','e','l','e','c','t','P','a','l','e','t','t','e',0 };
	char szRealizePalette[] = { 'R','e','a','l','i','z','e','P','a','l','e','t','t','e',0 };
	char szGetStockObject[] = { 'G','e','t','S','t','o','c','k','O','b','j','e','c','t',0 };
	char szGetDIBits[] = { 'G','e','t','D','I','B','i','t','s',0 };
	char szDeleteObject[] = { 'D','e','l','e','t','e','O','b','j','e','c','t',0 };

	char szDllWinmm[] = { 'W','i','n','m','m','.','d','l','l',0 };
	char szDllAvicap[] = { 'A','v','i','c','a','p','3','2','.','d','l','l',0 };
	char szDllGdiplus[] = { 'g','d','i','p','l','u','s','.','d','l','l',0 };
	char szDllMsVfw32[] = { 'm','s','v','f','w','3','2','.','d','l','l',0 };
	char szDllGdi32[] = { 'g','d','i','3','2','.','d','l','l',0 };

	lpDllWinmm = (HMODULE)lpLoadLibraryA(szDllWinmm);
	lpmmioAscend = (ptrmmioAscend)lpGetProcAddress(lpDllWinmm, szmmioAscend);
	lpmmioOpenA = (ptrmmioOpenA)lpGetProcAddress(lpDllWinmm, szmmioOpenA);
	lpmmioClose = (ptrmmioClose)lpGetProcAddress(lpDllWinmm, szmmioClose);
	lpmmioCreateChunk = (ptrmmioCreateChunk)lpGetProcAddress(lpDllWinmm, szmmioCreateChunk);
	lpmmioWrite = (ptrmmioWrite)lpGetProcAddress(lpDllWinmm, szmmioWrite);
	lpwaveInOpen = (ptrwaveInOpen)lpGetProcAddress(lpDllWinmm, szwaveInOpen);
	lpwaveInClose = (ptrwaveInClose)lpGetProcAddress(lpDllWinmm, szwaveInClose);
	lpwaveInPrepareHeader = (ptrwaveInPrepareHeader)lpGetProcAddress(lpDllWinmm, szwaveInPrepareHeader);
	lpwaveInUnprepareHeader = (ptrwaveInUnprepareHeader)lpGetProcAddress(lpDllWinmm, szwaveInUnprepareHeader);
	lpwaveInAddBuffer = (ptrwaveInAddBuffer)lpGetProcAddress(lpDllWinmm, szwaveInAddBuffer);
	lpwaveInStart = (ptrwaveInStart)lpGetProcAddress(lpDllWinmm, szwaveInStart);
	lpwaveInStop = (ptrwaveInStop)lpGetProcAddress(lpDllWinmm, szwaveInStop);
	lpwaveInReset = (ptrwaveInReset)lpGetProcAddress(lpDllWinmm, szwaveInReset);

	lpDllAvicap32 = (HMODULE)lpLoadLibraryA(szDllAvicap);
	lpcapCreateCaptureWindowA = (ptrcapCreateCaptureWindowA)lpGetProcAddress(lpDllAvicap32, szcapCreateCaptureWindowA);
	lpcapCreateCaptureWindowW = (ptrcapCreateCaptureWindowW)lpGetProcAddress(lpDllAvicap32, szcapCreateCaptureWindowW);

	lpDllAvifil32 = lpLoadLibraryA(szDllAvifil32);
	lpAVIFileInit = (ptrAVIFileInit)lpGetProcAddress(lpDllAvifil32, szAVIFileInit);
	lpAVIStreamRelease = (ptrAVIStreamRelease)lpGetProcAddress(lpDllAvifil32, szAVIStreamRelease);
	lpAVIFileRelease = (ptrAVIFileRelease)lpGetProcAddress(lpDllAvifil32, szAVIFileRelease);
	lpAVIFileExit = (ptrAVIFileExit)lpGetProcAddress(lpDllAvifil32, szAVIFileExit);
	lpAVIFileOpenA = (ptrAVIFileOpenA)lpGetProcAddress(lpDllAvifil32, szAVIFileOpenA);
	lpAVIFileCreateStreamA = (ptrAVIFileCreateStreamA)lpGetProcAddress(lpDllAvifil32, szAVIFileCreateStreamA);
	lpAVIStreamSetFormat = (ptrAVIStreamSetFormat)lpGetProcAddress(lpDllAvifil32, szAVIStreamSetFormat);
	lpAVIStreamWrite = (ptrAVIStreamWrite)lpGetProcAddress(lpDllAvifil32, szAVIStreamWrite);
	lpAVIMakeCompressedStream = (ptrAVIMakeCompressedStream)lpGetProcAddress(lpDllAvifil32, szAVIMakeCompressedStream);
	lpAVIFileRelease = (ptrAVIFileRelease)lpGetProcAddress(lpDllAvifil32, szAVIFileRelease);

	lpDllGdi32 = lpLoadLibraryA(szDllGdi32);
	lpCreateDCA = (ptrCreateDCA)lpGetProcAddress(lpDllGdi32, szCreateDCA);
	lpCreateCompatibleBitmap = (ptrCreateCompatibleBitmap)lpGetProcAddress(lpDllGdi32, szCreateCompatibleBitmap);
	lpCreateCompatibleDC = (ptrCreateCompatibleDC)lpGetProcAddress(lpDllGdi32, szCreateCompatibleDC);
	lpCreateDIBSection = (ptrCreateDIBSection)lpGetProcAddress(lpDllGdi32, szCreateDIBSection);
	lpSelectObject = (ptrSelectObject)lpGetProcAddress(lpDllGdi32, szSelectObject);
	lpBitBlt = (ptrBitBlt)lpGetProcAddress(lpDllGdi32, szBitBlt);
	lpDeleteDC = (ptrDeleteDC)lpGetProcAddress(lpDllGdi32, szDeleteDC);
	lpGetDeviceCaps = (ptrGetDeviceCaps)lpGetProcAddress(lpDllGdi32, szGetDeviceCaps);
	lpSelectPalette = (ptrSelectPalette)lpGetProcAddress(lpDllGdi32, szSelectPalette);
	lpRealizePalette = (ptrRealizePalette)lpGetProcAddress(lpDllGdi32, szRealizePalette);
	lpGetStockObject = (ptrGetStockObject)lpGetProcAddress(lpDllGdi32, szGetStockObject);
	lpGetDIBits = (ptrGetDIBits)lpGetProcAddress(lpDllGdi32, szGetDIBits);
	lpDeleteObject = (ptrDeleteObject)lpGetProcAddress(lpDllGdi32, szDeleteObject);
	lpGetObjectA = (ptrGetObjectA)lpGetProcAddress(lpDllGdi32, szGetObjectA);

	//if not use this ,will save itream error in jpg format photo
	lpDllGdiPlus = lpLoadLibraryA(szDllGdiplus);
	lpGetImageEncoders = (ptrGetImageEncoders)lpGetProcAddress(lpDllGdiPlus, szGetImageEncoders);
	lpGetImageEncodersSize = (ptrGetImageEncodersSize)lpGetProcAddress(lpDllGdiPlus, szGetImageEncodersSize);
	lpGdiplusStartup = (ptrGdiplusStartup)lpGetProcAddress(lpDllGdiPlus, szGdiplusStartup);
	lpGdiplusShutdown = (ptrGdiplusShutdown)lpGetProcAddress(lpDllGdiPlus, szGdiplusShutdown);

	lpDllMsvfw32 = lpLoadLibraryA(szDllMsVfw32);
	lpICOpen = (ptrICOpen)lpGetProcAddress(lpDllMsvfw32, szICOpen);
	lpICClose = (ptrICClose)lpGetProcAddress(lpDllMsvfw32, szICClose);
	lpICSendMessage = (ptrICSendMessage)lpGetProcAddress(lpDllMsvfw32, szICSendMessage);

	char szSendMessageA[] = { 'S','e','n','d','M','e','s','s','a','g','e','A',0 };
	char szSendMessageW[] = { 'S','e','n','d','M','e','s','s','a','g','e','W',0 };
	char szGetMessageA[] = { 'G','e','t','M','e','s','s','a','g','e','A',0 };
	char szSystemParametersInfoA[] = { 'S','y','s','t','e','m','P','a','r','a','m','e','t','e','r','s','I','n','f','o','A',0 };
	char szOpenClipboard[] = { 'O','p','e','n','C','l','i','p','b','o','a','r','d',0 };
	char szCloseClipboard[] = { 'C','l','o','s','e','C','l','i','p','b','o','a','r','d',0 };
	char szIsClipboardFormatAvailable[] = { 'I','s','C','l','i','p','b','o','a','r','d','F','o','r','m','a','t','A','v','a','i','l','a','b','l','e',0 };
	char szGetClipboardData[] = { 'G','e','t','C','l','i','p','b','o','a','r','d','D','a','t','a',0 };
	char szGetCursorPos[] = { 'G','e','t','C','u','r','s','o','r','P','o','s',0 };
	char szGetKeyboardState[] = { 'G','e','t','K','e','y','b','o','a','r','d','S','t','a','t','e',0 };
	char szSetRect[] = { 'S','e','t','R','e','c','t',0 };

	char szUpdateWindow[] = { 'U','p','d','a','t','e','W','i','n','d','o','w',0 };
	char szGetDC[] = { 'G','e','t','D','C',0 };
	char szReleaseDC[] = { 'R','e','l','e','a','s','e','D','C',0 };
	char szGetAsyncKeyState[] = { 'G','e','t','A','s','y','n','c','K','e','y','S','t','a','t','e',0 };
	char szGetKeyState[] = { 'G','e','t','K','e','y','S','t','a','t','e',0 };
	char szFindWindowA[] = { 'F','i','n','d','W','i','n','d','o','w','A',0 };
	char szGetWindowRect[] = { 'G','e','t','W','i','n','d','o','w','R','e','c','t',0 };
	char szGetForegroundWindow[] = { 'G','e','t','F','o','r','e','g','r','o','u','n','d','W','i','n','d','o','w',0 };
	char szGetWindowTextA[] = { 'G','e','t','W','i','n','d','o','w','T','e','x','t','A',0 };
	char szSetCursorPos[] = { 'S','e','t','C','u','r','s','o','r','P','o','s',0 };

	lpSetCursorPos = (ptrSetCursorPos)lpGetProcAddress(lpDllUser32, szSetCursorPos);
	lpUpdateWindow = (ptrUpdateWindow)lpGetProcAddress(lpDllUser32, szUpdateWindow);
	lpGetWindowRect = (ptrGetWindowRect)lpGetProcAddress(lpDllUser32, szGetWindowRect);
	lpGetKeyboardState = (ptrGetKeyboardState)lpGetProcAddress(lpDllUser32, szGetKeyboardState);
	lpGetCursorPos = (ptrGetCursorPos)lpGetProcAddress(lpDllUser32, szGetCursorPos);
	lpSetRect = (ptrSetRect)lpGetProcAddress(lpDllUser32, szSetRect);
	lpGetForegroundWindow = (ptrGetForegroundWindow)lpGetProcAddress(lpDllUser32, szGetForegroundWindow);
	lpGetWindowTextA = (ptrGetWindowTextA)lpGetProcAddress(lpDllUser32, szGetWindowTextA);
	lpSendMessageA = (ptrSendMessageA)lpGetProcAddress(lpDllUser32, szSendMessageA);
	lpSendMessageW = (ptrSendMessageW)lpGetProcAddress(lpDllUser32, szSendMessageW);
	lpSystemParametersInfoA = (ptrSystemParametersInfoA)lpGetProcAddress(lpDllUser32, szSystemParametersInfoA);
	lpOpenClipboard = (ptrOpenClipboard)lpGetProcAddress(lpDllUser32, szOpenClipboard);
	lpCloseClipboard = (ptrCloseClipboard)lpGetProcAddress(lpDllUser32, szCloseClipboard);
	lpIsClipboardFormatAvailable = (ptrIsClipboardFormatAvailable)lpGetProcAddress(lpDllUser32, szIsClipboardFormatAvailable);
	lpGetClipboardData = (ptrGetClipboardData)lpGetProcAddress(lpDllUser32, szGetClipboardData);
	lpGetMessageA = (ptrGetMessageA)lpGetProcAddress(lpDllUser32, szGetMessageA);
	lpGetKeyState = (ptrGetKeyState)lpGetProcAddress(lpDllUser32, szGetKeyState);
	lpGetAsyncKeyState = (ptrGetAsyncKeyState)lpGetProcAddress(lpDllUser32, szGetAsyncKeyState);
	lpGetDC = (ptrGetDC)lpGetProcAddress(lpDllUser32, szGetDC);
	lpReleaseDC = (ptrReleaseDC)lpGetProcAddress(lpDllUser32, szReleaseDC);

	return TRUE;
}





int _GetApi()
{
	char szGetDesktopWindow[] = { 'G','e','t','D','e','s','k','t','o','p','W','i','n','d','o','w',0 };
	char szFindWindowA[] = { 'F','i','n','d','W','i','n','d','o','w','A',0 };
	char szFindWindowExA[] = { 'F','i','n','d','W','i','n','d','o','w','E','x','A',0 };
	char szwsprintfA[] = { 'w','s','p','r','i','n','t','f','A',0 };
	char szwsprintfW[] = { 'w','s','p','r','i','n','t','f','W',0 };
	char szGetCurrentProcess[] = { 'G','e','t','C','u','r','r','e','n','t','P','r','o','c','e','s','s',0 };
	char szRegisterClassExA[] = { 'R','e','g','i','s','t','e','r','C','l','a','s','s','E','x','A',0 };
	char szTranslateMessage[] = { 'T','r','a','n','s','l','a','t','e','M','e','s','s','a','g','e',0 };
	char szDispatchMessageA[] = { 'D','i','s','p','a','t','c','h','M','e','s','s','a','g','e','A',0 };
	char szDefWindowProcA[] = { 'D','e','f','W','i','n','d','o','w','P','r','o','c','A',0 };

	char szCreateWindowExA[] = { 'C','r','e','a','t','e','W','i','n','d','o','w','E','x','A',0 };
	char szDllUser32[] = { 'u','s','e','r','3','2','.','d','l','l',0 };
	char szDllWs2_32[] = { 'w','s','2','_','3','2','.','d','l','l',0 };
	char szDlladvapi32[] = { 'a','d','v','a','p','i','3','2','.','d','l','l',0 };
	char szDlliphlpapi[] = { 'i','p','h','l','p','a','p','i','.','d','l','l',0 };
	char szDllntdll[] = { 'n','t','d','l','l','.','d','l','l',0 };
	char szDllOle32[] = { 'O','l','e','3','2','.','d','l','l',0 };
	//char szDllSetupApi[]			= {'s','e','t','u','p','a','p','i','.','d','l','l',0};
	char szRegCreateKeyA[] = { 'R','e','g','C','r','e','a','t','e','K','e','y','A',0 };
	char szRegCloseKey[] = { 'R','e','g','C','l','o','s','e','K','e','y',0 };
	char szRegQueryValueExA[] = { 'R','e','g','Q','u','e','r','y','V','a','l','u','e','E','x','A',0 };
	char szRegSetValueExA[] = { 'R','e','g','S','e','t','V','a','l','u','e','E','x','A',0 };
	char szRegOpenKeyExA[] = { 'R','e','g','O','p','e','n','K','e','y','E','x','A',0 };
	char szRegEnumKeyExA[] = { 'R','e','g','E','n','u','m','K','e','y','E','x','A',0 };
	char szRegCreateKeyExA[] = { 'R','e','g','C','r','e','a','t','e','K','e','y','E','x','A',0 };
	char szRegDeleteKeyA[] = { 'R','e','g','D','e','l','e','t','e','K','e','y','A',0 };
	char szAllocateAndInitializeSid[] = { 'A','l','l','o','c','a','t','e','A','n','d','I','n','i','t','i','a','l','i','z','e','S','i','d',0 };
	char szEqualSid[] = { 'E','q','u','a','l','S','i','d',0 };
	char szFreeSid[] = { 'F','r','e','e','S','i','d',0 };
	char szconnect[] = { 'c','o','n','n','e','c','t',0 };
	char szWSAStartup[] = { 'W','S','A','S','t','a','r','t','u','p',0 };
	char szrecv[] = { 'r','e','c','v',0 };
	char szsend[] = { 's','e','n','d',0 };
	char szsocket[] = { 's','o','c','k','e','t',0 };
	char szclosesocket[] = { 'c','l','o','s','e','s','o','c','k','e','t',0 };
	char szinet_addr[] = { 'i','n','e','t','_','a','d','d','r',0 };
	char szntohs[] = { 'n','t','o','h','s',0 };
	char szsetsockopt[] = { 's','e','t','s','o','c','k','o','p','t',0 };
	char szgethostname[] = { 'g','e','t','h','o','s','t','n','a','m','e',0 };
	char szgethostbyname[] = { 'g','e','t','h','o','s','t','b','y','n','a','m','e',0 };
	char szGetTokenInformation[] = { 'G','e','t','T','o','k','e','n','I','n','f','o','r','m','a','t','i','o','n',0 };

	char szLoadLibraryA[] = { 'L','o','a','d','L','i','b','r','a','r','y','A',0 };
	char szFreeLibrary[] = { 'F','r','e','e','L','i','b','r','a','r','y',0 };
	char szGetProcAddress[] = { 'G','e','t','P','r','o','c','A','d','d','r','e','s','s',0 };
	char szCreateFileA[] = { 'C','r','e','a','t','e','F','i','l','e','A',0 };
	char szCreateFileW[] = { 'C','r','e','a','t','e','F','i','l','e','W',0 };
	char szWriteFile[] = { 'W','r','i','t','e','F','i','l','e',0 };
	char szCloseHandle[] = { 'C','l','o','s','e','H','a','n','d','l','e',0 };
	char szVirtualAlloc[] = { 'V','i','r','t','u','a','l','A','l','l','o','c',0 };
	char szVirtualFree[] = { 'V','i','r','t','u','a','l','F','r','e','e',0 };
	char szGetStartupInfoA[] = { 'G','e','t','S','t','a','r','t','u','p','I','n','f','o','A',0 };
	char szCreateProcessA[] = { 'C','r','e','a','t','e','P','r','o','c','e','s','s','A',0 };
	char szWinExec[] = { 'W','i','n','E','x','e','c',0 };
	char szGetSystemDirectoryA[] = { 'G','e','t','S','y','s','t','e','m','D','i','r','e','c','t','o','r','y','A',0 };
	char szGetModuleFileNameA[] = { 'G','e','t','M','o','d','u','l','e','F','i','l','e','N','a','m','e','A',0 };
	char szExitProcess[] = { 'E','x','i','t','P','r','o','c','e','s','s',0 };
	char szGetModuleHandleA[] = { 'G','e','t','M','o','d','u','l','e','H','a','n','d','l','e','A',0 };
	char szGetFileSize[] = { 'G','e','t','F','i','l','e','S','i','z','e',0 };
	char szSetFilePointer[] = { 'S','e','t','F','i','l','e','P','o','i','n','t','e','r',0 };
	char szReadFile[] = { 'R','e','a','d','F','i','l','e',0 };
	char szCreateDirectoryA[] = { 'C','r','e','a','t','e','D','i','r','e','c','t','o','r','y','A',0 };
	char szlstrlenA[] = { 'l','s','t','r','l','e','n','A',0 };
	char szlstrlenW[] = { 'l','s','t','r','l','e','n','W',0 };
	char szlstrcmpiA[] = { 'l','s','t','r','c','m','p','i','A',0 };
	char szGlobalAlloc[] = { 'G','l','o','b','a','l','A','l','l','o','c',0 };
	char szGlobalFree[] = { 'G','l','o','b','a','l','F','r','e','e',0 };
	char szGetTickCount[] = { 'G','e','t','T','i','c','k','C','o','u','n','t',0 };
	char szGetLocalTime[] = { 'G','e','t','L','o','c','a','l','T','i','m','e',0 };
	char szSleep[] = { 'S','l','e','e','p',0 };
	char szGetVolumeInformationA[] = { 'G','e','t','V','o','l','u','m','e','I','n','f','o','r','m','a','t','i','o','n','A',0 };
	char szGetCommandLineA[] = { 'G','e','t','C','o','m','m','a','n','d','L','i','n','e','A',0 };
	char szDeleteFileA[] = { 'D','e','l','e','t','e','F','i','l','e','A',0 };
	char szGetLogicalDriveStringsA[] = { 'G','e','t','L','o','g','i','c','a','l','D','r','i','v','e','S','t','r','i','n','g','s','A',0 };
	char szFindFirstFileA[] = { 'F','i','n','d','F','i','r','s','t','F','i','l','e','A',0 };
	char szFindNextFileA[] = { 'F','i','n','d','N','e','x','t','F','i','l','e','A',0 };
	char szFindClose[] = { 'F','i','n','d','C','l','o','s','e',0 };
	char szGetVersionExA[] = { 'G','e','t','V','e','r','s','i','o','n','E','x','A',0 };
	char szGetSystemInfo[] = { 'G','e','t','S','y','s','t','e','m','I','n','f','o',0 };
	char szMoveFileExA[] = { 'M','o','v','e','F','i','l','e','E','x','A',0 };
	char szGetDriveTypeA[] = { 'G','e','t','D','r','i','v','e','T','y','p','e','A',0 };

	char szGetUserNameA[] = { 'G','e','t','U','s','e','r','N','a','m','e','A',0 };
	char szGetComputerNameA[] = { 'G','e','t','C','o','m','p','u','t','e','r','N','a','m','e','A',0 };
	char szCreateToolhelp32Snapshot[] = { 'C','r','e','a','t','e','T','o','o','l','h','e','l','p','3','2','S','n','a','p','s','h','o','t',0 };
	char szProcess32First[] = { 'P','r','o','c','e','s','s','3','2','F','i','r','s','t','W',0 };
	char szProcess32Next[] = { 'P','r','o','c','e','s','s','3','2','N','e','x','t','W',0 };
	char szGetAdaptersInfo[] = { 'G','e','t','A','d','a','p','t','e','r','s','I','n','f','o',0 };
	char szRtlGetLastWin32Error[] = { 'R','t','l','G','e','t','L','a','s','t','W','i','n','3','2','E','r','r','o','r',0 };
	char szRtlZeroMemory[] = { 'R','t','l','Z','e','r','o','M','e','m','o','r','y',0 };
	char szRtlMoveMemory[] = { 'R','t','l','M','o','v','e','M','e','m','o','r','y',0 };
	char szRtlCompareMemory[] = { 'R','t','l','C','o','m','p','a','r','e','M','e','m','o','r','y',0 };

	// 	char szSetupDiEnumDeviceInfo[]			= {'S','e','t','u','p','D','i','E','n','u','m','D','e','v','i','c','e','I','n','f','o',0};
	// 	char szSetupDiSetClassInstallParamsA[]	= {'S','e','t','u','p','D','i','S','e','t','C','l','a','s','s','I','n','s','t','a','l','l','P','a','r','a','m','s','A',0};
	// 	char szSetupDiCallClassInstaller[]		= {'S','e','t','u','p','D','i','C','a','l','l','C','l','a','s','s','I','n','s','t','a','l','l','e','r',0};
	// 	char szSetupDiGetClassDevsA[]			= {'S','e','t','u','p','D','i','G','e','t','C','l','a','s','s','D','e','v','s','A',0};
	// 	char szSetupDiOpenClassRegKey[]			= {'S','e','t','u','p','D','i','O','p','e','n','C','l','a','s','s','R','e','g','K','e','y',0};

	char szGlobalLock[] = { 'G','l','o','b','a','l','L','o','c','k',0 };
	char szGlobalUnlock[] = { 'G','l','o','b','a','l','U','n','l','o','c','k',0 };
	char szMultiByteToWideChar[] = { 'M','u','l','t','i','B','y','t','e','T','o','W','i','d','e','C','h','a','r',0 };
	char szWideCharToMultiByte[] = { 'W','i','d','e','C','h','a','r','T','o','M','u','l','t','i','B','y','t','e',0 };
	char szDeviceIoControl[] = { 'D','e','v','i','c','e','I','o','C','o','n','t','r','o','l',0 };
	char szReadDirectoryChangesW[] = { 'R','e','a','d','D','i','r','e','c','t','o','r','y','C','h','a','n','g','e','s','W',0 };
	char szCreateThread[] = { 'C','r','e','a','t','e','T','h','r','e','a','d',0 };
	char szCreateMutexA[] = { 'C','r','e','a','t','e','M','u','t','e','x','A',0 };

	// 	char szIsWindowsVersionOrGreater[]	= {'I','s','W','i','n','d','o','w','s','V','e','r','s','i','o','n','O','r','G','r','e','a','t','e','r',0};
	// 	char szIsWindowsXPOrGreater[]		= {'I','s','W','i','n','d','o','w','s','X','P','O','r','G','r','e','a','t','e','r',0};
	// 	char szIsWindowsVistaOrGreater[]	= {'I','s','W','i','n','d','o','w','s','V','i','s','t','a','O','r','G','r','e','a','t','e','r',0};
	// 	char szIsWindows7OrGreater[]		= {'I','s','W','i','n','d','o','w','s','7','O','r','G','r','e','a','t','e','r',0};
	// 	char szIsWindows8OrGreater[]		= {'I','s','W','i','n','d','o','w','s','8','O','r','G','r','e','a','t','e','r',0};
	// 	char szIsWindowsServer[]			= {'I','s','W','i','n','d','o','w','s','S','e','r','v','e','r',0};

	char szGlobalMemoryStatusEx[] = { 'G','l','o','b','a','l','M','e','m','o','r','y','S','t','a','t','u','s','E','x',0 };
	char szKernel32Dll[] = { 'k','e','r','n','e','l','3','2','.','d','l','l',0 };
	char szVerifyVersionInfoW[] = { 'V','e','r','i','f','y','V','e','r','s','i','o','n','I','n','f','o','W',0 };
	char szVerSetConditionMask[] = { 'V','e','r','S','e','t','C','o','n','d','i','t','i','o','n','M','a','s','k',0 };
	char szCopyFileA[] = { 'C','o','p','y','F','i','l','e','A',0 };
	char szDllPsapi[] = { 'P','s','a','p','i','.','d','l','l',0 };
	char szEnumProcessModules[] = { 'E','n','u','m','P','r','o','c','e','s','s','M','o','d','u','l','e','s',0 };
	char szEnumProcesses[] = { 'E','n','u','m','P','r','o','c','e','s','s','e','s',0 };
	char szGetModuleFileNameExA[] = { 'G','e','t','M','o','d','u','l','e','F','i','l','e','N','a','m','e','E','x','A',0 };
	char szMoveFileA[] = { 'M','o','v','e','F','i','l','e','A',0 };
	char szOpenProcess[] = { 'O','p','e','n','P','r','o','c','e','s','s',0 };
	char szGetModuleBaseNameA[] = { 'G','e','t','M','o','d','u','l','e','B','a','s','e','N','a','m','e','A',0 };
	char szCreateRemoteThread[] = { 'C','r','e','a','t','e','R','e','m','o','t','e','T','h','r','e','a','d',0 };
	char szVirtualAllocEx[] = { 'V','i','r','t','u','a','l','A','l','l','o','c','E','x',0 };
	char szWriteProcessMemory[] = { 'W','r','i','t','e','P','r','o','c','e','s','s','M','e','m','o','r','y',0 };
	char szVirtualFreeEx[] = { 'V','i','r','t','u','a','l','F','r','e','e','E','x',0 };
	char szWaitForSingleObject[] = { 'W','a','i','t','F','o','r','S','i','n','g','l','e','O','b','j','e','c','t',0 };
	char szGetExitCodeThread[] = { 'G','e','t','E','x','i','t','C','o','d','e','T','h','r','e','a','d',0 };
	char szWaitForMultipleObjects[] = { 'W','a','i','t','F','o','r','M','u','l','t','i','p','l','e','O','b','j','e','c','t','s',0 };
	char szExitThread[] = { 'E','x','i','t','T','h','r','e','a','d',0 };

	char szCreateEventA[] = { 'C','r','e','a','t','e','E','v','e','n','t','A',0 };
	char szSetEvent[] = { 'S','e','t','E','v','e','n','t',0 };
	char szTerminateProcess[] = { 'T','e','r','m','i','n','a','t','e','P','r','o','c','e','s','s',0 };
	char szGetCurrentProcessId[] = { 'G','e','t','C','u','r','r','e','n','t','P','r','o','c','e','s','s','I','d',0 };
	char szSetFileAttributesA[] = { 'S','e','t','F','i','l','e','A','t','t','r','i','b','u','t','e','s','A',0 };

	char szDllNetApi32[] = { 'n','e','t','a','p','i','3','2','.','d','l','l',0 };
	char szNetScheduleJobAddW[] = { 'N','e','t','S','c','h','e','d','u','l','e','J','o','b','A','d','d',0 }; ;
	char szNetWkstaGetInfo[] = { 'N','e','t','W','k','s','t','a','G','e','t','I','n','f','o',0 };
	char szNetApiBufferFree[] = { 'N','e','t','A','p','i','B','u','f','f','e','r','F','r','e','e',0 };

	char szOpenProcessToken[] = { 'O','p','e','n','P','r','o','c','e','s','s','T','o','k','e','n',0 };
	char szLookupPrivilegeValueW[] = { 'L','o','o','k','u','p','P','r','i','v','i','l','e','g','e','V','a','l','u','e','W',0 };
	char szAdjustTokenPrivileges[] = { 'A','d','j','u','s','t','T','o','k','e','n','P','r','i','v','i','l','e','g','e','s',0 };
	char szOpenServiceA[] = { 'O','p','e','n','S','e','r','v','i','c','e','A',0 };
	char szCloseServiceHandle[] = { 'C','l','o','s','e','S','e','r','v','i','c','e','H','a','n','d','l','e',0 };
	char szQueryServiceStatus[] = { 'Q','u','e','r','y','S','e','r','v','i','c','e','S','t','a','t','u','s',0 };
	char szControlService[] = { 'C','o','n','t','r','o','l','S','e','r','v','i','c','e',0 };
	char szCreateServiceA[] = { 'C','r','e','a','t','e','S','e','r','v','i','c','e','A',0 };
	char szOpenSCManagerA[] = { 'O','p','e','n','S','C','M','a','n','a','g','e','r','A',0 };
	char szDeleteService[] = { 'D','e','l','e','t','e','S','e','r','v','i','c','e',0 };
	char szStartServiceA[] = { 'S','t','a','r','t','S','e','r','v','i','c','e','A',0 };

	char szDllShell32[] = { 's','h','e','l','l','3','2','.','d','l','l',0 };
	char szShellExecuteExA[] = { 'S','h','e','l','l','E','x','e','c','u','t','e','E','x','A',0 };
	char szShellExecuteExW[] = { 'S','h','e','l','l','E','x','e','c','u','t','e','E','x','W',0 };
	char szlstrcpyA[] = { 'l','s','t','r','c','p','y','A',0 };
	char szlstrcatA[] = { 'l','s','t','r','c','a','t','A',0 };
	char szlstrcmpA[] = { 'l','s','t','r','c','m','p','A',0 };
	char szDeleteFileW[] = { 'D','e','l','e','t','e','F','i','l','e','W',0 };
	char szGetCommandLineW[] = { 'G','e','t','C','o','m','m','a','n','d','L','i','n','e','W',0 };
	char szCommandLineToArgvW[] = { 'C','o','m','m','a','n','d','L','i','n','e','T','o','A','r','g','v','W',0 };
	char szReleaseMutex[] = { 'R','e','l','e','a','s','e','M','u','t','e','x',0 };
	char szSetCurrentDirectoryA[] = { 'S','e','t','C','u','r','r','e','n','t','D','i','r','e','c','t','o','r','y','A',0 };
	char szGetDiskFreeSpaceExA[] = { 'G','e','t','D','i','s','k','F','r','e','e','S','p','a','c','e','E','x','A',0 };

	char szRemoveDirectoryA[] = { 'R','e','m','o','v','e','D','i','r','e','c','t','o','r','y','A',0 };
	char szGetExitCodeProcess[] = { 'G','e','t','E','x','i','t','C','o','d','e','P','r','o','c','e','s','s',0 };
	char szCreatePipe[] = { 'C','r','e','a','t','e','P','i','p','e',0 };
	char szGetClassNameA[] = { 'G','e','t','C','l','a','s','s','N','a','m','e','A',0 };

	// 	char szIcmpCreateFile[]			= {'I','c','m','p','C','r','e','a','t','e','F','i','l','e',0};
	// 	char szIcmpSendEcho2[]			= {'I','c','m','p','S','e','n','d','E','c','h','o','2',0};
	// 	char szIcmpSendEcho2Ex[]		= {'I','c','m','p','S','e','n','d','E','c','h','o','2','E','x',0};
	// 	char szIcmpCloseHandle[]		= {'I','c','m','p','C','l','o','s','e','H','a','n','d','l','e',0};
	// 	char szIcmpParseReplies[]		= {'I','c','m','p','P','a','r','s','e','R','e','p','l','i','e','s',0};
	char szGetCurrentDirectoryA[] = { 'G','e','t','C','u','r','r','e','n','t','D','i','r','e','c','t','o','r','y','A',0 };
	char szDllShlwapi[] = { 's','h','l','w','a','p','i','.','d','l','l',0 };
	char szPathStripPathA[] = { 'P','a','t','h','S','t','r','i','p','P','a','t','h','A',0 };
	char szStrCmpIW[] = { 'S','t','r','C','m','p','I','W',0 };
	char szShellExecuteA[] = { 'S','h','e','l','l','E','x','e','c','u','t','e','A',0 };

	char szCreateStreamOnHGlobal[] = { 'C','r','e','a','t','e','S','t','r','e','a','m','O','n','H','G','l','o','b','a','l',0 };
	char szCoCreateInstance[] = { 'C','o','C','r','e','a','t','e','I','n','s','t','a','n','c','e',0 };
	char szCoUninitialize[] = { 'C','o','U','n','i','n','i','t','i','a','l','i','z','e',0 };
	char szCoInitialize[] = { 'C','o','I','n','i','t','i','a','l','i','z','e',0 };
	char szSHChangeNotify[] = { 'S','H','C','h','a','n','g','e','N','o','t','i','f','y',0 };

	char szWow64DisableWow64FsRedirection[] =
	{ 'W','o','w','6','4','D','i','s','a','b','l','e','W','o','w','6','4','F','s','R','e','d','i','r','e','c','t','i','o','n',0 };
	char szWow64RevertWow64FsRedirection[] =
	{ 'W','o','w','6','4','R','e','v','e','r','t','W','o','w','6','4','F','s','R','e','d','i','r','e','c','t','i','o','n',0 };

	char szFindResourceA[] = { 'F','i','n','d','R','e','s','o','u','r','c','e','A',0 };
	char szSizeofResource[] = { 'S','i','z','e','o','f','R','e','s','o','u','r','c','e',0 };
	char szLoadResource[] = { 'L','o','a','d','R','e','s','o','u','r','c','e',0 };
	char szLockResource[] = { 'L','o','c','k','R','e','s','o','u','r','c','e',0 };

	char szIsProcessDPIAware[] = { 'I','s','P','r','o','c','e','s','s','D','P','I','A','w','a','r','e',0 };
	char szSetProcessDPIAware[] = { 'S','e','t','P','r','o','c','e','s','s','D','P','I','A','w','a','r','e',0 };

	lpDllKernel32 = (HMODULE)GetKernel32Base();
	if (lpDllKernel32 == 0)
	{
		return FALSE;
}

	lpGetProcAddress = (ptrGetProcAddress)getGetProcAddress(lpDllKernel32);
	//lpGetProcAddress = (ptrGetProcAddress)GetProcAddress;

	lpLoadLibraryA = (ptrLoadLibraryA)lpGetProcAddress(lpDllKernel32, szLoadLibraryA);
	lpFreeLibrary = (ptrFreeLibrary)lpGetProcAddress(lpDllKernel32, szFreeLibrary);
	lpTerminateProcess = (ptrTerminateProcess)lpGetProcAddress(lpDllKernel32, szTerminateProcess);
	lpMoveFileA = (ptrMoveFileA)lpGetProcAddress(lpDllKernel32, szMoveFileA);
	lplstrcpyA = (ptrlstrcpyA)lpGetProcAddress(lpDllKernel32, szlstrcpyA);
	lplstrcatA = (ptrlstrcatA)lpGetProcAddress(lpDllKernel32, szlstrcatA);
	lplstrcmpA = (ptrlstrcmpA)lpGetProcAddress(lpDllKernel32, szlstrcmpA);
	lpDeleteFileW = (ptrDeleteFileW)lpGetProcAddress(lpDllKernel32, szDeleteFileW);
	lpGetCommandLineW = (ptrGetCommandLineW)lpGetProcAddress(lpDllKernel32, szGetCommandLineW);
	lpReleaseMutex = (ptrReleaseMutex)lpGetProcAddress(lpDllKernel32, szReleaseMutex);
	lpSetCurrentDirectoryA = (ptrSetCurrentDirectoryA)lpGetProcAddress(lpDllKernel32, szSetCurrentDirectoryA);
	lpGetDiskFreeSpaceExA = (ptrGetDiskFreeSpaceExA)lpGetProcAddress(lpDllKernel32, szGetDiskFreeSpaceExA);
	lpGetCurrentDirectoryA = (ptrGetCurrentDirectoryA)lpGetProcAddress(lpDllKernel32, szGetCurrentDirectoryA);
	lpRemoveDirectoryA = (ptrRemoveDirectoryA)lpGetProcAddress(lpDllKernel32, szRemoveDirectoryA);
	lpGetComputerNameA = (ptrGetComputerNameA)lpGetProcAddress(lpDllKernel32, szGetComputerNameA);
	lpGetModuleFileNameExA = (ptrGetModuleFileNameExA)lpGetProcAddress(lpDllKernel32, szGetModuleFileNameExA);
	lplstrcmpiA = (ptrlstrcmpiA)lpGetProcAddress(lpDllKernel32, szlstrcmpiA);
	lplstrlenA = (ptrlstrlenA)lpGetProcAddress(lpDllKernel32, szlstrlenA);
	lplstrlenW = (ptrlstrlenW)lpGetProcAddress(lpDllKernel32, szlstrlenW);
	lpDeleteFileA = (ptrDeleteFileA)lpGetProcAddress(lpDllKernel32, szDeleteFileA);
	lpCopyFileA = (ptrCopyFileA)lpGetProcAddress(lpDllKernel32, szCopyFileA);
	lpLoadLibraryA = (ptrLoadLibraryA)lpGetProcAddress(lpDllKernel32, szLoadLibraryA);
	lpCreateFileA = (ptrCreateFileA)lpGetProcAddress(lpDllKernel32, szCreateFileA);
	lpCreateFileW = (ptrCreateFileW)lpGetProcAddress(lpDllKernel32, szCreateFileW);
	lpWriteFile = (ptrWriteFile)lpGetProcAddress(lpDllKernel32, szWriteFile);
	lpCloseHandle = (ptrCloaseHandle)lpGetProcAddress(lpDllKernel32, szCloseHandle);
	lpVirtualAlloc = (ptrVirtualAlloc)lpGetProcAddress(lpDllKernel32, szVirtualAlloc);
	lpVirtualFree = (ptrVirtualFree)lpGetProcAddress(lpDllKernel32, szVirtualFree);
	lpGetModuleFileNameA = (ptrGetModuleFileNameA)lpGetProcAddress(lpDllKernel32, szGetModuleFileNameA);
	lpGetStartupInfoA = (ptrGetStartupInfoA)lpGetProcAddress(lpDllKernel32, szGetStartupInfoA);
	lpCreateProcessA = (ptrCreateProcessA)lpGetProcAddress(lpDllKernel32, szCreateProcessA);
	lpWinExec = (ptrWinExec)lpGetProcAddress(lpDllKernel32, szWinExec);
	lpGetSystemDirectoryA = (ptrGetSystemDirectoryA)lpGetProcAddress(lpDllKernel32, szGetSystemDirectoryA);
	lpExitProcess = (ptrExitProcess)lpGetProcAddress(lpDllKernel32, szExitProcess);
	lpGetModuleHandleA = (ptrGetModuleHandleA)lpGetProcAddress(lpDllKernel32, szGetModuleHandleA);
	lpGetFileSize = (ptrGetFileSize)lpGetProcAddress(lpDllKernel32, szGetFileSize);
	lpSetFilePointer = (ptrSetFilePointer)lpGetProcAddress(lpDllKernel32, szSetFilePointer);
	lpReadFile = (ptrReadFile)lpGetProcAddress(lpDllKernel32, szReadFile);
	lpCreateDirectoryA = (ptrCreateDirectoryA)lpGetProcAddress(lpDllKernel32, szCreateDirectoryA);
	lpGlobalAlloc = (ptrGlobalAlloc)lpGetProcAddress(lpDllKernel32, szGlobalAlloc);
	lpGlobalLock = (ptrGlobalLock)lpGetProcAddress(lpDllKernel32, szGlobalLock);
	lpGlobalUnlock = (ptrGlobalUnlock)lpGetProcAddress(lpDllKernel32, szGlobalUnlock);
	lpGlobalFree = (ptrGlobalFree)lpGetProcAddress(lpDllKernel32, szGlobalFree);
	lpGetTickCount = (ptrGetTickCount)lpGetProcAddress(lpDllKernel32, szGetTickCount);
	lpGetLocalTime = (ptrGetLocalTime)lpGetProcAddress(lpDllKernel32, szGetLocalTime);
	lpSleep = (ptrSleep)lpGetProcAddress(lpDllKernel32, szSleep);
	lpGetVolumeInformationA = (ptrGetVolumeInformationA)lpGetProcAddress(lpDllKernel32, szGetVolumeInformationA);
	lpGetCommandLineA = (ptrGetCommandLineA)lpGetProcAddress(lpDllKernel32, szGetCommandLineA);
	lpGetLogicalDriveStringsA = (ptrGetLogicalDriveStringsA)lpGetProcAddress(lpDllKernel32, szGetLogicalDriveStringsA);
	lpGetCurrentProcess = (ptrGetCurrentProcess)lpGetProcAddress(lpDllKernel32, szGetCurrentProcess);
	lpGlobalMemoryStatusEx = (ptrGlobalMemoryStatusEx)lpGetProcAddress(lpDllKernel32, szGlobalMemoryStatusEx);
	lpCreateToolhelp32Snapshot = (ptrCreateToolhelp32Snapshot)lpGetProcAddress(lpDllKernel32, szCreateToolhelp32Snapshot);
	lpProcess32First = (ptrProcess32First)lpGetProcAddress(lpDllKernel32, szProcess32First);
	lpProcess32Next = (ptrProcess32Next)lpGetProcAddress(lpDllKernel32, szProcess32Next);
	lpGetExitCodeProcess = (ptrGetExitCodeProcess)lpGetProcAddress(lpDllKernel32, szGetExitCodeProcess);
	lpCreatePipe = (ptrCreatePipe)lpGetProcAddress(lpDllKernel32, szCreatePipe);
	// 	lpVerifyVersionInfoW	= (ptrVerifyVersionInfoW)lpGetProcAddress(lpDllKernel32,szVerifyVersionInfoW);
	// 	lpVerSetConditionMask	= (ptrVerSetConditionMask)lpGetProcAddress(lpDllKernel32,szVerSetConditionMask);
	// 	lpGetVersionExA = (ptrGetVersionExA)lpGetProcAddress(lpDllKernel32,szGetVersionExA);
	lpGetDriveTypeA = (ptrGetDriveTypeA)lpGetProcAddress(lpDllKernel32, szGetDriveTypeA);
	lpGetSystemInfo = (ptrGetSystemInfo)lpGetProcAddress(lpDllKernel32, szGetSystemInfo);
	lpMoveFileExA = (ptrMoveFileExA)lpGetProcAddress(lpDllKernel32, szMoveFileExA);
	lpFindFirstFileA = (ptrFindFirstFileA)lpGetProcAddress(lpDllKernel32, szFindFirstFileA);
	lpFindNextFileA = (ptrFindNextFileA)lpGetProcAddress(lpDllKernel32, szFindNextFileA);
	lpFindClose = (ptrFindClose)lpGetProcAddress(lpDllKernel32, szFindClose);
	lpMultiByteToWideChar = (ptrMultiByteToWideChar)lpGetProcAddress(lpDllKernel32, szMultiByteToWideChar);
	lpWideCharToMultiByte = (ptrWideCharToMultiByte)lpGetProcAddress(lpDllKernel32, szWideCharToMultiByte);
	lpGlobalUnlock = (ptrGlobalUnlock)lpGetProcAddress(lpDllKernel32, szGlobalUnlock);
	lpDeviceIoControl = (ptrDeviceIoControl)lpGetProcAddress(lpDllKernel32, szDeviceIoControl);
	lpReadDirectoryChangesW = (ptrReadDirectoryChangesW)lpGetProcAddress(lpDllKernel32, szReadDirectoryChangesW);
	lpCreateThread = (ptrCreateThread)lpGetProcAddress(lpDllKernel32, szCreateThread);
	lpCreateMutexA = (ptrCreateMutexA)lpGetProcAddress(lpDllKernel32, szCreateMutexA);
	lpExitThread = (ptrExitThread)lpGetProcAddress(lpDllKernel32, szExitThread);
	lpGetCurrentProcessId = (ptrGetCurrentProcessId)lpGetProcAddress(lpDllKernel32, szGetCurrentProcessId);
	lpSetFileAttributesA = (ptrSetFileAttributesA)lpGetProcAddress(lpDllKernel32, szSetFileAttributesA);

	lpOpenProcess = (ptrOpenProcess)lpGetProcAddress(lpDllKernel32, szOpenProcess);
	lpCreateRemoteThread = (ptrCreateRemoteThread)lpGetProcAddress(lpDllKernel32, szCreateRemoteThread);
	lpVirtualAllocEx = (ptrVirtualAllocEx)lpGetProcAddress(lpDllKernel32, szVirtualAllocEx);
	lpWriteProcessMemory = (ptrWriteProcessMemory)lpGetProcAddress(lpDllKernel32, szWriteProcessMemory);
	lpVirtualFreeEx = (ptrVirtualFreeEx)lpGetProcAddress(lpDllKernel32, szVirtualFreeEx);
	lpWaitForSingleObject = (ptrWaitForSingleObject)lpGetProcAddress(lpDllKernel32, szWaitForSingleObject);
	lpGetExitCodeThread = (ptrGetExitCodeThread)lpGetProcAddress(lpDllKernel32, szGetExitCodeThread);
	lpCreateEventA = (ptrCreateEventA)lpGetProcAddress(lpDllKernel32, szCreateEventA);
	lpSetEvent = (ptrSetEvent)lpGetProcAddress(lpDllKernel32, szSetEvent);
	pfnWow64DisableWow64FsRedirection = (PFN_Wow64DisableWow64FsRedirection)lpGetProcAddress(lpDllKernel32, szWow64DisableWow64FsRedirection);
	pfnWow64RevertWow64FsRedirection = (PFN_Wow64RevertWow64FsRedirection)lpGetProcAddress(lpDllKernel32, szWow64RevertWow64FsRedirection);

	lpFindResourceA = (ptrFindResourceA)lpGetProcAddress(lpDllKernel32, szFindResourceA);
	lpSizeofResource = (ptrSizeofResource)lpGetProcAddress(lpDllKernel32, szSizeofResource);
	lpLoadResource = (ptrLoadResource)lpGetProcAddress(lpDllKernel32, szLoadResource);
	lpLockResource = (ptrLockResource)lpGetProcAddress(lpDllKernel32, szLockResource);

	// 	lpIsWindowsVersionOrGreater = (ptrIsWindowsVersionOrGreater)lpGetProcAddress(lpDllKernel32,szIsWindowsVersionOrGreater);
	// 	lprIsWindowsXPOrGreater = (ptrIsWindowsXPOrGreater)lpGetProcAddress(lpDllKernel32,szIsWindowsXPOrGreater);

	lpDllShell32 = lpLoadLibraryA(szDllShell32);
	lpSHChangeNotify = (ptrSHChangeNotify)lpGetProcAddress(lpDllShell32, szSHChangeNotify);
	lpShellExecuteExA = (ptrShellExecuteExA)lpGetProcAddress(lpDllShell32, szShellExecuteExA);
	lpShellExecuteExW = (ptrShellExecuteExW)lpGetProcAddress(lpDllShell32, szShellExecuteExW);
	lpShellExecuteA = (ptrShellExecuteA)lpGetProcAddress(lpDllShell32, szShellExecuteA);
	lpCommandLineToArgvW = (ptrCommandLineToArgvW)lpGetProcAddress(lpDllShell32, szCommandLineToArgvW);

	lpDllPsapi = (HMODULE)lpLoadLibraryA(szDllPsapi);
	lpEnumProcesses = (ptrEnumProcesses)lpGetProcAddress(lpDllPsapi, szEnumProcesses);
	lpEnumProcessModules = (ptrEnumProcessModules)lpGetProcAddress(lpDllPsapi, szEnumProcessModules);
	lpGetModuleBaseNameA = (ptrGetModuleBaseNameA)lpGetProcAddress(lpDllPsapi, szGetModuleBaseNameA);
	if (lpGetModuleFileNameExA == 0)
	{
		lpGetModuleFileNameExA = (ptrGetModuleFileNameExA)lpGetProcAddress(lpDllPsapi, szGetModuleFileNameExA);
	}

	lpDllntdll = (HMODULE)lpLoadLibraryA(szDllntdll);
	lpRtlGetLastWin32Error = (ptrRtlGetLastWin32Error)lpGetProcAddress(lpDllntdll, szRtlGetLastWin32Error);
	lpRtlZeroMemory = (ptrRtlZeroMomery)lpGetProcAddress(lpDllntdll, szRtlZeroMemory);
	lpRtlMoveMemory = (ptrRtlMoveMomery)lpGetProcAddress(lpDllntdll, szRtlMoveMemory);
	lpRtlCompareMemory = (ptrRtlCompareMomery)lpGetProcAddress(lpDllntdll, szRtlCompareMemory);
	// 	lpIsWindowsVistaOrGreater = (ptrIsWindowsVistaOrGreater)lpGetProcAddress(lpDllntdll,szIsWindowsVistaOrGreater);
	// 	lprIsWindows7OrGreater = (ptrIsWindows7OrGreater)lpGetProcAddress(lpDllntdll,szIsWindows7OrGreater);
	// 	lpIsWindows8OrGreater = (ptrIsWindows8OrGreater)lpGetProcAddress(lpDllntdll,szIsWindows8OrGreater);
	// 	lpIsWindowsServer = (ptrIsWindowsServer)lpGetProcAddress(lpDllntdll,szIsWindowsServer);

	lpDllUser32 = lpLoadLibraryA(szDllUser32);
	lpIsProcessDPIAware = (ptrIsProcessDPIAware)lpGetProcAddress(lpDllUser32, szIsProcessDPIAware);
	lpSetProcessDPIAware = (ptrSetProcessDPIAware)lpGetProcAddress(lpDllUser32, szSetProcessDPIAware);
	lpwsprintfA = (ptrwsprintfA)lpGetProcAddress(lpDllUser32, szwsprintfA);
	lpwsprintfW = (ptrwsprintfW)lpGetProcAddress(lpDllUser32, szwsprintfW);
	lpFindWindowA = (ptrFindWindowA)lpGetProcAddress(lpDllUser32, szFindWindowA);
	lpFindWindowExA = (ptrFindWindowExA)lpGetProcAddress(lpDllUser32, szFindWindowExA);
	lpGetDesktopWindow = (ptrGetDesktopWindow)lpGetProcAddress(lpDllUser32, szGetDesktopWindow);
	lpGetClassNameA = (ptrGetClassNameA)lpGetProcAddress(lpDllUser32, szGetClassNameA);
	lpFindWindowA = (ptrFindWindowA)lpGetProcAddress(lpDllUser32, szFindWindowA);
	lpRegisterClassExA = (ptrRegisterClassExA)lpGetProcAddress(lpDllUser32, szRegisterClassExA);
	lpTranslateMessage = (ptrTranslateMessage)lpGetProcAddress(lpDllUser32, szTranslateMessage);
	lpDispatchMessageA = (ptrDispatchMessageA)lpGetProcAddress(lpDllUser32, szDispatchMessageA);
	lpCreateWindowExA = (ptrCreateWindowExA)lpGetProcAddress(lpDllUser32, szCreateWindowExA);
	lpDefWindowProcA = (ptrDefWindowProcA)lpGetProcAddress(lpDllUser32, szDefWindowProcA);

	lpDllWs2_32 = lpLoadLibraryA(szDllWs2_32);
	lpsend = (ptrsend)lpGetProcAddress(lpDllWs2_32, szsend);
	lprecv = (ptrrecv)lpGetProcAddress(lpDllWs2_32, szrecv);
	lpclosesocket = (ptrclosesocket)lpGetProcAddress(lpDllWs2_32, szclosesocket);
	lpconnect = (ptrconnect)lpGetProcAddress(lpDllWs2_32, szconnect);
	lpinet_addr = (ptrinet_addr)lpGetProcAddress(lpDllWs2_32, szinet_addr);
	lpntohs = (ptrntohs)lpGetProcAddress(lpDllWs2_32, szntohs);
	lpgethostname = (ptrgethostname)lpGetProcAddress(lpDllWs2_32, szgethostname);
	lpgethostbyname = (ptrgethostbyname)lpGetProcAddress(lpDllWs2_32, szgethostbyname);
	lpsocket = (ptrsocket)lpGetProcAddress(lpDllWs2_32, szsocket);
	lpWSAStartup = (ptrWSAStartup)lpGetProcAddress(lpDllWs2_32, szWSAStartup);
	lpsetsockopt = (ptrsetsockopt)lpGetProcAddress(lpDllWs2_32, szsetsockopt);

	lpDlladvapi32 = lpLoadLibraryA(szDlladvapi32);
	lpAllocateAndInitializeSid = (ptrAllocateAndInitializeSid)lpGetProcAddress(lpDlladvapi32, szAllocateAndInitializeSid);
	lpEqualSid = (ptrEqualSid)lpGetProcAddress(lpDlladvapi32, szEqualSid);
	lpFreeSid = (ptrFreeSid)lpGetProcAddress(lpDlladvapi32, szFreeSid);
	lpRegDeleteKeyA = (ptrRegDeleteKeyA)lpGetProcAddress(lpDlladvapi32, szRegDeleteKeyA);
	lpRegCreateKeyA = (ptrRegCreateKeyA)lpGetProcAddress(lpDlladvapi32, szRegCreateKeyA);
	lpRegCloseKey = (ptrRegCloseKey)lpGetProcAddress(lpDlladvapi32, szRegCloseKey);
	lpRegQueryValueExA = (ptrRegQueryValueExA)lpGetProcAddress(lpDlladvapi32, szRegQueryValueExA);
	lpRegSetValueExA = (ptrRegSetValueExA)lpGetProcAddress(lpDlladvapi32, szRegSetValueExA);
	lpRegOpenKeyExA = (ptrRegOpenKeyExA)lpGetProcAddress(lpDlladvapi32, szRegOpenKeyExA);
	lpRegEnumKeyExA = (ptrRegEnumKeyExA)lpGetProcAddress(lpDlladvapi32, szRegEnumKeyExA);
	lpRegCreateKeyExA = (ptrRegCreateKeyExA)lpGetProcAddress(lpDlladvapi32, szRegCreateKeyExA);
	lpOpenProcessToken = (ptrOpenProcessToken)lpGetProcAddress(lpDlladvapi32, szOpenProcessToken);
	lpLookupPrivilegeValueW = (ptrLookupPrivilegeValueW)lpGetProcAddress(lpDlladvapi32, szLookupPrivilegeValueW);
	lpAdjustTokenPrivileges = (ptrAdjustTokenPrivileges)lpGetProcAddress(lpDlladvapi32, szAdjustTokenPrivileges);
	lpOpenServiceA = (ptrOpenServiceA)lpGetProcAddress(lpDlladvapi32, szOpenServiceA);
	lpCloseServiceHandle = (ptrCloseServiceHandle)lpGetProcAddress(lpDlladvapi32, szCloseServiceHandle);
	lpQueryServiceStatus = (ptrQueryServiceStatus)lpGetProcAddress(lpDlladvapi32, szQueryServiceStatus);
	lpControlService = (ptrControlService)lpGetProcAddress(lpDlladvapi32, szControlService);
	lpCreateServiceA = (ptrCreateServiceA)lpGetProcAddress(lpDlladvapi32, szCreateServiceA);
	lpOpenSCManagerA = (ptrOpenSCManagerA)lpGetProcAddress(lpDlladvapi32, szOpenSCManagerA);
	lpDeleteService = (ptrDeleteService)lpGetProcAddress(lpDlladvapi32, szDeleteService);
	lpStartServiceA = (ptrStartServiceA)lpGetProcAddress(lpDlladvapi32, szStartServiceA);
	lpGetUserNameA = (ptrGetUserNameA)lpGetProcAddress(lpDlladvapi32, szGetUserNameA);
	lpGetTokenInformation = (ptrGetTokenInformation)lpGetProcAddress(lpDlladvapi32, szGetTokenInformation);

	lpDlliphlpapi = lpLoadLibraryA(szDlliphlpapi);
	lpGetAdaptersInfo = (ptrGetAdaptersInfo)lpGetProcAddress(lpDlliphlpapi, szGetAdaptersInfo);
	// 	lpIcmpCreateFile =(ptrIcmpCreateFile)lpGetProcAddress(lpDlliphlpapi,szIcmpCreateFile);
	// 	lpIcmpSendEcho2 =(ptrIcmpSendEcho2)lpGetProcAddress(lpDlliphlpapi,szIcmpSendEcho2);
	// 	lpIcmpSendEcho2Ex =(ptrIcmpSendEcho2Ex)lpGetProcAddress(lpDlliphlpapi,szIcmpSendEcho2Ex);
	// 	lpIcmpCloseHandle =(ptrIcmpCloseHandle)lpGetProcAddress(lpDlliphlpapi,szIcmpCloseHandle);
	// 	lpIcmpParseReplies =(ptrIcmpParseReplies)lpGetProcAddress(lpDlliphlpapi,szIcmpParseReplies);

	lpDllOle32 = lpLoadLibraryA(szDllOle32);
	_lpCreateStreamOnHGlobal = (_ptrCreateStreamOnHGlobal)lpGetProcAddress(lpDllOle32, szCreateStreamOnHGlobal);
	lpCoCreateInstance = (ptrCoCreateInstance)lpGetProcAddress(lpDllOle32, szCoCreateInstance);
	lpCoUninitialize = (ptrCoUninitialize)lpGetProcAddress(lpDllOle32, szCoUninitialize);
	lpCoInitialize = (ptrCoInitialize)lpGetProcAddress(lpDllOle32, szCoInitialize);

	//lpDllSetupApi = lpLoadLibraryA(szDllSetupApi);
	//lpSetupDiEnumDeviceInfo = (ptrSetupDiEnumDeviceInfo)lpGetProcAddress(lpDllSetupApi,szSetupDiEnumDeviceInfo);
	//lpSetupDiSetClassInstallParamsA = (ptrSetupDiSetClassInstallParamsA)lpGetProcAddress(lpDllSetupApi,szSetupDiSetClassInstallParamsA);
	//lpSetupDiCallClassInstaller = (ptrSetupDiCallClassInstaller)lpGetProcAddress(lpDllSetupApi,szSetupDiCallClassInstaller);
	//lpSetupDiGetClassDevsA = (ptrSetupDiGetClassDevsA)lpGetProcAddress(lpDllSetupApi,szSetupDiGetClassDevsA);
	//lpSetupDiOpenClassRegKey = (ptrSetupDiOpenClassRegKey)lpGetProcAddress(lpDllSetupApi,szSetupDiOpenClassRegKey);

	lpDllNetApi32 = lpLoadLibraryA(szDllNetApi32);
	lpNetScheduleJobAddW = (ptrNetScheduleJobAddW)lpGetProcAddress(lpDllNetApi32, szNetScheduleJobAddW);
	lpNetWkstaGetInfo = (ptrNetWkstaGetInfo)lpGetProcAddress(lpDllNetApi32, szNetWkstaGetInfo);
	lpNetApiBufferFree = (ptrNetApiBufferFree)lpGetProcAddress(lpDllNetApi32, szNetApiBufferFree);

	lpDllShlwapi = lpLoadLibraryA(szDllShlwapi);
	pPathStripPathA = (ptrPathStripPathA)lpGetProcAddress(lpDllShlwapi, szPathStripPathA);
	lpStrCmpIW = (ptrStrCmpIW)lpGetProcAddress(lpDllShlwapi, szStrCmpIW);

	_lpstrstr = (_ptrstrstr)_strstr;
	_lpHexStrStr = (_ptrHexStrStr)_HexStrStr;
	lpwcscmp = (ptrwcscmp)_wcscmp;

	//before process be overed,u can not free library
// 	int iRet = lpFreeLibrary(lpDllAvicap32);
// 	iRet = lpFreeLibrary(lpDllKernel32);
// 	iRet = lpFreeLibrary(lpDllUser32);
// 	iRet = lpFreeLibrary(lpDllWinmm);
// 	iRet = lpFreeLibrary(lpDllSetupApi);
// 	iRet = lpFreeLibrary(lpDllOle32);
// 	iRet = lpFreeLibrary(lpDllGdiPlus);
// 	iRet = lpFreeLibrary(lpDllntdll);
// 	iRet = lpFreeLibrary(lpDllGdi32);
// 	iRet = lpFreeLibrary(lpDllWs2_32);
// 	iRet = lpFreeLibrary(lpDlladvapi32);
// 	iRet = lpFreeLibrary(lpDlliphlpapi);
	return TRUE;
}


HMODULE GetKernel32Base()
{
#ifndef _WIN64
	HMODULE hKernel32 = 0;
	__asm
	{
		//assume fs:nothing
		push esi
		xor eax, eax
		mov eax, fs: [eax + 30h] ; 指向PEB的指针
		mov eax, [eax + 0ch]; 指向PEB_LDR_DATA的指针
		mov esi, [eax + 0ch]; 根据PEB_LDR_DATA得出InLoadOrderModuleList的Flink字段
		lodsd
		mov eax, [eax]; 指向下一个节点
		mov eax, [eax + 18h]; Kernel.dll的基地址
		mov hKernel32, eax
		pop esi
	}

	return hKernel32;
#else
	char szKernel32[] = { 'k','e','r','n','e','l','3','2','.','d','l','l',0 };
	return GetModuleHandleA(szKernel32);
#endif
}




FARPROC getGetProcAddress(HMODULE hKernel32)
{
	char szGetProcAdress[] = { 'G','e','t','P','r','o','c','A','d','d','r','e','s','s',0 };
#ifndef _WIN64
	FARPROC pGetProcAddr = 0;
	__asm
	{
		pushad
		mov edi, hKernel32

		mov eax, [edi + 3ch]

		mov edx, [edi + eax + 78h]		//export rva

		add edx, edi

		mov ecx, [edx + 18h]			//numberOfNames

		mov ebx, [edx + 20h]			//AddressOfNames of rva

		add ebx, edi					//AddressOfNames

		search :
		push ecx

			push edx
			push ebx

			push edi

			mov esi, [ebx + ecx * 4]		//function name rva

			add esi, edi					//function name address


			push esi
			lea eax, szGetProcAdress
			push eax
			call lstrcmpiA					//stdcall

			pop edi
			pop ebx
			pop edx

			pop ecx

			cmp eax, 0
			jz _findAddress
			loop search

			jmp _notFoundAdress

			_findAddress :

		mov ebx, [edx + 24h]			//AddressOfNameOrdinals

			add ebx, edi

			movzx ecx, WORD PTR[ebx + ecx * 2]

			mov ebx, [edx + 1ch]			//addressOfFunctions

			add ebx, edi

			mov eax, [ebx + ecx * 4]

			add eax, edi

			mov pGetProcAddr, eax
			jmp _searchEnd

			_notFoundAdress :
		mov eax, 0
			mov pGetProcAddr, eax

			_searchEnd :
		popad
	}

	return  pGetProcAddr;

#else
	return (FARPROC)GetProcAddress;
#endif
}







