#pragma once
#ifndef COMMANDPROC_H_H_H
#define COMMANDPROC_H_H_H

#include <windows.h>
#include "main.h"

class CommandRecver {
public:
	static int __stdcall CommandRecver::CommandRecverProc(LPNETWORKPROCPARAM lpParam);
	static int __stdcall CommandRecverProcess(LPNETWORKPROCPARAM lpParam, char* lpdata);
	static int CommandRecver::SendHeartBeat(LPUNIQUECLIENTSYMBOL lpUnique, NETWORKPROCPARAM stParam, char* lpdata, int datasize);
};


#endif