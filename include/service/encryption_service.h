#ifndef ENCRYPTION_SERVICE_H
#define ENCRYPTION_SERVICE_H

#include <windows.h>

#define AES_KEY_SIZE 16
#define AES_BLOCK_SIZE 16

#define AES_KEY "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10"
#define AES_IV "\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20"

BOOL AESEncrypt(const BYTE *in, DWORD inLen, BYTE **out, DWORD *outLen);
BOOL AESDecrypt(const BYTE *in, DWORD inLen, BYTE **out, DWORD *outLen);

#endif