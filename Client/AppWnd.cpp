/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPWND.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppWnd class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "AppWnd.hpp"
#include "NetDDECltApp.hpp"
#include <WCL/Cursor.hpp>
#include <NCL/DDEServer.hpp>

/******************************************************************************
**
** Constants.
**
*******************************************************************************
*/

const tchar* CAppWnd::WNDCLASS_NAME = TXT("NetDDEClient");

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
** Method:		GetClassParams()
**
** Description:	Template method to get the window class data.
**
** Parameters:	rParams		The class structure to fill.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::GetClassParams(WNDCLASS& rParams)
{
	// Get base class settings.
	CDlgFrame::GetClassParams(rParams);

	// Override any settings.
	rParams.lpszClassName = WNDCLASS_NAME;
}

/******************************************************************************
** Method:		GetCreateParams()
**
** Description:	Template method to get the window creation data.
**
** Parameters:	rParams		The create structure to fill.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::GetCreateParams(WNDCREATE& rParams)
{
	// Get base class settings.
	CDlgFrame::GetCreateParams(rParams);

	// Override any settings.
	rParams.pszClassName = WNDCLASS_NAME;
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
		Restore();
	}
	// Icon clicked with right button?
	else if ( (lParam == WM_RBUTTONUP) || (lParam == WM_CONTEXTMENU) )
	{
		// Required to allow menu to be dismissed.
		::SetForegroundWindow(m_hWnd);

		CPopupMenu oMenu(IDR_TRAYMENU);

		// Disable relevant commands.
		oMenu.EnableCmd(ID_TRAY_RESTORE, (IsMinimised() || !IsVisible()));

		// Show context menu.
		uint nCmdID = oMenu.TrackMenu(*this, CCursor::CurrentPos());

		if (nCmdID != 0)
		{
			// Handle command.
			switch (nCmdID)
			{
				case ID_TRAY_RESTORE:	Restore();				break;
				case ID_TRAY_EXIT:		Close();				break;
				default:				ASSERT_FALSE();			break;
			}
		}

		// Required to allow menu to be dismissed.
		PostMessage(WM_NULL);
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
	int nConvs = App.m_pDDEServer->GetNumConversations();

	// Warn user if still in use.
	if (nConvs > 0)
	{
		CString strMsg;

		strMsg.Format(TXT("There are %d client(s) still connected.\n\nAre you sure you want to close it?"), nConvs);

		// Abort if NO or CANCEL.
		if (QueryMsg(strMsg) != IDYES)
			return false;
	}

	return true;
}

/******************************************************************************
** Method:		OnClose()
**
** Description:	The main window is closing.
**				NB: Called from WM_CLOSE or WM_ENDSESSION.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::OnClose()
{
	// Remove the tray icon.
	if (m_oTrayIcon.IsVisible())
		ShowTrayIcon(false);

	// Fetch windows final placement.
	App.m_rcLastPos = Placement();
}

/******************************************************************************
** Method:		Restore()
**
** Description:	Restore the window from the system tray or taskbar.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::Restore()
{
	// Not already restored?
	if (!IsVisible() || IsMinimised())
	{
		Show(SW_SHOWNORMAL);
		::SetForegroundWindow(m_hWnd);
	}
}
