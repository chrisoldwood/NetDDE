/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDECLTAPP.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CNetDDECltApp class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"

/******************************************************************************
**
** Global variables.
**
*******************************************************************************
*/

// "The" application object.
CNetDDECltApp App;

/******************************************************************************
**
** Class constants.
**
*******************************************************************************
*/

#ifdef _DEBUG
const char* CNetDDECltApp::VERSION      = "v1.0 [Debug]";
#else
const char* CNetDDECltApp::VERSION      = "v1.0";
#endif
const char* CNetDDECltApp::INI_FILE_VER = "1.0";

/******************************************************************************
** Method:		Constructor
**
** Description:	Default constructor.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CNetDDECltApp::CNetDDECltApp()
	: CApp(m_AppWnd, m_AppCmds)
	, m_pDDEServer(CDDEServer::Instance())
{

}

/******************************************************************************
** Method:		Destructor
**
** Description:	Cleanup.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CNetDDECltApp::~CNetDDECltApp()
{
	m_aoServices.DeleteAll();
}

/******************************************************************************
** Method:		OnOpen()
**
** Description:	Initialises the application.
**
** Parameters:	None.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDECltApp::OnOpen()
{
	// Set the app title.
	m_strTitle = "Net DDE Client";

	// Load settings.
	LoadConfig();

	try
	{
		// Initialise the DDE server.
		m_pDDEServer->Initialise();
		m_pDDEServer->AddListener(this);
	}
	catch (CException& e)
	{
		FatalMsg(e.ErrorText());
		return false;
	}

	// Initialise services.
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		try
		{
			// Register the DDE service name.
			m_pDDEServer->Register(pService->m_strService);

			CString strPipeName;

			strPipeName.Format(NETDDE_PIPE_FORMAT, pService->m_strServer, pService->m_strPipeName);

			// Open the connection to the server
			pService->m_oConnection.Open(strPipeName);

			// Create connect message.
			CBuffer    oBuffer;
			CMemStream oStream(oBuffer);

			oStream.Create();

			oStream << NETDDE_PROTOCOL;
			oStream << pService->m_strService;
			oStream << CSysInfo::ComputerName();
			oStream << CSysInfo::UserName();

			oStream.Close();

			CNetDDEPacket oPacket(CNetDDEPacket::NETDDE_CONNECT, oBuffer);

			// Send it.
			pService->m_oConnection.SendPacket(oPacket);

			// Wait for response.
			pService->m_oConnection.WaitForPacket(oPacket, CNetDDEPacket::NETDDE_CONNECT);
		}
		catch (CException& e)
		{
			AlertMsg("Failed to initialise service: %s\n\n%s", pService->m_strService, e.ErrorText());
		}
	}

	// Create the main window.
	if (!m_AppWnd.Create())
		return false;

	// Show it.
	m_AppWnd.Show(m_iCmdShow);

	// Update UI.
	m_AppCmds.UpdateUI();

	return true;
}

/******************************************************************************
** Method:		OnClose()
**
** Description:	Cleans up the application resources.
**
** Parameters:	None.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDECltApp::OnClose()
{
	// Save settings.
	SaveConfig();

	
	// Terminate the services.
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		try
		{
			// Unregister the service name.
			m_pDDEServer->Unregister(pService->m_strService);

			// Send discoonect message.
			CNetDDEPacket oPacket(CNetDDEPacket::NETDDE_DISCONNECT, NULL, 0);

			pService->m_oConnection.SendPacket(oPacket);
		}
		catch (CException& /*e*/)
		{
		}

		// Close the connection.
		pService->m_oConnection.Close();
	}

	// Unnitialise the DDE client.
	m_pDDEServer->RemoveListener(this);
	m_pDDEServer->Uninitialise();

	return true;
}

/******************************************************************************
** Method:		LoadConfig()
**
** Description:	Load the app configuration.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::LoadConfig()
{
	// Read the file version.
	CString strVer = m_oIniFile.ReadString("Version", "Version", INI_FILE_VER);

	// Read the service descriptions.
	int nCount = m_oIniFile.ReadInt("Services", "Count", 0);

	for (int i = 0; i < nCount; ++i)
	{
		CString strEntry;

		strEntry.Format("Service[%d]", i);

		CString strSection = m_oIniFile.ReadString("Services", strEntry, "");

		// Entry valid?
		if (strSection != "")
		{
			CString strService  = m_oIniFile.ReadString(strSection, "Service", "");
			CString strServer   = m_oIniFile.ReadString(strSection, "Server",  "");
			CString strPipeName = m_oIniFile.ReadString(strSection, "Pipe",    NETDDE_PIPE_DEFAULT);

			// Section valid?
			if ( (strService != "") && (strServer != "") && (strPipeName != "") )
			{
				// Add to collection.
				CNetDDEService* pService = new CNetDDEService;

				pService->m_strService  = strService;
				pService->m_strServer   = strServer;
				pService->m_strPipeName = strPipeName;

				m_aoServices.Add(pService);
			}
		}
	}
}

/******************************************************************************
** Method:		SaveConfig()
**
** Description:	Save the app configuration.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::SaveConfig()
{
	// Write the file version.
	m_oIniFile.WriteString("Version", "Version", INI_FILE_VER);
}

/******************************************************************************
** Method:		OnConnect()
**
** Description:	DDE Server querying to accept a connection.
**
** Parameters:	pszService	The service name.
**				pszTopic	The topic name.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDECltApp::OnConnect(const char* pszService, const char* pszTopic)
{
	bool bAccept = false;

	try
	{
		// Check services list...
		for (int i = 0; i < m_aoServices.Size(); ++i)
		{
			CNetDDEService* pService = m_aoServices[i];

			// Valid service name?
			if (pService->m_strService == pszService)
			{
				// Create conversation message.
				CBuffer    oBuffer;
				CMemStream oReqStream(oBuffer);

				oReqStream.Create();

				oReqStream << pszService;
				oReqStream << pszTopic;

				oReqStream.Close();

				CNetDDEPacket oPacket(CNetDDEPacket::DDE_CREATE_CONVERSATION, oBuffer);

				// Send it.
				pService->m_oConnection.SendPacket(oPacket);

				// Wait for response.
				pService->m_oConnection.WaitForPacket(oPacket, CNetDDEPacket::DDE_CREATE_CONVERSATION);

				CMemStream oRspStream(oPacket.Buffer());

				oRspStream.Open();
				oRspStream.Seek(sizeof(CNetDDEPacket::Header));

				// Get result.
				oRspStream >> bAccept;

				oRspStream.Close();
			}
		}
	}
	catch (CException& /*e*/)
	{
	}

	return bAccept;
}

/******************************************************************************
** Method:		OnConnectConfirm()
**
** Description:	Connection established.
**
** Parameters:	pConv	The conversation created.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnConnectConfirm(CDDESvrConv* pConv)
{
}

/******************************************************************************
** Method:		OnDisconnect()
**
** Description:	Connection terminated.
**
** Parameters:	pConv	The conversation terminated.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnDisconnect(CDDESvrConv* pConv)
{
}

/******************************************************************************
** Method:		OnRequest()
**
** Description:	DDE Client is requesting an item.
**
** Parameters:	pConv		The conversation.
**				pszItem		The item being requested.
**				nFormat		The data format.
**				oData		The data to return.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDECltApp::OnRequest(CDDEConv* pConv, const char* pszItem, uint nFormat, CDDEData& oData)
{
	if (nFormat != CF_TEXT)
		return false;

	oData.SetString("Hello World 2");

	return true;
}

/******************************************************************************
** Method:		OnAdviseStart()
**
** Description:	DDE Client is querying to accept a link.
**
** Parameters:	pConv		The conversation.
**				pszItem		The item being requested.
**				nFormat		The data format.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDECltApp::OnAdviseStart(CDDEConv* pConv, const char* pszItem, uint nFormat)
{
	return false;
}

/******************************************************************************
** Method:		OnAdviseConfirm()
**
** Description:	DDE link established.
**
** Parameters:	pConv		The conversation.
**				pLink		The link.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnAdviseConfirm(CDDEConv* pConv, CDDELink* pLink)
{
}

/******************************************************************************
** Method:		OnAdviseStop()
**
** Description:	DDE link terminated.
**
** Parameters:	pConv		The conversation.
**				pLink		The link.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnAdviseStop(CDDEConv* pConv, CDDELink* pLink)
{
}
