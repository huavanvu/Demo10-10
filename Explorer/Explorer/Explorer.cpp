#include "Explorer.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
		return false;

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
		if (!IsDialogMessage(g_hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}//if

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX	wc;

	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = _T("MyApp");
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.style = CS_HREDRAW | CS_VREDRAW;

	return RegisterClassEx(&wc);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance;
	g_hWnd = CreateWindowEx(0, _T("MyApp"), _T("My Explorer"),
		WS_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_MAXIMIZE,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		NULL, NULL, hInstance, NULL);

	if (!g_hWnd)
		return false;

	ShowWindow(g_hWnd, SW_MAXIMIZE);
	UpdateWindow(g_hWnd);

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
		HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	RECT main;
	GetWindowRect(hwnd, &main);

	g_Address = new CAddress;
	g_Address->Create(hwnd, IDC_ADDRESS, g_hInst);
	g_ToolBar = new CToolBar;
	g_ToolBar->Create(hwnd, IDC_TOOLBAR, g_hInst, 0, 0, 0, 0);
	g_ToolBar->EnableBack(FALSE);
	g_ToolBar->EnableForward(FALSE);

	g_Status = new CStatus;
	g_Status->Create(hwnd, IDC_STATUSBAR, g_hInst);

	g_History = new DList;

	g_Drive = new CDrive;
	g_Drive->GetSystemDrives();

	g_TreeView = new TreeView;
	g_TreeView->Create(hwnd, IDC_TREEVIEW, g_hInst, main.right / 3, main.bottom);
	g_TreeView->LoadMyComputer(g_Drive);
	g_TreeView->GetFocus();

	g_ListView = new CListView;
	g_ListView->Create(hwnd, IDC_LISTVIEW, g_hInst, (main.right - main.left) * 2 / 3 + 1, main.bottom, main.right / 3);
	g_ListView->LoadMyComputer(g_Drive);
	g_History->InsertAfterCur(g_ListView->GetCurParentPath());
	g_bStarted = TRUE;

	return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDM_FILE_EXIT:
		DestroyWindow(hwnd);
		break;

	case ID_FILE_SELECTALL:
		g_ListView->SelectAll();
		break;

	case IDM_HELP_ABOUT:
		MessageBox(NULL, _T("Explorer"), _T("About"), MB_OK);
		break;
	case ID_VIEW_ICONS:
		g_ListView->ChangeViewOption(LVS_ICON);
		break;

	case ID_VIEW_SMALLICONS:
		g_ListView->ChangeViewOption(LVS_SMALLICON);
		break;

	case ID_VIEW_LIST1:
		g_ListView->ChangeViewOption(LVS_LIST);
		break;

	case ID_VIEW_DETAILS1:
		g_ListView->ChangeViewOption(LVS_REPORT);
		break;
	}
}

void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	g_TreeView->Size(cy);
	GetWindowRect(g_TreeView->GetHandle(), &g_TreeViewRect); 

	g_ListView->Size();
	g_Status->Size();

	SendMessage(g_ToolBar->GetHandle(), TB_AUTOSIZE, 0, (LPARAM)0);

	SendMessage(g_Address->GetHandle(), TB_AUTOSIZE, 0, (LPARAM)0);
	g_Address->Size();
}

LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm)
{
	int nCurSelIndex;
	LPNMTOOLBAR lpnmToolBar = (LPNMTOOLBAR)pnm;

	if (g_bStarted) 
	{
		LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)pnm;

		switch (pnm->code)
		{
		case TVN_ITEMEXPANDING:
			g_TreeView->PreloadExpanding(lpnmTree->itemOld.hItem, lpnmTree->itemNew.hItem);
			break;
			//------------------------------------------------------------------------------
		case TVN_SELCHANGED:
			g_TreeView->Expand(g_TreeView->GetCurSel());
			g_ListView->DeleteAll(); 
			g_ListView->LoadChild(g_TreeView->GetCurPath(), g_Drive);
			g_History->InsertAfterCur(g_TreeView->GetCurPath());

			if (g_History->GetCur()->GetPrev() != NULL)
			{
				g_ToolBar->EnableBack(TRUE);
			}
			break;
			//------------------------------------------------------------------------------
		case NM_CLICK:
			if (pnm->hwndFrom == g_ListView->GetHandle())
			{
				nCurSelIndex = ListView_GetNextItem(g_ListView->GetHandle(), -1, LVNI_FOCUSED);
				if (nCurSelIndex != -1)
					g_ListView->DisplayInfoCurSel();

			}
			else
				switch (pnm->idFrom)
				{
				case IDC_TOOLBAR:
					switch (lpnmToolBar->iItem)
					{
					case IDC_TOOLBAR_UP:
						DoGoUp();
						break;
					case IDC_TOOLBAR_BACK:
						DoGoBack();
						break;
					case IDC_TOOLBAR_FORWARD:
						DoGoForward();
						break;
					}
					break; 
				case IDC_ADDRESS:
					if (lpnmToolBar->iItem == IDC_ADDRESS_GO)
						DoGo();
					break;
				}
			break; 
		case NM_DBLCLK:
			if (pnm->hwndFrom == g_ListView->GetHandle())
				g_ListView->LoadCurSel();
			break;
		case NM_CUSTOMDRAW: 
			if (pnm->hwndFrom == g_TreeView->GetHandle())
				DoSizeTreeView();
			break;
		case TBN_DROPDOWN:
			if (lpnmToolBar->iItem == IDC_TOOLBAR_VIEW)
				DoViewChange(lpnmToolBar);
			break;
		}
	}
	return 0;
}


void DoViewChange(LPNMTOOLBAR lpnmToolBar)
{
	HMENU     hPopupMenu = NULL;
	HMENU     hMenuLoaded;
	RECT rc;
	TPMPARAMS tpm;

	SendMessage(lpnmToolBar->hdr.hwndFrom, TB_GETRECT,
		(WPARAM)lpnmToolBar->iItem, (LPARAM)&rc);
	MapWindowPoints(lpnmToolBar->hdr.hwndFrom,
		HWND_DESKTOP, (LPPOINT)&rc, 2);
	tpm.cbSize = sizeof(TPMPARAMS);
	tpm.rcExclude = rc;

	hMenuLoaded = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_MENU2));

	hPopupMenu = GetSubMenu(hMenuLoaded, 0);

	TrackPopupMenuEx(hPopupMenu,
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
		rc.left, rc.bottom, g_ToolBar->GetHandle(), &tpm);

	DestroyMenu(hMenuLoaded);
}
void DoGoForward()
{
	g_History->GoForward();
	g_ToolBar->EnableBack(TRUE);

	if (g_History->GetCur()->GetNext() == NULL)
		g_ToolBar->EnableForward(FALSE);

	g_ListView->LoadChild(g_History->GetCur()->GetPath(), g_Drive);
}

void DoGoBack()
{
	g_History->GoBack();
	g_ToolBar->EnableForward(TRUE);

	if (g_History->GetCur()->GetPrev() == NULL)
		g_ToolBar->EnableBack(FALSE);

	g_ListView->LoadChild(g_History->GetCur()->GetPath(), g_Drive);
}
void DoGoUp()
{
	g_ListView->Up(g_Drive);
	g_History->InsertAfterCur(g_ListView->GetCurParentPath());
	g_ToolBar->EnableBack(TRUE);
}
void DoGo()
{
	TCHAR *buffer = new TCHAR[10240];
	GetDlgItemText(g_Address->GetHandle(), IDC_ADDRESS_EDIT, buffer, 10240);
	g_ListView->LoadChild(buffer, g_Drive);
}
void DoSizeTreeView()
{
	RECT newTreeRC;
	GetClientRect(g_TreeView->GetHandle(), &newTreeRC);

	if (newTreeRC.right != g_TreeViewRect.right)
	{
		g_ListView->Size();
		g_TreeViewRect = newTreeRC;
	}
}