#pragma once

/*
 Name: HttpLib.h
 Copyright: 山东兆物网络技术有限公司
 Author: 陆腾
 Date: 2008-11-12 15:20
 Description: 通讯库接口头文件 
*/

#ifndef _HTTPLIB_H
#define _HTTPLIB_H

#include <afx.h>

//回调函数类型声明
typedef CString (*PCOMMCALLBACKPROC)(LPCSTR lpVirtualDir, LPCSTR lpContent,	ULONG& DataLen);

#ifndef COMMAPI
#define COMMAPI __stdcall
#endif

// #ifdef __cplusplus
// extern "C" {
// #endif

/*
 添加名称服务器
       [IN]lpNameServerIP 名称服务器的IP
 返回值: TRUE:成功 FALSE:失败
*/
BOOL COMMAPI CommAddNameServer(LPCSTR lpNameServerIP);

/*
 指定监听端口和回调函数地址
       [IN]sLocalPort 要监听的本地端口
	   [IN]procGetResult 处理命令的回调函数地址
 返回值: TRUE:成功 FALSE:失败
*/
BOOL COMMAPI CommListen(USHORT sLocalPort, PCOMMCALLBACKPROC procGetResult);

/*
 发送命令并获取结果
       [IN]lpDest 对方机器编号或IP
	   [IN]sDestPort 对方端口
	   [IN]lpVirtualDir 请求行内容
	   [IN]lpContent 命令内容
	   [IN]nDataLength 命令长度
 返回值: 远程返回的结果
*/
CString COMMAPI CommGetData(LPCSTR lpDest, USHORT sDestPort, LPCSTR lpVirtualDir, LPCSTR lpContent, int nDataLenth);

/*
 得到版本信息
 返回值: 编译的日期和时间
*/
LPCSTR COMMAPI CommGetVersion();


// #ifdef __cplusplus
// }
// #endif

#endif
