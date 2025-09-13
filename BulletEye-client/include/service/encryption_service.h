#ifndef ENCRYPTION_SERVICE_H
#define ENCRYPTION_SERVICE_H


#include <windows.h>

// Define your key and IV as raw bytes
#define AES_KEY_SIZE 16   // 128-bit key
#define AES_BLOCK_SIZE 16

// Example key and IV (must be same for encryption and decryption)
#define AES_KEY "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10"
#define AES_IV  "\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20"

BOOL InitAESKey(HCRYPTPROV *hProv, HCRYPTKEY *hKey, const BYTE *key, DWORD keyLen, const BYTE *iv);
BOOL AESEncrypt(const BYTE *in, DWORD inLen, BYTE **out, DWORD *outLen);
BOOL AESDecrypt(const BYTE *in, DWORD inLen, BYTE **out, DWORD *outLen);

#endif