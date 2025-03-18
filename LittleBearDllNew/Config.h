#pragma once

#ifndef CONFIG_H_H_H
#define CONFIG_H_H_H

#pragma  pack(1)

typedef struct
{
	char username[32];
	char ip[32];
	int mode;
	char unused[60];
}ATTACK_RUN_PARAM, * LPATTACK_RUN_PARAM;

#pragma pack()



int getUserAndServer();

int networkInit();


#endif




