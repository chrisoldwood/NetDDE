/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPCMDS.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppCmds class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
#include "ClientConnsDlg.hpp"
#include "DDEConvsDlg.hpp"
#include "OptionsDlg.hpp"
#include "TraceOptionsDlg.hpp"
#include "AboutDlg.hpp"

#ifdef _DEBUG
// For memory leak detection.
#define new DBGCRT_NEW
#endif

/******************************************************************************
** Method:		Constructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CAppCmds::CAppCmds()
{
	// Define the command table.
	DEFINE_CMD_TABLE
		// File menu.
		CMD_ENTRY(ID_FILE_EXIT,			OnFileExit,			NULL,				-1)
		// View menu.
		CMD_ENTRY(ID_VIEW_CLIENT_CONNS,	OnViewClientConns,	NULL,				-1)
		CMD_ENTRY(ID_VIEW_DDE_CONVS,	OnViewDDEConvs,		NULL,				-1)
		CMD_ENTRY(ID_VIEW_CLEAR_TRACE,	OnViewClearTrace,	NULL,				-1)
		// Options menu.
		CMD_ENTRY(ID_OPTIONS_GENERAL,	OnOptionsGeneral,	NULL,				-1)
		CMD_ENTRY(ID_OPTIONS_TRACE,		OnOptionsTrace,		NULL,				-1)
		// Help menu.
		CMD_ENTRY(ID_HELP_ABOUT,		OnHelpAbout,		NULL,				10)
	END_CMD_TABLE
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

CAppCmds::~CAppCmds()
{
}

/******************************************************************************
** Method:		OnFileExit()
**
** Description:	Terminate the app.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnFileExit()
{
	App.m_AppWnd.Close();
}

/******************************************************************************
** Method:		OnViewClientConns()
**
** Description:	Show the client connections status.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnViewClientConns()
{
	CClientConnsDlg Dlg;

	Dlg.RunModal(App.m_rMainWnd);
}

/******************************************************************************
** Method:		OnViewDDEConvs()
**
** Description:	Show the DDE conversations.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnViewDDEConvs()
{
	CDDEConvsDlg Dlg;

	Dlg.RunModal(App.m_rMainWnd);
}

/******************************************************************************
** Method:		OnViewClearTrace()
**
** Description:	Clear the trace window.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnViewClearTrace()
{
	App.m_AppWnd.m_AppDlg.Clear();
}

/******************************************************************************
** Method:		OnOptionsGeneral()
**
** Description:	Configure the general options.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnOptionsGeneral()
{
	COptionsDlg Dlg;

	if (Dlg.RunModal(App.m_rMainWnd) == IDOK)
	{
		// Add tray icon?
		if ( (App.m_bTrayIcon) && (!App.m_AppWnd.m_oTrayIcon.IsVisible()) )
			App.m_AppWnd.ShowTrayIcon(true);
		
		// Remove tray icon?
		if ( (!App.m_bTrayIcon) && (App.m_AppWnd.m_oTrayIcon.IsVisible()) )
			App.m_AppWnd.ShowTrayIcon(false);
	}
}

/******************************************************************************
** Method:		OnOptionsTrace()
**
** Description:	Configure the trace options.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnOptionsTrace()
{
	CTraceOptionsDlg Dlg;

	Dlg.RunModal(App.m_rMainWnd);
}

/******************************************************************************
** Method:		OnHelpAbout()
**
** Description:	Show the about dialog.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnHelpAbout()
{
	CAboutDlg Dlg;

	Dlg.RunModal(App.m_rMainWnd);
}

/******************************************************************************
** Method:		OnUI...()
**
** Description:	UI update handlers.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/
