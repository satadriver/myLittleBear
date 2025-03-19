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


void CryptData(unsigned char* pdata, int size, unsigned char* psrckey, int keylen) {

	unsigned char* pkey = new unsigned char[keylen];
	for (int i = 0; i < keylen; i++)
	{
		pkey[i] = (~psrckey[i]) ^ 0x2b;
	}

	for (int i = 0, j = 0; i < size;)
	{
		pdata[i] ^= pkey[j];
		j++;
		if (j == keylen)
		{
			j = 0;
		}
		i++;
	}

	delete[]pkey;
}
