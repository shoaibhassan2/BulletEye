#include <globals.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <gui/log_page.h>
#include <gui/log_handler.h>

void CopyLogToClipboard(HWND hListView)
{
    int itemCount = ListView_GetItemCount(hListView);
    if (itemCount == 0)
        return;

    size_t estimatedTotalLen = (size_t)itemCount * (80 + 1024 + 4);

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, estimatedTotalLen * sizeof(WCHAR));
    if (!hMem)
    {
        AddLogMessage(L"Failed to allocate memory for clipboard.");
        return;
    }

    WCHAR *pMem = (WCHAR *)GlobalLock(hMem);
    if (!pMem)
    {
        GlobalFree(hMem);
        AddLogMessage(L"Failed to lock memory for clipboard.");
        return;
    }

    pMem[0] = L'\0';
    WCHAR timeBuf[32];
    WCHAR msgBuf[1024];

    size_t currentOffset = 0;
    for (int i = 0; i < itemCount; i++)
    {
        ListView_GetItemText(hListView, i, 0, timeBuf, _countof(timeBuf));
        ListView_GetItemText(hListView, i, 1, msgBuf, _countof(msgBuf));

        if (currentOffset + wcslen(timeBuf) + wcslen(msgBuf) + 6 > estimatedTotalLen)
        {
            AddLogMessage(L"Clipboard buffer too small, truncating log copy.");
            break;
        }

        currentOffset += wsprintfW(pMem + currentOffset, L"[%s] %s\r\n", timeBuf, msgBuf);
    }

    GlobalUnlock(hMem);

    if (OpenClipboard(g_hWnd))
    {
        EmptyClipboard();
        SetClipboardData(CF_UNICODETEXT, hMem);
        CloseClipboard();
        AddLogMessage(L"Log copied to clipboard.");
    }
    else
    {
        GlobalFree(hMem);
        AddLogMessage(L"Failed to open clipboard.");
    }
}

void SaveLogToFile(HWND hListView)
{
    WCHAR szFileName[MAX_PATH] = L"";
    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"txt";

    if (GetSaveFileNameW(&ofn))
    {
        FILE *pFile;
        errno_t err = _wfopen_s(&pFile, szFileName, L"w, ccs=UTF-8");
        if (err == 0 && pFile)
        {
            int itemCount = ListView_GetItemCount(hListView);
            WCHAR timeBuf[32];
            WCHAR msgBuf[1024];
            for (int i = 0; i < itemCount; i++)
            {
                ListView_GetItemText(hListView, i, 0, timeBuf, _countof(timeBuf));
                ListView_GetItemText(hListView, i, 1, msgBuf, _countof(msgBuf));
                fwprintf(pFile, L"[%s] %s\r\n", timeBuf, msgBuf);
            }
            fclose(pFile);

            WCHAR logMsg[MAX_PATH + 64];
            wsprintfW(logMsg, L"Log successfully saved to: %s", szFileName);
            AddLogMessage(logMsg);
        }
        else
        {
            WCHAR errBuf[256];
            _wcserror_s(errBuf, _countof(errBuf), err);
            WCHAR logMsg[MAX_PATH + 128];
            wsprintfW(logMsg, L"Failed to open file for writing: %s (Error: %s)", szFileName, errBuf);
            AddLogMessage(logMsg);
        }
    }
    else
    {
        AddLogMessage(L"Save file operation was cancelled by the user.");
    }
}