#ifndef CLISTVIEW_H
#define CLISTVIEW_H

#include <windows.h>

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include "Drive.h"

class CListView
{
private:
	HINSTANCE	m_hInst;
	HWND		m_hParent;
	HWND		m_hListView;
	int			m_nID;

	void InitImageList();
	LPWSTR	_GetSize(const WIN32_FIND_DATA &fd);
	LPWSTR	_GetType(const WIN32_FIND_DATA &fd);
	LPWSTR	_GetDateModified(const FILETIME &ftLastWrite);
	int		_GetImageListIconIndex(LPCWSTR path, const WIN32_FIND_DATA &fd); 

public:
	CListView();
	~CListView();

	void Create(HWND parentWnd, long ID, HINSTANCE hParentInst, int nWidth,
		int nHeight, int x = CW_USEDEFAULT, int y = 0,
		long lExtStyle = WS_EX_CLIENTEDGE,
		long lStyle = LVS_ICON | LVS_EDITLABELS | LVS_SHOWSELALWAYS);
	void Init4Cols();
	void InitDriveColumn();
	void InitFolderColumn();

	void Size();

	HWND	GetHandle();
	LPCWSTR GetPath(int iItem);
	LPCWSTR GetCurSelPath();
	void	GetFocus();
	LPCWSTR GetCurParentPath();

	void LoadDesktop();
	void LoadMyComputer(CDrive *drive);
	void LoadChild(LPCWSTR path, CDrive *drive);
	void LoadFileAndFolder(LPCWSTR path);
	void LoadCurSel();
	void DisplayInfoCurSel();

	void ChangeViewOption(int nNewStyle);
	void DeleteAll();
	void SelectAll();
	void Up(CDrive *drive);
};

#endif