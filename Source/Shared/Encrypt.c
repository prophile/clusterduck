#include "Encrypt.h"
#include <openssl/evp.h>
#include <stdlib.h>
#include <stdio.h>

static int _key_loaded = 0;

static unsigned char _key_iv[16];
static unsigned char _key_key[32];

static void load_key_if_needed ()
{
	if (!_key_loaded)
	{
		FILE* fp = fopen("/etc/cduck.key", "r");
		if (!fp)
		{
			fprintf(stderr, "Failed to load /etc/cduck.key\n");
			exit(1);
		}
		if (fread(_key_key, sizeof(_key_key), 1, fp) != 1)
		{
			fprintf(stderr, "Failed to load key from /etc/cduck.key\n");
			exit(1);
		}
		if (fread(_key_iv, sizeof(_key_iv), 1, fp) != 1)
		{
			fprintf(stderr, "Failed to load IV from /etc/cduck.key\n");
			exit(1);
		}
		if (fclose(fp))
		{
			fprintf(stderr, "Failed to close /etc/cduck.key\n");
			exit(1);
		}
		_key_loaded = 1;
	}
}

void* cduck_encrypt ( const void* data, unsigned int inLen, unsigned int* outLen )
{
	unsigned int outBufferInitialSize = (inLen < 128) ? 256 : inLen * 2;
	unsigned char* outBuffer = malloc(outBufferInitialSize);
	int outBufferLen, outBufferTempLen;
	int inBufferLen = (int)inLen;
	EVP_CIPHER_CTX context;
	load_key_if_needed();
	EVP_CIPHER_CTX_init(&context);
	EVP_EncryptInit_ex(&context, EVP_aes_256_cbc(), NULL, _key_key, _key_iv);
	EVP_EncryptUpdate(&context, outBuffer, &outBufferLen, (unsigned char*)data, inBufferLen);
	EVP_EncryptFinal_ex(&context, outBuffer + outBufferLen, &outBufferTempLen);
	outBufferLen += outBufferTempLen;
	*outLen = (unsigned int)outBufferLen;
	return realloc(outBuffer, outBufferLen);
}

void* cduck_decrypt ( const void* data, unsigned int inLen, unsigned int* outLen )
{
	unsigned int outBufferInitialSize = (inLen < 128) ? 192 : inLen;
	unsigned char* outBuffer = malloc(outBufferInitialSize);
	int outBufferLen, outBufferTempLen;
	int inBufferLen = (int)inLen;
	EVP_CIPHER_CTX context;
	load_key_if_needed();
	EVP_CIPHER_CTX_init(&context);
	EVP_DecryptInit_ex(&context, EVP_aes_256_cbc(), NULL, _key_key, _key_iv);
	EVP_DecryptUpdate(&context, outBuffer, &outBufferLen, (unsigned char*)data, inBufferLen);
	EVP_DecryptFinal_ex(&context, outBuffer + outBufferLen, &outBufferTempLen);
	outBufferLen += outBufferTempLen;
	*outLen = (unsigned int)outBufferLen;
	return realloc(outBuffer, outBufferLen);
}

