#include <service/encryption_service.h>
#include <wincrypt.h>

// Helper to init AES key/IV
BOOL InitAESKey(HCRYPTPROV *hProv, HCRYPTKEY *hKey, const BYTE *key, DWORD keyLen, const BYTE *iv) {
    HCRYPTHASH hHash = 0;

    if (!CryptAcquireContext(hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        return FALSE;
    }
    if (!CryptCreateHash(*hProv, CALG_SHA_256, 0, 0, &hHash)) {
        CryptReleaseContext(*hProv, 0);
        return FALSE;
    }
    if (!CryptHashData(hHash, key, keyLen, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(*hProv, 0);
        return FALSE;
    }
    if (!CryptDeriveKey(*hProv, CALG_AES_128, hHash, CRYPT_EXPORTABLE, hKey)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(*hProv, 0);
        return FALSE;
    }
    CryptDestroyHash(hHash);

    // Set IV
    if (!CryptSetKeyParam(*hKey, KP_IV, iv, 0)) {
        CryptDestroyKey(*hKey);
        CryptReleaseContext(*hProv, 0);
        return FALSE;
    }
    return TRUE;
}

// Encrypt function
BOOL AESEncrypt(const BYTE *in, DWORD inLen, BYTE **out, DWORD *outLen) {
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;

    if (!InitAESKey(&hProv, &hKey, (const BYTE*)AES_KEY, AES_KEY_SIZE, (const BYTE*)AES_IV))
        return FALSE;

    *outLen = inLen;
    DWORD bufLen = inLen + AES_BLOCK_SIZE; // allow padding
    *out = (BYTE*)malloc(bufLen);
    memcpy(*out, in, inLen);

    if (!CryptEncrypt(hKey, 0, TRUE, 0, *out, outLen, bufLen)) {
        free(*out);
        CryptDestroyKey(hKey);
        CryptReleaseContext(hProv, 0);
        return FALSE;
    }

    CryptDestroyKey(hKey);
    CryptReleaseContext(hProv, 0);
    return TRUE;
}

// Decrypt function
BOOL AESDecrypt(const BYTE *in, DWORD inLen, BYTE **out, DWORD *outLen) {
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;

    if (!InitAESKey(&hProv, &hKey, (const BYTE*)AES_KEY, AES_KEY_SIZE, (const BYTE*)AES_IV))
        return FALSE;

    *outLen = inLen;
    *out = (BYTE*)malloc(inLen);
    memcpy(*out, in, inLen);

    if (!CryptDecrypt(hKey, 0, TRUE, 0, *out, outLen)) {
        free(*out);
        CryptDestroyKey(hKey);
        CryptReleaseContext(hProv, 0);
        return FALSE;
    }

    CryptDestroyKey(hKey);
    CryptReleaseContext(hProv, 0);
    return TRUE;
}