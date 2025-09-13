
#include <helpers/icon_extractor.h>
#include <stdio.h>
#include <stdlib.h>

#pragma pack(push, 1)

typedef struct
{
    WORD Reserved;
    WORD Type;
    WORD Count;
} ICONFILEHEAD;

typedef struct
{
    BYTE Cx;
    BYTE Cy;
    BYTE ColorCount;
    BYTE Reserved;
    WORD Planes;
    WORD BitCount;
    DWORD Size;
    DWORD Address;
} ICONFILEINF;

typedef struct
{
    WORD Reserved;
    WORD Type;
    WORD Count;
} ICONRESHEAD;

typedef struct
{
    BYTE Cx;
    BYTE Cy;
    BYTE ColorCount;
    BYTE Reserved;
    WORD Planes;
    WORD BitCount;
    DWORD Size;
    WORD ID;
} ICONRESINF;

#pragma pack(pop)

typedef struct
{
    FILE *outFile;
    BOOL processed;
} EnumCallbackData;

static BOOL WriteFullIconFile(HMODULE hModule, LPTSTR lpszName, FILE *outFile);

static BOOL CALLBACK EnumResNameProcCallback(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
    EnumCallbackData *pData = (EnumCallbackData *)lParam;
    if (WriteFullIconFile(hModule, lpszName, pData->outFile))
    {
        pData->processed = TRUE;
        return FALSE;
    }
    return TRUE;
}

BOOL ExtractFirstIcon(const WCHAR *sourceFile, const WCHAR *outputFile)
{
    HMODULE hModule = LoadLibraryExW(sourceFile, NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (hModule == NULL)
    {

        return FALSE;
    }

    FILE *outFile;
    if (_wfopen_s(&outFile, outputFile, L"wb") != 0 || outFile == NULL)
    {
        FreeLibrary(hModule);
        return FALSE;
    }

    EnumCallbackData callbackData = {outFile, FALSE};

    EnumResourceNamesW(hModule, RT_GROUP_ICON, EnumResNameProcCallback, (LONG_PTR)&callbackData);

    FreeLibrary(hModule);
    fclose(outFile);

    if (!callbackData.processed)
    {
        _wremove(outputFile);
        return FALSE;
    }

    return TRUE;
}

static BOOL WriteFullIconFile(HMODULE hModule, LPTSTR lpszName, FILE *outFile)
{
    HRSRC hResGroup = FindResourceW(hModule, lpszName, RT_GROUP_ICON);
    if (!hResGroup)
        return FALSE;

    HGLOBAL hMemGroup = LoadResource(hModule, hResGroup);
    if (!hMemGroup)
        return FALSE;

    LPVOID pMemGroup = LockResource(hMemGroup);
    if (!pMemGroup)
        return FALSE;

    ICONRESHEAD *pResHead = (ICONRESHEAD *)pMemGroup;
    if (pResHead->Type != 1)
        return FALSE;

    WORD count = pResHead->Count;
    if (count == 0)
        return FALSE;

    ICONRESINF *pResInfos = (ICONRESINF *)((BYTE *)pMemGroup + sizeof(ICONRESHEAD));
    ICONFILEINF *fileInfos = (ICONFILEINF *)calloc(count, sizeof(ICONFILEINF));
    LPVOID *iconDataPtrs = (LPVOID *)calloc(count, sizeof(LPVOID));
    DWORD *iconDataSizes = (DWORD *)calloc(count, sizeof(DWORD));

    if (!fileInfos || !iconDataPtrs || !iconDataSizes)
    {
        free(fileInfos);
        free(iconDataPtrs);
        free(iconDataSizes);
        return FALSE;
    }

    DWORD currentAddress = sizeof(ICONFILEHEAD) + count * sizeof(ICONFILEINF);
    BOOL success = TRUE;

    for (WORD i = 0; i < count; ++i)
    {
        HRSRC hResIcon = FindResourceW(hModule, MAKEINTRESOURCEW(pResInfos[i].ID), RT_ICON);
        if (!hResIcon)
        {
            success = FALSE;
            break;
        }

        HGLOBAL hMemIcon = LoadResource(hModule, hResIcon);
        if (!hMemIcon)
        {
            success = FALSE;
            break;
        }

        DWORD dwSizeIcon = SizeofResource(hModule, hResIcon);
        LPVOID pMemIcon = LockResource(hMemIcon);
        if (!pMemIcon || dwSizeIcon == 0)
        {
            success = FALSE;
            break;
        }

        iconDataPtrs[i] = pMemIcon;
        iconDataSizes[i] = dwSizeIcon;

        fileInfos[i].Cx = pResInfos[i].Cx;
        fileInfos[i].Cy = pResInfos[i].Cy;
        fileInfos[i].ColorCount = pResInfos[i].ColorCount;
        fileInfos[i].Reserved = 0;
        fileInfos[i].Planes = pResInfos[i].Planes;
        fileInfos[i].BitCount = pResInfos[i].BitCount;
        fileInfos[i].Size = dwSizeIcon;
        fileInfos[i].Address = currentAddress;
        currentAddress += dwSizeIcon;
    }

    if (success)
    {
        ICONFILEHEAD fileHead = {0, 1, count};
        fwrite(&fileHead, sizeof(ICONFILEHEAD), 1, outFile);
        fwrite(fileInfos, sizeof(ICONFILEINF), count, outFile);
        for (WORD i = 0; i < count; ++i)
        {
            fwrite(iconDataPtrs[i], 1, iconDataSizes[i], outFile);
        }
    }

    free(fileInfos);
    free(iconDataPtrs);
    free(iconDataSizes);
    return success;
}