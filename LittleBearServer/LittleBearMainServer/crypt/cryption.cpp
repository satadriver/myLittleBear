#include "cryption.h"

unsigned char gKey[64] = "fuck all the windows cracker";

void xorCrypt(unsigned char * data, int len, unsigned char * key,int keylen) {
	for (int i = 0,j = 0;i < len;i ++)
	{
		data[i] ^= key[j];
		j++;
		if (j >= keylen)
		{
			j = 0;
		}
	}
}