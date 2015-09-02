#include "aes.h"
#include "openssl/aes.h"
#include "openssl/rand.h"
#include <string.h>

int AESGenerateKey(byte* key, int keylen)
{
	if(keylen > 32 || !key || keylen%8 != 0)
	{
		return -1;
	}
	for(int i = 0; i < keylen; i = i + 8)
	{
		if(!RAND_bytes(key, sizeof(key)))
		{
			return 0;
		}

		key = key + 8;
	}

	return keylen;
}

int GetEncriptedDataLen(int datalen)
{
	int rest = 0;
	if(datalen%AES_BLOCK_SIZE != 0)
	{
		rest = (datalen/AES_BLOCK_SIZE + 1)*AES_BLOCK_SIZE - datalen;
	}

	return datalen + rest;
}

int AESEncrypt(byte* key, int keylength,
			   byte* data, int datalen,
			   byte* encryptedData, int encryptedDataLen)
{
	
	int realDataLen = GetEncriptedDataLen(datalen);

	if(encryptedDataLen < realDataLen || !encryptedData)
	{
		return -1;
	}
	else if(realDataLen > MAX_BUFFER_LEN || !data || realDataLen == 0)
	{
		return -2;
	}
	else if(keylength > 32 || !key || keylength%8 != 0)
	{
		return -3;
	}

	unsigned char tempdata[MAX_BUFFER_LEN] = {0};
	memset(tempdata, 0, MAX_BUFFER_LEN);
	memcpy(tempdata, data, datalen);

	unsigned char iv[16] = "123456789abcdef";
	AES_KEY aesKey = {0};
	int keyCrypt = AES_set_encrypt_key(key, keylength*8, &aesKey);
	if(keyCrypt)
	{
		return 0;
	}
	
	AES_cbc_encrypt((unsigned char*)tempdata, (unsigned char*)encryptedData, realDataLen, &aesKey, iv, AES_ENCRYPT);

	return realDataLen;
}

int AESDecrypt(byte* key, int keylength,
			   byte* data, int datalen,
			   byte* decryptedData, int decryptedDataLen)
{
	if(decryptedDataLen > datalen || !decryptedData)
	{
		return -1;
	}
	else if(datalen > MAX_BUFFER_LEN || !data || datalen%AES_BLOCK_SIZE != 0)
	{
		return -2;
	}
	else if(keylength > 32 || !key || keylength%8 != 0)
	{
		return -3;
	}
	else if (datalen == 0)
	{
		return 0;
	}

	unsigned char iv[16] = "123456789abcdef";
	AES_KEY aesKey = {0};
	int keyCrypt = AES_set_decrypt_key(key, keylength*8, &aesKey);
	if(keyCrypt)
	{
		return 0;
	}

	char decryptData[MAX_BUFFER_LEN] = {0};
	AES_cbc_encrypt((unsigned char*)data, (unsigned char*)decryptData, datalen, &aesKey, iv, AES_DECRYPT);
	memcpy(decryptedData, decryptData, decryptedDataLen);

	return decryptedDataLen;
}