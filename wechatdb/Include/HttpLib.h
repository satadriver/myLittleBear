#pragma once

/*
 Name: HttpLib.h
 Copyright: ɽ���������缼�����޹�˾
 Author: ½��
 Date: 2008-11-12 15:20
 Description: ͨѶ��ӿ�ͷ�ļ� 
*/

#ifndef _HTTPLIB_H
#define _HTTPLIB_H

#include <afx.h>

//�ص�������������
typedef CString (*PCOMMCALLBACKPROC)(LPCSTR lpVirtualDir, LPCSTR lpContent,	ULONG& DataLen);

#ifndef COMMAPI
#define COMMAPI __stdcall
#endif

// #ifdef __cplusplus
// extern "C" {
// #endif

/*
 ������Ʒ�����
       [IN]lpNameServerIP ���Ʒ�������IP
 ����ֵ: TRUE:�ɹ� FALSE:ʧ��
*/
BOOL COMMAPI CommAddNameServer(LPCSTR lpNameServerIP);

/*
 ָ�������˿ںͻص�������ַ
       [IN]sLocalPort Ҫ�����ı��ض˿�
	   [IN]procGetResult ��������Ļص�������ַ
 ����ֵ: TRUE:�ɹ� FALSE:ʧ��
*/
BOOL COMMAPI CommListen(USHORT sLocalPort, PCOMMCALLBACKPROC procGetResult);

/*
 ���������ȡ���
       [IN]lpDest �Է�������Ż�IP
	   [IN]sDestPort �Է��˿�
	   [IN]lpVirtualDir ����������
	   [IN]lpContent ��������
	   [IN]nDataLength �����
 ����ֵ: Զ�̷��صĽ��
*/
CString COMMAPI CommGetData(LPCSTR lpDest, USHORT sDestPort, LPCSTR lpVirtualDir, LPCSTR lpContent, int nDataLenth);

/*
 �õ��汾��Ϣ
 ����ֵ: ��������ں�ʱ��
*/
LPCSTR COMMAPI CommGetVersion();


// #ifdef __cplusplus
// }
// #endif

#endif
