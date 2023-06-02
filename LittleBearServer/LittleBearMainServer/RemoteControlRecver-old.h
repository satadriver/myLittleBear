#pragma once
#ifndef REMOTECONTROLPROC_H_H_H
#define REMOTECONTROLPROC_H_H_H

#include <windows.h>
#include "main.h"
#include "Public.h"




extern map <HWND, REMOTE_CONTROL_PARAM> RemoteCtrlParamMap;

extern map<HWND, REMOTE_CONTROL_PARAM >::iterator mapit;

extern unsigned char RemoteClientKey[4];
extern STMOUSEACTION RemoteClientLButtonClick;
extern STMOUSEACTION RemoteClientLButtonDoubleClick;
extern STMOUSEACTION RemoteClientRButtonClick;
extern STMOUSEACTION RemoteClientRButtonDoubleClick;





class RemoteControler {
public:
	static LRESULT CALLBACK RemoteControlWindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static BOOL __stdcall RemoteControlWindow(LPREMOTE_CONTROL_PARAM lpparam);

	static int RemoteControl(REMOTE_CONTROL_PARAM stParam, char** lpRecvBuf, int BufSize);
	static int __stdcall RemoteControlProc(LPREMOTE_CONTROL_PARAM lpParam);
};






#endif