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
const char* CNetDDECltApp::INI_FILE_VER  = "1.0";
const uint  CNetDDECltApp::BG_TIMER_FREQ =  1;
const uint  CNetDDECltApp::DEF_MAX_TRACE = 25;

// Background processing re-entrancy flag.
bool CNetDDECltApp::g_bInBgProcessing = false;

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
	, m_nTimerID(0)
	, m_nMaxTrace(DEF_MAX_TRACE)
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

	// Create the main window.
	if (!m_AppWnd.Create())
		return false;

	// Show it.
	if ( (m_iCmdShow == SW_SHOWNORMAL) && (m_rcLastPos.Empty() == false) )
		m_AppWnd.Move(m_rcLastPos);

	m_AppWnd.Show(m_iCmdShow);

	// Update UI.
	m_AppCmds.UpdateUI();

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

			App.Trace("PIPE_STATUS: Connecting to %s", strPipeName);

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

			// Send client connect message.
			CNetDDEPacket oPacket(CNetDDEPacket::NETDDE_CLIENT_CONNECT, oBuffer);

			App.Trace("NETDDE_CLIENT_CONNECT: %u %s %s %s", NETDDE_PROTOCOL, pService->m_strService, CSysInfo::ComputerName(), CSysInfo::UserName());

			pService->m_oConnection.SendPacket(oPacket);

			// Wait for response.
			pService->m_oConnection.WaitForPacket(oPacket, CNetDDEPacket::NETDDE_CLIENT_CONNECT);
		}
		catch (CPipeException& e)
		{
			App.Trace("PIPE_ERROR: %s", e.ErrorText());

			AlertMsg("Failed to initialise service: %s\n\n%s", pService->m_strService, e.ErrorText());
		}
		catch (CException& e)
		{
			AlertMsg("Failed to initialise service: %s\n\n%s", pService->m_strService, e.ErrorText());
		}
	}

	// Start the background timer.
	m_nTimerID = StartTimer(BG_TIMER_FREQ);

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
	// Start the background timer.
	StopTimer(m_nTimerID);

	// Terminate the services.
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		try
		{
			// Unregister the service name.
			m_pDDEServer->Unregister(pService->m_strService);

			CBuffer    oBuffer;
			CMemStream oStream(oBuffer);

			oStream.Create();

			oStream << pService->m_strService;
			oStream << CSysInfo::ComputerName();

			oStream.Close();

			// Send discoonect message.
			CNetDDEPacket oPacket(CNetDDEPacket::NETDDE_CLIENT_DISCONNECT, oBuffer);

			App.Trace("NETDDE_CLIENT_DISCONNECT: %s %s", pService->m_strService, CSysInfo::ComputerName());

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

	// Save settings.
	SaveConfig();
	
	return true;
}

/******************************************************************************
** Method:		Trace()
**
** Description:	Displays a trace message in the trace window.
**
** Parameters:	pszMsg			The message format.
**				...				Variable number of arguments.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::Trace(const char* pszMsg, ...)
{
	CString strMsg; //, strTime;

	// Setup arguments.
	va_list	args;
	va_start(args, pszMsg);
	
	// Format message.
	strMsg.FormatEx(pszMsg, args);
//	strTime.Format("0x%08X ", ::GetTickCount());

	// Send to trace window.	
	m_AppWnd.m_AppDlg.Trace(/*strTime +*/ strMsg);
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
			CString strAlias    = m_oIniFile.ReadString(strSection, "Alias",   "");
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

	// Read the trace settings.
	m_nMaxTrace = m_oIniFile.ReadInt("UI", "MaxTrace", m_nMaxTrace);

	// Read the window pos and size.
	m_rcLastPos.left   = m_oIniFile.ReadInt("UI", "Left",   0);
	m_rcLastPos.top    = m_oIniFile.ReadInt("UI", "Top",    0);
	m_rcLastPos.right  = m_oIniFile.ReadInt("UI", "Right",  0);
	m_rcLastPos.bottom = m_oIniFile.ReadInt("UI", "Bottom", 0);
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

	// Write the trace settings.
	m_oIniFile.WriteInt("UI", "MaxTrace", m_nMaxTrace);

	// Write the window pos and size.
	m_oIniFile.WriteInt("UI", "Left",   m_rcLastPos.left  );
	m_oIniFile.WriteInt("UI", "Top",    m_rcLastPos.top   );
	m_oIniFile.WriteInt("UI", "Right",  m_rcLastPos.right );
	m_oIniFile.WriteInt("UI", "Bottom", m_rcLastPos.bottom);
}

/******************************************************************************
** Method:		FindService()
**
** Description:	Finds the NetDDEService by name.
**
** Parameters:	pszService	The service name.
**
** Returns:		The service or NULL.
**
*******************************************************************************
*/

CNetDDEService* CNetDDECltApp::FindService(const char* pszService) const
{
	// Check services list...
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		// Valid service name?
		if (pService->m_strService == pszService)
			return pService;
	}

	return NULL;
}

/******************************************************************************
** Method:		FindService()
**
** Description:	Finds the NetDDEService for the server side conversarion handle.
**
** Parameters:	hSvrConv	The server side conversation handle.
**
** Returns:		The service or NULL.
**
*******************************************************************************
*/

CNetDDEService* CNetDDECltApp::FindService(HCONV hSvrConv) const
{
	// Check services list...
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		// Valid service name?
		if (pService->m_hSvrConv== hSvrConv)
			return pService;
	}

	return NULL;
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
		CNetDDEService* pService = FindService(pszService);

		// Valid service name?
		if (pService != NULL)
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream << pszService;
			oReqStream << pszTopic;

			oReqStream.Close();

			CNetDDEPacket oPacket(CNetDDEPacket::DDE_CREATE_CONVERSATION, oBuffer);

			App.Trace("DDE_CREATE_CONVERSATION: %s %s", pszService, pszTopic);

			// Send it.
			pService->m_oConnection.SendPacket(oPacket);

			// Wait for response.
			pService->m_oConnection.WaitForPacket(oPacket, CNetDDEPacket::DDE_CREATE_CONVERSATION);

			HCONV hSvrConv;

			CMemStream oRspStream(oPacket.Buffer());

			oRspStream.Open();
			oRspStream.Seek(sizeof(CNetDDEPacket::Header));

			// Get result.
			oRspStream >> bAccept;
			oRspStream.Read(&hSvrConv, sizeof(HCONV));

			oRspStream.Close();

			ASSERT((pService->m_hSvrConv == NULL) || (pService->m_hSvrConv == hSvrConv));

			if (bAccept)
				pService->m_hSvrConv = hSvrConv;
		}
	}
	catch (CPipeException& e)
	{
		App.Trace("PIPE_ERROR: %s", e.ErrorText());
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
	CNetDDEService* pService = FindService(pConv->Service());

	ASSERT(pService != NULL);

	pService->m_aoConvs.Add(pConv);
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
	bool bResult = false;

	try
	{
		CNetDDEService* pService = FindService(pConv->Service());

		// Valid service name?
		if (pService != NULL)
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pService->m_hSvrConv, sizeof(HCONV));

			oReqStream.Close();

			CNetDDEPacket oPacket(CNetDDEPacket::DDE_DESTROY_CONVERSATION, oBuffer);

			App.Trace("DDE_DESTROY_CONVERSATION: %s %s", pConv->Service(), pConv->Topic());

			// Send it.
			pService->m_oConnection.SendPacket(oPacket);

			// Wait for response.
			pService->m_oConnection.WaitForPacket(oPacket, CNetDDEPacket::DDE_DESTROY_CONVERSATION);

			CMemStream oRspStream(oPacket.Buffer());

			oRspStream.Open();
			oRspStream.Seek(sizeof(CNetDDEPacket::Header));

			// Get result.
			oRspStream >> bResult;

			oRspStream.Close();

			// Remove from connections conversation list.
			pService->m_aoConvs.Remove(pService->m_aoConvs.Find(pConv));
		}
	}
	catch (CPipeException& e)
	{
		App.Trace("PIPE_ERROR: %s", e.ErrorText());
	}
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

bool CNetDDECltApp::OnRequest(CDDESvrConv* pConv, const char* pszItem, uint nFormat, CDDEData& oData)
{
	// Custom formats not supported.
	if (nFormat >= 0xC000)
		return false;

	bool bResult = false;

	try
	{
		CNetDDEService* pService = FindService(pConv->Service());

		// Valid service name?
		if (pService != NULL)
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pService->m_hSvrConv, sizeof(HCONV));
			oReqStream << pszItem;
			oReqStream << (uint32) nFormat;

			oReqStream.Close();

			CNetDDEPacket oPacket(CNetDDEPacket::DDE_REQUEST, oBuffer);

			App.Trace("DDE_REQUEST: %s %s %s %s", pConv->Service(), pConv->Topic(), pszItem, CClipboard::FormatName(nFormat));

			// Send it.
			pService->m_oConnection.SendPacket(oPacket);

			// Wait for response.
			pService->m_oConnection.WaitForPacket(oPacket, CNetDDEPacket::DDE_REQUEST);

			CBuffer    oDDEData;
			CMemStream oRspStream(oPacket.Buffer());

			oRspStream.Open();
			oRspStream.Seek(sizeof(CNetDDEPacket::Header));

			// Get result.
			oRspStream >> bResult;
			oRspStream >> oDDEData;

			oRspStream.Close();

			if (bResult)
				oData.SetBuffer(oDDEData);
		}
	}
	catch (CPipeException& e)
	{
		App.Trace("PIPE_ERROR: %s", e.ErrorText());
	}

	return bResult;
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

bool CNetDDECltApp::OnAdviseStart(CDDESvrConv* pConv, const char* pszItem, uint nFormat)
{
	// Custom formats not supported.
	if (nFormat >= 0xC000)
		return false;

	bool bResult = false;

	try
	{
		CNetDDEService* pService = FindService(pConv->Service());

		// Valid service name?
		if (pService != NULL)
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pService->m_hSvrConv, sizeof(HCONV));
			oReqStream << pszItem;
			oReqStream << (uint32) nFormat;

			oReqStream.Close();

			CNetDDEPacket oPacket(CNetDDEPacket::DDE_START_ADVISE, oBuffer);

			App.Trace("DDE_START_ADVISE: %s %s %s %s", pConv->Service(), pConv->Topic(), pszItem, CClipboard::FormatName(nFormat));

			// Send it.
			pService->m_oConnection.SendPacket(oPacket);

			// Wait for response.
			pService->m_oConnection.WaitForPacket(oPacket, CNetDDEPacket::DDE_START_ADVISE);

			CMemStream oRspStream(oPacket.Buffer());

			oRspStream.Open();
			oRspStream.Seek(sizeof(CNetDDEPacket::Header));

			// Get result.
			oRspStream >> bResult;

			oRspStream.Close();
		}
	}
	catch (CPipeException& e)
	{
		App.Trace("PIPE_ERROR: %s", e.ErrorText());
	}

	return bResult;
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

void CNetDDECltApp::OnAdviseConfirm(CDDESvrConv* pConv, CDDELink* pLink)
{
	CNetDDEService* pService = FindService(pConv->Service());

	ASSERT(pService != NULL);

	pService->m_aoLinks.Add(pLink);
}

/******************************************************************************
** Method:		OnAdviseRequest()
**
** Description:	DDEML requesting data for a link which has been updated.
**
** Parameters:	hConv		The conversation handle.
**				pszItem		The item being requested.
**				nFormat		The data format.
**				oData		The data to return.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDECltApp::OnAdviseRequest(CDDESvrConv* pConv, CDDELink* pLink, CDDEData& oData)
{
	CBuffer* pBuffer = NULL;

	// Fetch link data from cache.
	if (m_oLinksData.Find(pLink, pBuffer))
		oData.SetBuffer(*pBuffer);

	return (pBuffer != NULL);
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

void CNetDDECltApp::OnAdviseStop(CDDESvrConv* pConv, CDDELink* pLink)
{
	bool bResult = false;

	try
	{
		CNetDDEService* pService = FindService(pConv->Service());

		// Valid service name?
		if (pService != NULL)
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pService->m_hSvrConv, sizeof(HCONV));
			oReqStream << pLink->Item();
			oReqStream << (uint32) pLink->Format();

			oReqStream.Close();

			CNetDDEPacket oPacket(CNetDDEPacket::DDE_STOP_ADVISE, oBuffer);

			App.Trace("DDE_STOP_ADVISE: %s %s %s %s", pConv->Service(), pConv->Topic(), pLink->Item(), CClipboard::FormatName(pLink->Format()));

			// Send it.
			pService->m_oConnection.SendPacket(oPacket);

			// Wait for response.
			pService->m_oConnection.WaitForPacket(oPacket, CNetDDEPacket::DDE_STOP_ADVISE);

			CMemStream oRspStream(oPacket.Buffer());

			oRspStream.Open();
			oRspStream.Seek(sizeof(CNetDDEPacket::Header));

			// Get result.
			oRspStream >> bResult;

			oRspStream.Close();

			// Remove from connections links list.
			pService->m_aoLinks.Remove(pService->m_aoLinks.Find(pLink));
		}
	}
	catch (CPipeException& e)
	{
		App.Trace("PIPE_ERROR: %s", e.ErrorText());
	}
}

/******************************************************************************
** Method:		OnTimer()
**
** Description:	The timer has gone off, process background tasks.
**				NB: Re-entrancy can be caused when performing DDE requests.
**
** Parameters:	nTimerID	The timer ID.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnTimer(uint nTimerID)
{
	ASSERT(nTimerID == m_nTimerID);

	// Guard against re-entrancy.
	if (!g_bInBgProcessing)
	{
		g_bInBgProcessing = true;

		HandleNotifications();

		g_bInBgProcessing = false;
	}
}

/******************************************************************************
** Method:		HandleRequests()
**
** Description:	Handle requests from NetDDEClients.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::HandleNotifications()
{
	// For all services...
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		// Ignore if connection severed.
		if (!pService->m_oConnection.IsOpen())
			continue;

		try
		{
			// Get next notification packet, if one.
			CNetDDEPacket* pPacket = pService->m_oConnection.ReadNotifyPacket();

			if (pPacket != NULL)
			{
					// Decode packet type.
					switch (pPacket->DataType())
					{
						case CNetDDEPacket::NETDDE_SERVER_DISCONNECT:	OnNetDDEServerDisconnect(*pService, *pPacket);	break;
						case CNetDDEPacket::DDE_DISCONNECT:				OnDDEDisconnect(*pService, *pPacket);			break;
						case CNetDDEPacket::DDE_ADVISE:					OnDDEAdvise(*pService, *pPacket);				break;
						default:										ASSERT(false);									break;
					}

				delete pPacket;
			}
		}
		catch (CPipeException& e)
		{
			App.Trace("PIPE_ERROR: %s", e.ErrorText());

			// Pipe disconnected?
			pService->m_oConnection.Close();
		}
		catch (CException& e)
		{
			App.Trace("UNHANDLED_EXCEPTION: %s", e.ErrorText());
		}
	}
}

/******************************************************************************
** Method:		OnNetDDEServerDisconnect()
**
** Description:	NetDDEServer terminating a connection.
**
** Parameters:	oService		The service the packet came from.
**				oNfyPacket		The notification packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnNetDDEServerDisconnect(CNetDDEService& oService, CNetDDEPacket& oNfyPacket)
{
	App.Trace("NETDDE_SERVER_DISCONNECT: %s", oService.m_strServer);

	// Close connection to server.
	oService.m_oConnection.Close();
}

/******************************************************************************
** Method:		OnDDEDisconnect()
**
** Description:	DDE Server disconnected from NetDDEServer.
**
** Parameters:	oService		The service the packet came from.
**				oNfyPacket		The notification packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnDDEDisconnect(CNetDDEService& oService, CNetDDEPacket& oNfyPacket)
{
	App.Trace("DDE_DISCONNECT:");
}

/******************************************************************************
** Method:		OnDDEAdvise()
**
** Description:	DDE Advise on an item.
**
** Parameters:	oService		The service the packet came from.
**				oNfyPacket		The notification packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnDDEAdvise(CNetDDEService& oService, CNetDDEPacket& oNfyPacket)
{
	ASSERT(oNfyPacket.DataType() == CNetDDEPacket::DDE_ADVISE);

	HCONV      hSvrConv;
	CString    strItem;
	uint32     nFormat;
	CBuffer    oData;

	CMemStream oStream(oNfyPacket.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	// Get advise parameters.
	oStream.Read(&hSvrConv, sizeof(HCONV));
	oStream >> strItem;
	oStream >> nFormat;
	oStream >> oData;;

	oStream.Close();

	App.Trace("DDE_ADVISE: %s %s %s", oService.m_strService, strItem, CClipboard::FormatName(nFormat));

	// Find the service for the conversation handle.
	CNetDDEService* pService = FindService(hSvrConv);

	if (pService != NULL)
	{
		// For all links....
		for (int i = 0; i < pService->m_aoLinks.Size(); ++i)
		{
			CDDELink* pLink = pService->m_aoLinks[i];

			// Post advise, if it's the link that has been updated.
			if ( (pLink->Item() == strItem) && (pLink->Format() == nFormat) )
			{
				CBuffer* pBuffer = NULL;

				// Allocate a cache for the link data, if required.
				if (!m_oLinksData.Find(pLink, pBuffer))
				{
					pBuffer = new CBuffer();

					m_oLinksData.Add(pLink, pBuffer);
				}

				// Cache links' data.
				*pBuffer = oData;

				// Notify DDE Client of advise.
				CDDESvrConv* pConv = static_cast<CDDESvrConv*>(pLink->Conversation());

				pConv->PostLinkUpdate(pLink);
			}
		}
	}
}
