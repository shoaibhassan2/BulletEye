#ifndef CLIENT_PAGE_H
#define CLIENT_PAGE_H

#include <globals.h>
#include <server/server_logic.h>

void CreateClientPage(HWND parent);
void ResizeClientPageControls(const RECT *prc);

void AddClientToListView(int clientIndex, const ClientInfo *clientInfo);

#endif