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
#include "ServerConnsDlg.hpp"
#include "DDEConvsDlg.hpp"
#include "OptionsDlg.hpp"
#include "TraceOptionsDlg.hpp"
#include "ServicesDlg.hpp"
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
		CMD_ENTRY(ID_VIEW_SERVER_CONNS,	OnViewServerConns,	NULL,				-1)
		CMD_ENTRY(ID_VIEW_DDE_CONVS,	OnViewDDEConvs,		NULL,				-1)
		CMD_ENTRY(ID_VIEW_CLEAR_TRACE,	OnViewClearTrace,	NULL,				-1)
		// Options menu.
		CMD_ENTRY(ID_OPTIONS_GENERAL,	OnOptionsGeneral,	NULL,				-1)
		CMD_ENTRY(ID_OPTIONS_TRACE,		OnOptionsTrace,		NULL,				-1)
		CMD_ENTRY(ID_OPTIONS_SERVICES,	OnOptionsServices,	NULL,				-1)
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
** Method:		OnViewServerConns()
**
** Description:	Show the server connections status.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnViewServerConns()
{
	CServerConnsDlg Dlg;

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
** Method:		OnOptionsServices()
**
** Description:	Configure the services.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnOptionsServices()
{
	int nConns = 0;

	// Count connections...
	for (int i = 0; i < App.m_aoServices.Size(); ++i)
	{
		if (App.m_aoServices[i]->m_oConnection.IsOpen())
			nConns++;
	}

	// Disallow, if any live connections.
	if (nConns > 0)
	{
		App.NotifyMsg("Services cannot be configured while they are being used.\n"
					  "Please close all DDE connections to the NetDDEClient first.");
		return;
	}

	CServicesDlg Dlg;

	// Initialise dialog with current service list.
	for (int i = 0; i < App.m_aoServices.Size(); ++i)
		Dlg.m_aoServices.Add(new CNetDDESvcCfg(App.m_aoServices[i]->m_oCfg));

	// Show config dialog.
	if ( (Dlg.RunModal(App.m_rMainWnd) == IDOK) && (Dlg.m_bModified) )
	{
		// Cleanup all current services.
		for (int i = 0; i < App.m_aoServices.Size(); ++i)
		{
			CNetDDEService* pService = App.m_aoServices[i];

			App.Trace("DDE_STATUS: Unregistering service: %s", pService->m_oCfg.m_strLocName);

			// Unregister the service name.
			App.m_pDDEServer->Unregister(pService->m_oCfg.m_strLocName);
		}

		App.m_aoServices.DeleteAll();

		// Start all new ones.
		for (int i = 0; i < Dlg.m_aoServices.Size(); ++i)
		{
			CNetDDESvcCfg*  pSvcCfg  = Dlg.m_aoServices[i];
			CNetDDEService* pService = new CNetDDEService;

			// Set config.
			pService->m_oCfg = *pSvcCfg;

			try
			{
				App.Trace("DDE_STATUS: Registering service: %s", pService->m_oCfg.m_strLocName);

				// Register the DDE service name.
				App.m_pDDEServer->Register(pService->m_oCfg.m_strLocName);
			}
			catch (CException& e)
			{
				App.AlertMsg("Failed to register DDE service: %s\n\n%s", pService->m_oCfg.m_strLocName, e.ErrorText());
			}

			// Add to collection.
			App.m_aoServices.Add(pService);
		}
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
