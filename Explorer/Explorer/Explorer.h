#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <commctrl.h>

#include <WindowsX.h> 
#include <Dbt.h> 
#include "Resource.h"

#include "Drive.h"
#include "TreeView.h"
#include "ListView.h"
#include "Status.h"
#include "ToolBar.h"
#include "DList.h"
#include "Address.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


ATOM MyRegisterClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void OnDestroy(HWND hwnd);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm);
void OnSize(HWND hwnd, UINT state, int cx, int cy);

void DoViewChange(LPNMTOOLBAR lpnmToolBar);
void DoGoForward();
void DoGoBack();
void DoGoUp();
void DoGo();
void DoSizeTreeView();

HINSTANCE g_hInst;
HWND g_hWnd;

CDrive *g_Drive;
TreeView *g_TreeView;
RECT g_TreeViewRect;
CListView *g_ListView;
CStatus *g_Status;
CToolBar *g_ToolBar;
DList *g_History;
CAddress *g_Address;

BOOL g_bStarted = FALSE; 