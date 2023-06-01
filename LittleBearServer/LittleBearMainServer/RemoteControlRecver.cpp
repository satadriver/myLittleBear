
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

unsigned char RemoteClientKey[4] = { 0 };
STMOUSEACTION RemoteClientLButtonClick = { 0 };
STMOUSEACTION RemoteClientLButtonDoubleClick = { 0 };
STMOUSEACTION RemoteClientRButtonClick = { 0 };
STMOUSEACTION RemoteClientRButtonDoubleClick = { 0 };
REMOTECONTROLWHEEL RemoteMouseWheel = { 0 };



LRESULT CALLBACK RemoteControler::RemoteControlWindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_PAINT: {
		PAINTSTRUCT stPS = { 0 };
		HDC hdcDst = BeginPaint(hWnd, &stPS);

		HDC  hdcScr = CreateDCA("DISPLAY", NULL, NULL, NULL);
		HDC hdcSource = CreateCompatibleDC(hdcScr);

		mapit = RemoteCtrlParamMap.find(hWnd);
		if (mapit == RemoteCtrlParamMap.end())
		{
			WriteLog("can not found bitmap buffer in map\r\n");
			DeleteDC(hdcSource);
			DeleteDC(hdcScr);
			DeleteDC(hdcDst);
			EndPaint(hWnd, &stPS);
			return 0;
		}

		char* lpClientBitmap = mapit->second.lpClientBitmap;
		LPBITMAPFILEHEADER pBMFH = (LPBITMAPFILEHEADER)lpClientBitmap;
		void* pDibts = (void*)(lpClientBitmap + pBMFH->bfOffBits);
		LPBITMAPINFOHEADER pBMIH = (LPBITMAPINFOHEADER)(lpClientBitmap + sizeof(BITMAPFILEHEADER));
		DWORD dwDibtsSize = ((pBMIH->biWidth * pBMIH->biBitCount + 31) / 32) * 4 * pBMIH->biHeight;
		char* pRemoteSrnData = 0;
		HBITMAP hRemoteBM = CreateDIBSection(0, (LPBITMAPINFO)pBMIH, DIB_RGB_COLORS, (void**)&pRemoteSrnData, 0, 0);
		if (hRemoteBM)
		{
			memmove(pRemoteSrnData, pDibts, dwDibtsSize);
			HBITMAP hSrcBM = (HBITMAP)SelectObject(hdcSource, hRemoteBM);

			int iX = pBMIH->biWidth;
			int iY = pBMIH->biHeight;
			RECT stRect = { 0 };
			int iRet = GetClientRect(hWnd, &stRect);
			//iRet = BitBlt(hdcDst, 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top,hdcSrc, 0, 0, SRCCOPY);
			iRet = StretchBlt(hdcDst, 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top, hdcSource, 0, 0, iX, iY, SRCCOPY);
			DeleteObject(hSrcBM);
		}

		DeleteObject(hRemoteBM);
		DeleteDC(hdcScr);
		DeleteDC(hdcSource);
		DeleteDC(hdcDst);
		EndPaint(hWnd, &stPS);
		return 0;
	}

	case WM_KEYDOWN: {
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

	char* szWindowNameFormat = "<RemoteControl>_%s";
	char szWindowName[MAX_PATH] = "";
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
		wsprintfA(szShowInfo, "RemoteControlWindow RegisterClassExA error code:%u\r\n", GetLastError());
		WriteLog(szShowInfo);
		return FALSE;
	}

	DEVMODE devmode = { 0 };
	devmode.dmSize = sizeof(DEVMODE);
	BOOL Isgetdisplay = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	if (Isgetdisplay == FALSE)
	{
		nAtom = UnregisterClassA(szWindowName, (HINSTANCE)0x400000);
		return FALSE;
	}

	int xscrn = devmode.dmPelsWidth;
	int yscrn = devmode.dmPelsHeight;

	lpparam->hwndWindow = CreateWindowExA(WS_EX_CLIENTEDGE, szWindowName, szWindowName, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_MAXIMIZE | WS_HSCROLL | WS_VSCROLL,
		0, 0, xscrn, yscrn, NULL, NULL, 0, NULL);
	if (NULL == lpparam->hwndWindow)
	{
		nAtom = UnregisterClassA(szWindowName, (HINSTANCE)0x400000);
		WriteLog("RemoteControlWindow CreateWindowExA error\r\n");
		return NULL;
	}

	REMOTE_CONTROL_PARAM stParam = *lpparam;

	int iRet = ShowWindow(lpparam->hwndWindow, SW_SHOWMAXIMIZED);
	iRet = UpdateWindow(lpparam->hwndWindow);

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
			iRet = TranslateMessage(&msg);//转化虚拟按键到字符消息
			iRet = DispatchMessageA(&msg);
		}
	}

	nAtom = UnregisterClassA(szWindowName, (HINSTANCE)0x400000);

	//if socket closed by window,return 0

	char tmppack[1024];
	iRet = Commander::SendCmdPacket(&stParam.unique, stParam.hSockClient, tmppack, 1024, REMOTECONTROL_END);

	iRet = closesocket(stParam.hSockClient);		//if socket closed by window,return 0

	Sleep(3000);	//if no wait,uncompress buffer will cause exception in sometime
	delete stParam.lpbmpDataSize;
	delete stParam.lpControlWindowClose;

	delete[] stParam.lpClientBitmap;
	stParam.lpClientBitmap = 0;
	mapit = RemoteCtrlParamMap.find(stParam.hwndWindow);
	if (mapit != RemoteCtrlParamMap.end())
	{
		RemoteCtrlParamMap.erase(mapit);
	}

	return TRUE;
}






int __stdcall RemoteControler::RemoteControlProc(LPREMOTE_CONTROL_PARAM lpParam)
{
	REMOTE_CONTROL_PARAM stParam = *lpParam;
	int iRet = 0;

	char* lpRecvBuf = new char[NETWORK_BUF_SIZE];
	if (lpRecvBuf)
	{
		iRet = RemoteControl(stParam, &lpRecvBuf, NETWORK_BUF_SIZE);

	}
	else {
		WriteLog("RemoteControlListen new operator client screen recv data buffer error\r\n");
	}

	iRet = SendMessageA(stParam.hwndWindow, WM_CLOSE, 0, 0);	//if connection is shut by client,need to close window

	delete[]lpRecvBuf;
	return FALSE;
}








//any error will return to main listen thread
int RemoteControler::RemoteControl(REMOTE_CONTROL_PARAM stParam, char** lpRecvBuf, int BufSize)		//caution here!!!
{
	POINT stLastServerMousePos = { 0 };

	NETWORKPROCPARAM stNetWorkParam = { 0 };
	stNetWorkParam.hSockClient = stParam.hSockClient;
	stNetWorkParam.stAddrClient = stParam.stAddrClient;

	char szShowInfo[1024];
	char szClientInfo[MAX_PATH];
	int iRet = 0;

	int dwMaxRecvBufSize = BufSize;

	int framecnt = 0;
	while (TRUE) {

		int dwPackSize = recv(stParam.hSockClient, *lpRecvBuf, dwMaxRecvBufSize, 0);
		LPNETWORKPACKETHEADER lphdr = (LPNETWORKPACKETHEADER)*lpRecvBuf;
		int dwHdrLen = sizeof(NETWORKPACKETHEADER);
		int dwDataSize = lphdr->packlen;

		UNIQUECLIENTSYMBOL stUnique = lphdr->unique;
		DWORD dwCommand = lphdr->cmd;
		if (dwPackSize < dwHdrLen || (lphdr->cmd != REMOTE_CLIENT_SCREEN && dwCommand != REMOTE_NONE_SCREEN) ||
			dwDataSize < 0 || dwDataSize > stParam.bufLimit || *stParam.lpControlWindowClose)
		{
			iRet = GetLastError();
			InetAddrFormatString(stNetWorkParam, szClientInfo);
			wsprintfA(szShowInfo, "RemoteControlProc recv error code:%u from client:%s\r\n", iRet, szClientInfo);
			WriteLog(szShowInfo);
			return FALSE;
		}
		else if (dwDataSize > BufSize - dwHdrLen && dwDataSize <= stParam.bufLimit - dwHdrLen)
		{
			char* lpNewBuf = new char[dwDataSize + sizeof(NETWORKPACKETHEADER)];
			memmove(lpNewBuf, *lpRecvBuf, dwPackSize);
			delete[] * lpRecvBuf;
			*lpRecvBuf = lpNewBuf;
			BufSize = dwDataSize + sizeof(NETWORKPACKETHEADER);
			dwMaxRecvBufSize = dwDataSize + sizeof(NETWORKPACKETHEADER);
		}

		int iDamagedPacketFlag = 0;
		while (dwDataSize > dwPackSize - dwHdrLen)
		{
			int iNextPackSize = recv(stParam.hSockClient, *lpRecvBuf + dwPackSize, dwMaxRecvBufSize - dwPackSize, 0);
			if (iNextPackSize > 0)
			{
				dwPackSize += iNextPackSize;
			}
			else
			{
				int iRet = WSAGetLastError();
				if (iRet == 0 || dwDataSize == dwPackSize - dwHdrLen)
				{
					break;
				}
				else {
					iDamagedPacketFlag = TRUE;
					break;
				}
			}
		}

		if (iDamagedPacketFlag) {
			WriteLog("RemoteControlProc recv splitted packet slices error\r\n");
			return FALSE;
		}

		int iconic = IsIconic(stParam.hwndWindow);			//if IsIconic() == true,GetClientRect will fail
		if (iconic)
		{
			iRet = Commander::SendCmdPacket(&stUnique, stParam.hSockClient, *lpRecvBuf, BufSize, RECV_DATA_OK);
			if (iRet <= 0)
			{
				WriteLog("RemoteControlProc send RECV_DATA_OK error\r\n");
				return FALSE;
			}
			else {
				wsprintfA(szShowInfo, "RemoteControlProc IsIconic\r\n");
				WriteLog(szShowInfo);
			}
			//Sleep(0);
			continue;
		}

		//int iszoom = IsZoomed(stParam.hwndWindow);		
		RECT stWindowClientRect = { 0 };
		POINT stPtMouseNow = { 0 };
		int getclient = GetClientRect(stParam.hwndWindow, &stWindowClientRect);
		int getcurpos = GetCursorPos(&stPtMouseNow);
		int clientrectok = ScreenToClient(stParam.hwndWindow, &stPtMouseNow);
		if (getclient == FALSE || getcurpos == FALSE || clientrectok == FALSE) {
			iRet = Commander::SendCmdPacket(&stUnique, stParam.hSockClient, *lpRecvBuf, BufSize, RECV_DATA_OK);
			if (iRet <= 0)
			{
				WriteLog("RemoteControlProc send RECV_DATA_OK error\r\n");
				return FALSE;
			}
			else {
				wsprintfA(szShowInfo, "RemoteControlProc GetClientRect():%u or GetCursorPos():%u or ScreenToClient():%u error\r\n",
					getclient, getcurpos, clientrectok);
				WriteLog(szShowInfo);
			}
			//Sleep(0);
			continue;
		}

		if (dwCommand == REMOTE_CLIENT_SCREEN)
		{
			LPNETWORKFILEHDR lpfhdr = (LPNETWORKFILEHDR)*lpRecvBuf;
			char* lpcompdata = *lpRecvBuf + sizeof(NETWORKFILEHDR);
			int compsize = lpfhdr->packhdr.packlen - sizeof(int);
			int uncomplen = lpfhdr->len;
			long lZlibBufLen = stParam.bufLimit;
			if (lZlibBufLen < uncomplen || stParam.lpClientBitmap == 0)
			{
				WriteLog("RemoteControlProc decompress buffer size is not enough\r\n");
				return FALSE;
			}

			iRet = uncompress((Bytef*)stParam.lpClientBitmap, (uLongf*)&lZlibBufLen, (const Bytef*)lpcompdata, (uLongf)compsize);
			if (iRet != Z_OK || *(WORD*)(stParam.lpClientBitmap) != 0x4d42)
			{
				InetAddrFormatString(stNetWorkParam, szClientInfo);
				wsprintfA(szShowInfo, "RemoteControlProc unzip error or bitmap file format error,client:%s\r\n", szClientInfo);
				WriteLog(szShowInfo);
				//if connection is shut by client,need to close window
				return FALSE;
			}
			else {
				framecnt++;
				if (framecnt % 10 == 0)
				{
					wsprintfA(szShowInfo, "RemoteControlProc uncompress ok,zip size:%u,unzip size:%u\r\n", compsize, lZlibBufLen);
					WriteLog(szShowInfo);
				}
			}

			iRet = InvalidateRect(stParam.hwndWindow, &stWindowClientRect, FALSE);
			//false will make it more faster and not lighting when reflash
			//如果客户区全部无效 UpdateWindow将导致Windows用WM_PAINT消息调用窗口过程（如果整个客户区有效，则不调用窗口过程）
			//如果客户区部分有效 这一WM_PAINT消息不进入消息队列，直接由WINDOWS调用窗口过程
			//UpdateWindow(hWndRemoteCtrl);
			//RedrawWindow(hWndRemoteCtrl,&stRect,0,RDW_INTERNALPAINT);

		}
		else if (dwCommand == REMOTE_NONE_SCREEN)
		{
			/*
			InetAddrFormatString(stNetWorkParam,szClientInfo);
			wsprintfA(szShowInfo,"RemoteControlProc recv unrecognized command,client:%s\r\n",szClientInfo);
			WriteLog(szShowInfo);

			iRet = SendCmdPacket(&stUnique,stParam.hSockClient,*lpRecvBuf,BufSize,RECV_DATA_OK);
			return iRet;
			*/

			/*
			lphdr = (LPNETWORKPACKETHEADER)*lpRecvBuf;
			lphdr->cmd = RECV_DATA_OK;
			lphdr->unique = stUnique;
			lphdr->packlen = 0;
			iRet = send(stParam.hSockClient,(char*)lphdr,sizeof(NETWORKPACKETHEADER),0);
			return FALSE;
			*/

		}
		else {

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
			*(DWORD*)(szSendBuf + iSendDataLen) = RECV_DATA_OK;
			iSendDataLen += sizeof(DWORD);
			*(LPUNIQUECLIENTSYMBOL)(szSendBuf + iSendDataLen) = stUnique;
			iSendDataLen += sizeof(UNIQUECLIENTSYMBOL);
			*(DWORD*)(szSendBuf + iSendDataLen) = 0;
			iSendDataLen += sizeof(DWORD);
		}

		iRet = send(stParam.hSockClient, szSendBuf, iSendDataLen, 0);
		if (iRet <= 0)
		{
			iRet = GetLastError();
			wsprintfA(szShowInfo, "RemoteControlProc send error code:%u\r\n", iRet);
			WriteLog(szShowInfo);
			return FALSE;
		}

		//Sleep(0);
		continue;
	}

	return TRUE;
}