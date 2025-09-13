#ifndef FE_REMOTE_OPERATIONS_H
#define FE_REMOTE_OPERATIONS_H

#include <gui/file_explorer/fe_main_window.h>
#include <resource.h>
#include <service/network_service.h>

void RequestDrives(FileExplorerData *pData);

void RequestDirectoryListing(FileExplorerData *pData, LPCWSTR pszPath);

void RequestTreeChildren(FileExplorerData *pData, HTREEITEM hParentItem);

void RequestFileExecution(FileExplorerData *pData, LPCWSTR pszPath);

void RequestFileDeletion(FileExplorerData *pData, LPCWSTR pszPath);

void RequestFileRename(FileExplorerData *pData, LPCWSTR pszOldPath, LPCWSTR pszNewPath);

void RequestFileUpload(FileExplorerData *pData, LPCWSTR pszLocalPath, LPCWSTR pszRemoteDestDir);

void RequestFileDownload(FileExplorerData *pData, LPCWSTR pszRemotePath, LPCWSTR pszLocalDestPath);

void RequestFileEncryption(FileExplorerData *pData, LPCWSTR pszPath);

void RequestFileDecryption(FileExplorerData *pData, LPCWSTR pszPath);

#endif