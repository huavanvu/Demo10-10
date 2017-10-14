#include "TreeView.h"
#include "Extractor.h"
#include "Resource.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

//Vị trí icon trong file Shell32.dll
#define IDI_SHELL32_FOLDER 3
#define IDI_SHELL32_FOLDER_SELECTED 4 
#define IDI_SHELL32_DESKTOP 34
#define IDI_SHELL32_MYCOMPUTER 15

#define IDI_SHELL32_FLOPPY 6
#define IDI_SHELL32_USB 7
#define IDI_SHELL32_HDD 8
#define IDI_SHELL32_CD  11 

#define INITIAL_ICON_IN_TREE 8 
#define NUMBER_OF_ICON_TO_GROW 0 
#define IDI_FOLDER 0
#define IDI_FOLDER_SELECTED 1
#define IDI_DESKTOP 2
#define IDI_MYCOMPUTER 3

#define IDI_FLOPPY 4
#define IDI_USB 5
#define IDI_HDD 6
#define IDI_CD  7 

//Độ dài tối đa đường dẫn
#define MAX_PATH_LEN 10240
/*****************************************************************************************/

TreeView::TreeView()
{
	m_hInst = NULL;
	m_hParent = NULL;
	m_hTreeView = NULL;
	m_nID = 0;
}

TreeView::~TreeView()
{
	DestroyWindow(m_hTreeView);
}

void TreeView::Create(HWND parentWnd, long ID, HINSTANCE hParentInst, int nWidth, int nHeight,
	long lExtStyle, long lStyle, int x, int y)
{
	InitCommonControls();
	TreeView::m_hParent = parentWnd;
	TreeView::m_hInst = hParentInst;
	TreeView::m_hTreeView = CreateWindowEx(lExtStyle, WC_TREEVIEW, _T("Tree View"),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_SIZEBOX | WS_VSCROLL | WS_TABSTOP | lStyle,
		x, y, nWidth, nHeight, parentWnd,
		(HMENU)ID, hParentInst, NULL);
	TreeView::m_nID = ID;

	GetImageList();
}
/*****************************************************************************************/

void TreeView::GetImageList()
{
	HIMAGELIST *hIml = new HIMAGELIST;

	*hIml = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK,
		INITIAL_ICON_IN_TREE, NUMBER_OF_ICON_TO_GROW);

	HICON hIcon;
	CExtractor::Get(_T("%SystemRoot%\\System32\\Shell32.dll"), IDI_SHELL32_FOLDER, 16, 16,
		&hIcon, NULL, 1, LR_COLOR);
	ImageList_AddIcon(*hIml, hIcon);

	CExtractor::Get(_T("%SystemRoot%\\System32\\Shell32.dll"), IDI_SHELL32_FOLDER_SELECTED, 16, 16,
		&hIcon, NULL, 1, LR_COLOR);
	ImageList_AddIcon(*hIml, hIcon);

	CExtractor::Get(_T("%SystemRoot%\\System32\\Shell32.dll"), IDI_SHELL32_DESKTOP, 16, 16,
		&hIcon, NULL, 1, LR_COLOR);
	ImageList_AddIcon(*hIml, hIcon);

	CExtractor::Get(_T("%SystemRoot%\\System32\\Shell32.dll"), IDI_SHELL32_MYCOMPUTER, 16, 16,
		&hIcon, NULL, 1, LR_COLOR);
	ImageList_AddIcon(*hIml, hIcon);

	CExtractor::Get(_T("%SystemRoot%\\System32\\Shell32.dll"), IDI_SHELL32_FLOPPY, 16, 16,
		&hIcon, NULL, 1, LR_COLOR);
	ImageList_AddIcon(*hIml, hIcon);

	CExtractor::Get(_T("%SystemRoot%\\System32\\Shell32.dll"), IDI_SHELL32_USB, 16, 16,
		&hIcon, NULL, 1, LR_COLOR);
	ImageList_AddIcon(*hIml, hIcon);

	CExtractor::Get(_T("%SystemRoot%\\System32\\Shell32.dll"), IDI_SHELL32_HDD, 16, 16,
		&hIcon, NULL, 1, LR_COLOR);
	ImageList_AddIcon(*hIml, hIcon);

	CExtractor::Get(_T("%SystemRoot%\\System32\\Shell32.dll"), IDI_SHELL32_CD, 16, 16,
		&hIcon, NULL, 1, LR_COLOR);
	ImageList_AddIcon(*hIml, hIcon);

	TreeView_SetImageList(m_hTreeView, *hIml, TVSIL_NORMAL);
}

HWND TreeView::GetHandle()
{
	return m_hTreeView;
}

int TreeView::GetID()
{
	return m_nID;
}

LPCWSTR TreeView::GetPath(HTREEITEM hItem)
{
	TVITEMEX tv;
	tv.mask = TVIF_PARAM;
	tv.hItem = hItem;
	TreeView_GetItem(m_hTreeView, &tv);
	return (LPCWSTR)tv.lParam;
}

LPCWSTR TreeView::GetCurPath()
{
	return GetPath(GetCurSel());
}

HTREEITEM TreeView::GetCurSel()
{
	return TreeView_GetNextItem(m_hTreeView, NULL, TVGN_CARET);
}

LPCWSTR	TreeView::GetCurSelText()
{
	TVITEMEX tv;
	TCHAR *buffer = new TCHAR[256];

	tv.mask = TVIF_TEXT;
	tv.hItem = GetCurSel();
	tv.pszText = buffer;
	tv.cchTextMax = 256;
	TreeView_GetItem(m_hTreeView, &tv);
	return (LPCWSTR)tv.pszText;
}
HTREEITEM TreeView::GetChild(HTREEITEM hItem)
{
	return TreeView_GetChild(m_hTreeView, hItem);
}

void TreeView::GetFocus()
{
	SetFocus(m_hTreeView);
}
HTREEITEM TreeView::GetDesktop()
{
	return TreeView_GetRoot(m_hTreeView);
}

HTREEITEM TreeView::GetMyComputer()
{
	return TreeView_GetChild(m_hTreeView, GetDesktop());
}


void TreeView::LoadMyComputer(CDrive *drive)
{
	TV_INSERTSTRUCT tvInsert;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;


	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = TVI_ROOT;
	tvInsert.item.iImage = IDI_DESKTOP;
	tvInsert.item.iSelectedImage = IDI_DESKTOP;
	tvInsert.item.pszText = _T("Desktop");
	tvInsert.item.lParam = (LPARAM)_T("Desktop");
	HTREEITEM hDesktop = TreeView_InsertItem(m_hTreeView, &tvInsert);

	tvInsert.hParent = hDesktop;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.iImage = IDI_MYCOMPUTER;
	tvInsert.item.iSelectedImage = IDI_MYCOMPUTER;
	tvInsert.item.pszText = _T("My Computer");
	tvInsert.item.lParam = (LPARAM)_T("My Computer");
	HTREEITEM hMyComputer = TreeView_InsertItem(m_hTreeView, &tvInsert);

	for (int i = 0; i < drive->GetCount(); ++i)
	{
		tvInsert.hParent = hMyComputer; 
		tvInsert.item.iImage = drive->GetIconIndex(i);
		tvInsert.item.iSelectedImage = drive->GetIconIndex(i);
		tvInsert.item.pszText = drive->GetDisplayName(i);
		tvInsert.item.lParam = (LPARAM)drive->GetDriveName(i);
		HTREEITEM hDrive = TreeView_InsertItem(m_hTreeView, &tvInsert);

		if ((drive->GetIconIndex(i) == IDI_HDD) || (drive->GetIconIndex(i) == IDI_USB))
		{
			tvInsert.hParent = hDrive; 
			tvInsert.item.pszText = _T("PreLoad"); 
			tvInsert.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(m_hTreeView, &tvInsert);
		}
	}
	TreeView_Expand(m_hTreeView, hMyComputer, TVE_EXPAND);
	TreeView_SelectItem(m_hTreeView, hMyComputer);
}


void TreeView::LoadChild(HTREEITEM &hParent, LPCWSTR path, BOOL bShowHiddenSystem)
{
	TCHAR buffer[MAX_PATH_LEN];
	StrCpy(buffer, path);

	StrCat(buffer, _T("\\*"));

	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvInsert.item.iImage = IDI_FOLDER;
	tvInsert.item.iSelectedImage = IDI_FOLDER_SELECTED;

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);
	BOOL bFound = 1;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	TCHAR * folderPath;
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM)
			&& (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			tvInsert.item.pszText = fd.cFileName;
			folderPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];

			StrCpy(folderPath, path);
			if (wcslen(path) != 3)
				StrCat(folderPath, _T("\\"));
			StrCat(folderPath, fd.cFileName);

			tvInsert.item.lParam = (LPARAM)folderPath;
			HTREEITEM hItem = TreeView_InsertItem(m_hTreeView, &tvInsert);
		
			PreLoad(hItem);
		}

		bFound = FindNextFileW(hFile, &fd);
	}
}

void TreeView::PreloadExpanding(HTREEITEM hPrev, HTREEITEM hCurSel)
{
	if (hCurSel == GetMyComputer()) 
		return;

	HTREEITEM hCurSelChild = TreeView_GetChild(m_hTreeView, hCurSel);

	if (!StrCmp(GetPath(hCurSelChild), _T("PreLoad")))
	{
		TreeView_DeleteItem(m_hTreeView, hCurSelChild);
		LoadChild(hCurSel, GetPath(hCurSel));
	}
}

void TreeView::PreLoad(HTREEITEM hItem)
{
	TCHAR buffer[MAX_PATH_LEN];
	StrCpy(buffer, GetPath(hItem));

	if (wcslen(buffer) == 3) 
	{
		if (StrStr(buffer, _T("A:")) || StrStr(buffer, _T("B:"))) 
			return;
	}
	else
		StrCat(buffer, _T("\\"));

	StrCat(buffer, _T("*"));

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	BOOL bFound = true;
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			TV_INSERTSTRUCT tvInsert;
			tvInsert.hParent = hItem;
			tvInsert.hInsertAfter = TVI_LAST;
			tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
			tvInsert.item.pszText = NULL;
			tvInsert.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(m_hTreeView, &tvInsert);
			bFound = FALSE;
		}
		else
			bFound = FindNextFileW(hFile, &fd);
	}
}

void TreeView::Expand(HTREEITEM hItem)
{
	TreeView_Expand(m_hTreeView, hItem, TVE_EXPAND);
}

void TreeView::Size(int cy)
{
	RECT treeRC;
	GetWindowRect(m_hTreeView, &treeRC);

	MoveWindow(m_hTreeView, 0, 75, treeRC.right - treeRC.left, cy - 97, SWP_SHOWWINDOW);
}