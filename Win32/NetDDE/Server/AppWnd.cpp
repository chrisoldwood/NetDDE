/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPWND.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppWnd class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"

#ifdef _DEBUG
// For memory leak detection.
#define new DBGCRT_NEW
#endif

/******************************************************************************
** Method:		Default constructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CAppWnd::CAppWnd()
	: CDlgFrame(IDR_APPICON, m_AppDlg, false)
{
}

/******************************************************************************
** Method:		Destructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CAppWnd::~CAppWnd()
{
}

/******************************************************************************
** Method:		OnCreate()
**
** Description:	Creates the toolbar and status bars.
**
** Parameters:	rcClient	The client rectangle.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::OnCreate(const CRect& rcClient)
{
	//
	// Create and attach the components.
	//
	m_Accel.LoadRsc(IDR_APPACCEL);
	Accel(&m_Accel);

	m_Menu.LoadRsc(IDR_APPMENU);
	Menu(&m_Menu);

	m_StatusBar.Create(*this, IDC_STATUS_BAR, rcClient);
	StatusBar(&m_StatusBar);

	m_AppDlg.RunModeless(*this);

	// Show the tray icon?
	if (App.m_bTrayIcon)
		ShowTrayIcon(true);

	// Call base class.
	CDlgFrame::OnCreate(rcClient);
}

/******************************************************************************
** Method:		OnUserMsg()
**
** Description:	Callback message from the system tray.
**				
** Parameters:	As WindowProc.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::OnUserMsg(uint /*nMsg*/, WPARAM /*wParam*/, LPARAM lParam)
{
//	ASSERT(nMsg   == WM_USER_TRAY_NOTIFY);
//	ASSERT(wParam == TRAY_ICON_ID);

	// Icon double-clicked with left button?
	if (lParam == WM_LBUTTONDBLCLK)
	{
		// Restore window, if minimsed.
		if (!::IsWindowVisible(m_hWnd))
			Show(SW_SHOWNORMAL);
	}
	// Icon clicked with right button?
	else if ( (lParam == WM_RBUTTONUP) || (lParam == WM_CONTEXTMENU) )
	{
		// Show context menu.
	}
}

/******************************************************************************
** Method:		OnResize()
**
** Description:	Window has been resized.
**
** Parameters:	iFlag		The resize method.
**				rNewSize	The new window dimensions.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::OnResize(int iFlag, const CSize& rNewSize)
{
	// Window minimised AND "Minimise To Tray" set?
	if ( (iFlag == SIZE_MINIMIZED) && (App.m_bMinToTray) )
		Show(SW_HIDE);

	CDlgFrame::OnResize(iFlag, rNewSize);
}

/******************************************************************************
** Method:		OnFocus()
**
** Description:	Divert the focus to the active view window.
**				
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::OnFocus()
{
	App.m_AppWnd.m_AppDlg.Focus();
}

/******************************************************************************
** Method:		OnQueryClose()
**
** Description:	Check if the app can close.
**
** Parameters:	None.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CAppWnd::OnQueryClose()
{
	// Remove the tray icon.
	m_oTrayIcon.Remove();

	// Fetch windows final placement.
	App.m_rcLastPos = Placement();

	return true;
}
