/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPCMDS.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppCmds class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "AppCmds.hpp"
#include "NetDDECltApp.hpp"
#include "ServerConnsDlg.hpp"
#include "DDEConvsDlg.hpp"
#include "OptionsDlg.hpp"
#include "TraceOptionsDlg.hpp"
#include "ServicesDlg.hpp"
#include "AboutDlg.hpp"
#include "NetDDEService.hpp"
#include <NCL/DDEServer.hpp>
#include <WCL/Exception.hpp>
#include <Core/Algorithm.hpp>

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

CAppCmds::CAppCmds(CAppWnd& appWnd)
	: CCmdControl(appWnd)
{
	// Define the command table.
	DEFINE_CMD_TABLE
		// File menu.
		CMD_ENTRY(ID_FILE_EXIT,			&CAppCmds::OnFileExit,			nullptr,	-1)
		// View menu.
		CMD_ENTRY(ID_VIEW_SERVER_CONNS,	&CAppCmds::OnViewServerConns,	nullptr,	-1)
		CMD_ENTRY(ID_VIEW_DDE_CONVS,	&CAppCmds::OnViewDDEConvs,		nullptr,	-1)
		CMD_ENTRY(ID_VIEW_CLEAR_TRACE,	&CAppCmds::OnViewClearTrace,	nullptr,	-1)
		// Options menu.
		CMD_ENTRY(ID_OPTIONS_GENERAL,	&CAppCmds::OnOptionsGeneral,	nullptr,	-1)
		CMD_ENTRY(ID_OPTIONS_TRACE,		&CAppCmds::OnOptionsTrace,		nullptr,	-1)
		CMD_ENTRY(ID_OPTIONS_SERVICES,	&CAppCmds::OnOptionsServices,	nullptr,	-1)
		// Help menu.
		CMD_ENTRY(ID_HELP_ABOUT,		&CAppCmds::OnHelpAbout,			nullptr,	10)
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
	for (size_t i = 0; i < App.m_aoServices.size(); ++i)
	{
		if (App.m_aoServices[i]->m_oConnection.IsOpen())
			nConns++;
	}

	// Disallow, if any live connections.
	if (nConns > 0)
	{
		App.NotifyMsg(TXT("Services cannot be configured while they are being used.\n")
					  TXT("Please close all DDE connections to the NetDDEClient first."));
		return;
	}

	CServicesDlg Dlg;

	// Initialise dialog with current service list.
	for (size_t i = 0; i < App.m_aoServices.size(); ++i)
		Dlg.m_aoServices.push_back(new CNetDDESvcCfg(App.m_aoServices[i]->m_oCfg));

	// Show config dialog.
	if ( (Dlg.RunModal(App.m_rMainWnd) == IDOK) && (Dlg.m_bModified) )
	{
		// Cleanup all current services.
		for (size_t i = 0; i < App.m_aoServices.size(); ++i)
		{
			CNetDDEService* pService = App.m_aoServices[i];

			App.Trace(TXT("DDE_STATUS: Unregistering service: %s"), pService->m_oCfg.m_strLocName.c_str());

			// Unregister the service name.
			App.m_pDDEServer->Unregister(pService->m_oCfg.m_strLocName);
		}

		Core::deleteAll(App.m_aoServices);

		// Start all new ones.
		for (size_t i = 0; i < Dlg.m_aoServices.size(); ++i)
		{
			CNetDDESvcCfg*  pSvcCfg  = Dlg.m_aoServices[i];
			CNetDDEService* pService = new CNetDDEService;

			// Set config.
			pService->m_oCfg = *pSvcCfg;

			try
			{
				App.Trace(TXT("DDE_STATUS: Registering service: %s [%s]"), pService->m_oCfg.m_strLocName.c_str(), pService->m_oCfg.m_strServer.c_str());

				// Register the DDE service name.
				App.m_pDDEServer->Register(pService->m_oCfg.m_strLocName);
			}
			catch (const Core::Exception& e)
			{
				App.AlertMsg(TXT("Failed to register DDE service: %s\n\n%s"), pService->m_oCfg.m_strLocName.c_str(), e.twhat());
			}

			// Add to collection.
			App.m_aoServices.push_back(pService);

			// Attach event handler.
			pService->m_oConnection.AddClientListener(&App);
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
