#pragma once

extern unsigned char gKey[64];

void xorCrypt(unsigned char * data, int len, unsigned char * key,int keylen);