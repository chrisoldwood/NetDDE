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

#ifdef _DEBUG
// For memory leak detection.
#define new DBGCRT_NEW
#endif

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

const bool  CNetDDECltApp::DEF_TRAY_ICON         = true;
const bool  CNetDDECltApp::DEF_MIN_TO_TRAY       = false;
const bool  CNetDDECltApp::DEF_TRACE_CONVS       = true;
const bool  CNetDDECltApp::DEF_TRACE_REQUESTS    = false;
const bool  CNetDDECltApp::DEF_TRACE_ADVISES     = false;
const bool  CNetDDECltApp::DEF_TRACE_UPDATES     = false;
const bool  CNetDDECltApp::DEF_TRACE_NET_CONNS   = true;
const bool  CNetDDECltApp::DEF_TRACE_TO_WINDOW   = true;
const int   CNetDDECltApp::DEF_TRACE_LINES       = 100;
const bool  CNetDDECltApp::DEF_TRACE_TO_FILE     = false;
const char* CNetDDECltApp::DEF_TRACE_FILE        = "NetDDEClient.log";

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
	, m_bTrayIcon(DEF_TRAY_ICON)
	, m_bMinToTray(DEF_MIN_TO_TRAY)
	, m_bTraceConvs(DEF_TRACE_CONVS)
	, m_bTraceRequests(DEF_TRACE_REQUESTS)
	, m_bTraceAdvises(DEF_TRACE_ADVISES)
	, m_bTraceUpdates(DEF_TRACE_UPDATES)
	, m_bTraceNetConns(DEF_TRACE_NET_CONNS)
	, m_bTraceToWindow(DEF_TRACE_TO_WINDOW)
	, m_nTraceLines(DEF_TRACE_LINES)
	, m_bTraceToFile(DEF_TRACE_TO_FILE)
	, m_strTraceFile(DEF_TRACE_FILE)
	, m_nPktsSent(0)
	, m_nPktsRecv(0)
	, m_dwTickCount(::GetTickCount())
{
	// Expect around 1000 links.
	m_oLinksData.Reserve(1000);
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
	m_strTitle = "NetDDE Client";

	// Load settings.
	LoadConfig();

	// Create the full trace file path.
	m_strTracePath = CPath(CPath::ApplicationDir(), m_strTraceFile);

	// Clear the trace file.
	if (m_bTraceToFile)
	{
		try
		{
			m_fTraceFile.Create(m_strTracePath);
			m_fTraceFile.Close();
		}
		catch (CFileException& e)
		{
			AlertMsg("Failed to truncate trace file:\n\n%s", e.ErrorText());

			m_bTraceToFile = false;
		}
	}

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

	// Register DDE services.
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		try
		{
			App.Trace("DDE_STATUS: Registering service: %s", pService->m_oCfg.m_strService);

			// Register the DDE service name.
			m_pDDEServer->Register(pService->m_oCfg.m_strService);
		}
		catch (CException& e)
		{
			AlertMsg("Failed to register DDE service: %s\n\n%s", pService->m_oCfg.m_strService, e.ErrorText());
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

		// Unregister the service name.
		m_pDDEServer->Unregister(pService->m_oCfg.m_strService);

		// Disconnect from server.
		ServerDisconnect(pService);
	}

	// Unnitialise the DDE client.
	m_pDDEServer->RemoveListener(this);
	m_pDDEServer->Uninitialise();

	CString			strLink;
	CLinkValue*		pLinkValue = NULL;
	CLinksDataIter	oIter(App.m_oLinksData);

	// Empty the link value cache.
	while (oIter.Next(strLink, pLinkValue))
		delete pLinkValue;

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
	// Nothing to do?
	if (!m_bTraceToWindow && !m_bTraceToFile)
		return;

	CString strMsg;

	// Setup arguments.
	va_list	args;
	va_start(args, pszMsg);
	
	// Format message.
	strMsg.FormatEx(pszMsg, args);

	// Prepend date and time.
	strMsg = CDateTime::Current().ToString() + " " + strMsg;

	// Send to trace window.	
	if (m_bTraceToWindow)
	{
		m_AppWnd.m_AppDlg.Trace(strMsg);
	}

	// Write trace to log file.
	if (m_bTraceToFile)
	{
		try
		{
			if (m_strTracePath.Exists())
				m_fTraceFile.Open(m_strTracePath, GENERIC_WRITE);
			else
				m_fTraceFile.Create(m_strTracePath);

			m_fTraceFile.Seek(0, FILE_END);
			m_fTraceFile.WriteLine(strMsg);
			m_fTraceFile.Close();
		}
		catch (CFileException& e)
		{
			AlertMsg("Failed to write to trace file:\n\n%s", e.ErrorText());

			m_bTraceToFile = false;
		}
	}
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

				pService->m_oCfg.m_strService    = strService;
				pService->m_oCfg.m_strServer     = strServer;
				pService->m_oCfg.m_strPipeName   = strPipeName;
				pService->m_oCfg.m_bAsyncAdvises = m_oIniFile.ReadBool(strSection, "AsyncAdvises", pService->m_oCfg.m_bAsyncAdvises);

				m_aoServices.Add(pService);
			}
		}
	}

	// Read the trace settings.
	m_bTraceConvs    = m_oIniFile.ReadBool  ("Trace", "Conversations",  m_bTraceConvs   );
	m_bTraceRequests = m_oIniFile.ReadBool  ("Trace", "Requests",       m_bTraceRequests);
	m_bTraceAdvises  = m_oIniFile.ReadBool  ("Trace", "Advises",        m_bTraceAdvises );
	m_bTraceUpdates  = m_oIniFile.ReadBool  ("Trace", "Updates",        m_bTraceUpdates );
	m_bTraceNetConns = m_oIniFile.ReadBool  ("Trace", "NetConnections", m_bTraceNetConns);
	m_bTraceToWindow = m_oIniFile.ReadBool  ("Trace", "ToWindow",       m_bTraceToWindow);
	m_nTraceLines    = m_oIniFile.ReadInt   ("Trace", "Lines",          m_nTraceLines   );
	m_bTraceToFile   = m_oIniFile.ReadBool  ("Trace", "ToFile",         m_bTraceToFile  );
	m_strTraceFile   = m_oIniFile.ReadString("Trace", "FileName",       m_strTraceFile  );

	// Read the UI settings.
	m_bTrayIcon  = m_oIniFile.ReadBool("UI", "TrayIcon",  m_bTrayIcon );
	m_bMinToTray = m_oIniFile.ReadBool("UI", "MinToTray", m_bMinToTray);

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
	m_oIniFile.WriteBool  ("Trace", "Conversations",  m_bTraceConvs   );
	m_oIniFile.WriteBool  ("Trace", "Requests",       m_bTraceRequests);
	m_oIniFile.WriteBool  ("Trace", "Advises",        m_bTraceAdvises );
	m_oIniFile.WriteBool  ("Trace", "Updates",        m_bTraceUpdates );
	m_oIniFile.WriteBool  ("Trace", "NetConnections", m_bTraceNetConns);
	m_oIniFile.WriteBool  ("Trace", "ToWindow",       m_bTraceToWindow);
	m_oIniFile.WriteInt   ("Trace", "Lines",          m_nTraceLines   );
	m_oIniFile.WriteBool  ("Trace", "ToFile",         m_bTraceToFile  );
	m_oIniFile.WriteString("Trace", "FileName",       m_strTraceFile  );

	// Write the UI settings.
	m_oIniFile.WriteBool("UI", "TrayIcon",  m_bTrayIcon );
	m_oIniFile.WriteBool("UI", "MinToTray", m_bMinToTray);

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
		if (pService->m_oCfg.m_strService == pszService)
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

	CNetDDEService* pService = FindService(pszService);

	// Valid service name?
	if (pService != NULL)
	{
		try
		{
			// Open connection, if first conversation.
			if (pService->m_aoConvs.Size() == 0)
				ServerConnect(pService);

			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream << pszService;
			oReqStream << pszTopic;

			oReqStream.Close();

			CNetDDEPacket oPacket(CNetDDEPacket::DDE_CREATE_CONVERSATION, oBuffer);

			if (m_bTraceConvs)
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

			// Update stats.
			++m_nPktsSent;
			++m_nPktsRecv;
		}
		catch (CPipeException& e)
		{
			App.Trace("PIPE_ERROR: %s", e.ErrorText());

			// Pipe disconnected?
			pService->m_oConnection.Close();
		}
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
	CNetDDEService* pService = FindService(pConv->Service());

	// Valid service name?
	if (pService != NULL)
	{
		try
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pService->m_hSvrConv, sizeof(HCONV));

			oReqStream.Close();

			CNetDDEPacket oPacket(CNetDDEPacket::DDE_DESTROY_CONVERSATION, oBuffer);

			if (m_bTraceConvs)
				App.Trace("DDE_DESTROY_CONVERSATION: %s %s", pConv->Service(), pConv->Topic());

			// Send it.
			pService->m_oConnection.SendPacket(oPacket);

			// Remove from connections conversation list.
			pService->m_aoConvs.Remove(pService->m_aoConvs.Find(pConv));

			CDDESvrLinks aoLinks;

			// Get all conversation links.
			pConv->GetAllLinks(aoLinks);

			// Remove links from service links collection.
			for (int i = 0; i < aoLinks.Size(); ++i)
				pService->m_aoLinks.Remove(pService->m_aoLinks.Find(aoLinks[i]));

			// Close connection, if last conversation.
			if (pService->m_aoConvs.Size() == 0)
				ServerDisconnect(pService);

			// Update stats.
			++m_nPktsSent;
		}
		catch (CPipeException& e)
		{
			App.Trace("PIPE_ERROR: %s", e.ErrorText());

			// Pipe disconnected?
			pService->m_oConnection.Close();
		}
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

	CNetDDEService* pService = FindService(pConv->Service());

	// Valid service name AND connection open?
	if ( (pService != NULL) && (pService->m_oConnection.IsOpen()) )
	{
		try
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

			if (m_bTraceRequests)
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

			// Update stats.
			++m_nPktsSent;
			++m_nPktsRecv;
		}
		catch (CPipeException& e)
		{
			App.Trace("PIPE_ERROR: %s", e.ErrorText());

			// Pipe disconnected?
			pService->m_oConnection.Close();
		}
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

	CNetDDEService* pService = FindService(pConv->Service());

	// Valid service name?
	if ( (pService != NULL) && (pService->m_oConnection.IsOpen()) )
	{
		try
		{
			// Sync or Async advise?
			bool bSync    = !pService->m_oCfg.m_bAsyncAdvises;
			int  nPktType = bSync ? CNetDDEPacket::DDE_START_ADVISE : CNetDDEPacket::DDE_START_ADVISE_ASYNC;

			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pService->m_hSvrConv, sizeof(HCONV));
			oReqStream << pszItem;
			oReqStream << (uint32) nFormat;

			oReqStream.Close();

			CNetDDEPacket oPacket(nPktType, oBuffer);

			if (App.m_bTraceAdvises)
			{
				CString strType = bSync ? "DDE_START_ADVISE" : "DDE_START_ADVISE_ASYNC";

				App.Trace("%s: %s %s %s %s", strType, pConv->Service(), pConv->Topic(), pszItem, CClipboard::FormatName(nFormat));
			}

			// Send it.
			pService->m_oConnection.SendPacket(oPacket);

			// Expecting response?
			if (bSync)
			{
				// Wait for response.
				pService->m_oConnection.WaitForPacket(oPacket, CNetDDEPacket::DDE_START_ADVISE);

				CMemStream oRspStream(oPacket.Buffer());

				oRspStream.Open();
				oRspStream.Seek(sizeof(CNetDDEPacket::Header));

				// Get result.
				oRspStream >> bResult;

				oRspStream.Close();
			}
			// Asynchronous.
			else
			{
				bResult = true;
			}

			// Update stats.
			++m_nPktsSent;
			++m_nPktsRecv;
		}
		catch (CPipeException& e)
		{
			App.Trace("PIPE_ERROR: %s", e.ErrorText());

			// Pipe disconnected?
			pService->m_oConnection.Close();
		}
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

	// Add link to service links' list.
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
	CString     strLink;
	CLinkValue* pLinkValue = NULL;

	strLink.Format("%s%s%s%u", pConv->Service(), pConv->Topic(), pLink->Item(), pLink->Format());

	// Fetch link data from cache.
	m_oLinksData.Find(strLink, pLinkValue);

	ASSERT(pLinkValue != NULL);

	oData.SetBuffer(pLinkValue->m_oBuffer);

	return (pLinkValue != NULL);
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
	CNetDDEService* pService = FindService(pConv->Service());

	// Valid service name?
	if ( (pService != NULL) && (pService->m_oConnection.IsOpen()) )
	{
		try
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

			if (App.m_bTraceAdvises)
				App.Trace("DDE_STOP_ADVISE: %s %s %s %s", pConv->Service(), pConv->Topic(), pLink->Item(), CClipboard::FormatName(pLink->Format()));

			// Send it.
			pService->m_oConnection.SendPacket(oPacket);

			// Remove from connections links list.
			pService->m_aoLinks.Remove(pService->m_aoLinks.Find(pLink));

			// Update stats.
			++m_nPktsSent;
		}
		catch (CPipeException& e)
		{
			App.Trace("PIPE_ERROR: %s", e.ErrorText());

			// Pipe disconnected?
			pService->m_oConnection.Close();
		}
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
	try
	{
		ASSERT(nTimerID == m_nTimerID);

		// Guard against re-entrancy.
		if (!g_bInBgProcessing)
		{
			g_bInBgProcessing = true;

			HandleNotifications();
			HandleDisconnects();

			UpdateStats();

			g_bInBgProcessing = false;
		}
	}
	catch (CException& e)
	{
		StopTimer(m_nTimerID);

		FatalMsg("Unexpected Exception: %s", e.ErrorText());

		m_AppWnd.Close();
	}
	catch (...)
	{
		StopTimer(m_nTimerID);

		FatalMsg("Unhandled Exception.");

		m_AppWnd.Close();
	}
}

/******************************************************************************
** Method:		HandleNotifications()
**
** Description:	Handle notifications from NetDDEServers.
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
			CNetDDEPacket* pPacket = NULL; 

			// For all notification packets...
			while ((pPacket = pService->m_oConnection.ReadNotifyPacket()) != NULL)
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

				// Update stats.
				++m_nPktsRecv;
			}
		}
		catch (CPipeException& e)
		{
			App.Trace("PIPE_ERROR: %s", e.ErrorText());

			// Pipe disconnected?
			pService->m_oConnection.Close();
		}
	}
}

/******************************************************************************
** Method:		HandleDisconnects()
**
** Description:	Handle disconnections from NetDDEServers.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::HandleDisconnects()
{
	// For all services...
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		// Connection closed AND outstanding conversations?
		if ( (!pService->m_oConnection.IsOpen()) && (pService->m_aoConvs.Size() > 0) )
		{
			for (int j = 0; j < pService->m_aoConvs.Size(); ++j)
			{
				CDDESvrConv* pConv = pService->m_aoConvs[j];

				// Disconnect from DDE client.
				m_pDDEServer->DestroyConversation(pConv);
			}

			// Discard conversations and links.
			pService->m_aoConvs.RemoveAll();
			pService->m_aoLinks.RemoveAll();
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
	if (m_bTraceNetConns)
		App.Trace("NETDDE_SERVER_DISCONNECT: %s", oService.m_oCfg.m_strServer);

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
	ASSERT(oNfyPacket.DataType() == CNetDDEPacket::DDE_DISCONNECT);

	HCONV hSvrConv;

	CMemStream oStream(oNfyPacket.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	// Get conversation parameters.
	oStream.Read(&hSvrConv, sizeof(HCONV));

	oStream.Close();

	if (m_bTraceConvs)
		App.Trace("DDE_DISCONNECT:");

	// Find the service for the conversation handle.
	CNetDDEService* pService = FindService(hSvrConv);

	if (pService != NULL)
	{
		// Cleanup all client conversations...
		for (int j = 0; j < pService->m_aoConvs.Size(); ++j)
		{
			CDDESvrConv* pConv = pService->m_aoConvs[j];

			// Disconnect from DDE client.
			m_pDDEServer->DestroyConversation(pConv);
		}

		// Discard conversations and links.
		pService->m_aoConvs.RemoveAll();
		pService->m_aoLinks.RemoveAll();
	}
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

	if (App.m_bTraceUpdates)
	{
		CString strData;

		if (nFormat == CF_TEXT)
			strData = (const char*)(const void*)oData.Buffer();
		else
			strData = CClipboard::FormatName(nFormat);

		App.Trace("DDE_ADVISE: %s %s [%s]", oService.m_oCfg.m_strService, strItem, strData);
	}

	// Find the service for the conversation handle.
	CNetDDEService* pService = FindService(hSvrConv);

	if (pService != NULL)
	{
		// For all links....
		for (int i = 0; i < pService->m_aoLinks.Size(); ++i)
		{
			CDDELink* pLink = pService->m_aoLinks[i];
			CDDEConv* pConv = pLink->Conversation();

			// Post advise, if it's the link that has been updated.
			if ( (pLink->Item() == strItem) && (pLink->Format() == nFormat) )
			{
				CString     strLink;
				CLinkValue* pLinkValue = NULL;

				strLink.Format("%s%s%s%u", pConv->Service(), pConv->Topic(), pLink->Item(), pLink->Format());

				// Find the links' value cache.
				if (!m_oLinksData.Find(strLink, pLinkValue))
				{
					pLinkValue = new CLinkValue(strLink);

					m_oLinksData.Add(strLink, pLinkValue);
				}

				ASSERT(pLinkValue != NULL);

				// Cache links' value.
				pLinkValue->m_oBuffer = oData;

				// Notify DDE Client of advise.
				CDDESvrConv* pConv = static_cast<CDDESvrConv*>(pLink->Conversation());

				pConv->PostLinkUpdate(pLink);
			}
		}
	}
}

/******************************************************************************
** Method:		UpdateStats()
**
** Description:	Update the network stats and indicators.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::UpdateStats()
{
	// 1 second elapsed?
	if ((::GetTickCount() - m_dwTickCount) >= 1000)
	{
		int nConns  = 0;

		// How many server connections?
		for (int i = 0; i < m_aoServices.Size(); ++i)
		{
			if (m_aoServices[i]->m_oConnection.IsOpen())
				nConns++;
		}

		uint nIconID = IDI_NET_IDLE;

		// Which Icon to show?
		if ( (m_nPktsSent > 0) && (m_nPktsRecv > 0) )
			nIconID = IDI_NET_BOTH;
		else if (m_nPktsSent > 0)
			nIconID = IDI_NET_SEND;
		else if (m_nPktsRecv > 0) 
			nIconID = IDI_NET_RECV;
		else if (nConns == 0)
			nIconID = IDI_NET_LOST;

		// Update tray icon.
		if (m_AppWnd.m_oTrayIcon.IsVisible())
			m_AppWnd.m_oTrayIcon.Modify(nIconID);

		// Reset for next update.
		m_nPktsSent   = 0;
		m_nPktsRecv   = 0;
		m_dwTickCount = ::GetTickCount();
	}
}

/******************************************************************************
** Method:		ServerConnect()
**
** Description:	Open a connection to the server.
**
** Parameters:	pService	The service connection to open.
**
** Returns:		Nothing.
**
** Exceptions:	CPipeException.
**
*******************************************************************************
*/

void CNetDDECltApp::ServerConnect(CNetDDEService* pService)
{
	// Ignore, if already open.
	if (pService->m_oConnection.IsOpen())
		return;

	CString strPipeName;

	strPipeName.Format(NETDDE_PIPE_FORMAT, pService->m_oCfg.m_strServer, pService->m_oCfg.m_strPipeName);

	if (m_bTraceNetConns)
		App.Trace("PIPE_STATUS: Connecting to %s", strPipeName);

	// Open the connection to the server
	pService->m_oConnection.Open(strPipeName);

	bool bAccept;

	// Create connect message.
	CBuffer    oBuffer;
	CMemStream oReqStream(oBuffer);

	oReqStream.Create();

	oReqStream << NETDDE_PROTOCOL;
	oReqStream << pService->m_oCfg.m_strService;
	oReqStream << CSysInfo::ComputerName();
	oReqStream << CSysInfo::UserName();

	oReqStream.Close();

	// Send client connect message.
	CNetDDEPacket oPacket(CNetDDEPacket::NETDDE_CLIENT_CONNECT, oBuffer);

	if (m_bTraceNetConns)
		App.Trace("NETDDE_CLIENT_CONNECT: %u %s %s %s", NETDDE_PROTOCOL, pService->m_oCfg.m_strService, CSysInfo::ComputerName(), CSysInfo::UserName());

	pService->m_oConnection.SendPacket(oPacket);

	// Wait for response.
	pService->m_oConnection.WaitForPacket(oPacket, CNetDDEPacket::NETDDE_CLIENT_CONNECT);

	CMemStream oRspStream(oPacket.Buffer());

	oRspStream.Open();
	oRspStream.Seek(sizeof(CNetDDEPacket::Header));

	// Get result.
	oRspStream >> bAccept;

	oRspStream.Close();

	if (!bAccept)
		throw CException(CException::E_GENERIC, "Invalid protocol: %u", NETDDE_PROTOCOL);

	// Update stats.
	++m_nPktsSent;
	++m_nPktsRecv;
}

/******************************************************************************
** Method:		ServerDisconnect()
**
** Description:	Close a connection to the server.
**
** Parameters:	pService	The service connection to close.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::ServerDisconnect(CNetDDEService* pService)
{
	// Ignore, if already closed.
	if (!pService->m_oConnection.IsOpen())
		return;

	try
	{
		CBuffer    oBuffer;
		CMemStream oStream(oBuffer);

		oStream.Create();

		oStream << pService->m_oCfg.m_strService;
		oStream << CSysInfo::ComputerName();

		oStream.Close();

		// Send disconnect message.
		CNetDDEPacket oPacket(CNetDDEPacket::NETDDE_CLIENT_DISCONNECT, oBuffer);

		if (m_bTraceNetConns)
			App.Trace("NETDDE_CLIENT_DISCONNECT: %s %s", pService->m_oCfg.m_strService, CSysInfo::ComputerName());

		pService->m_oConnection.SendPacket(oPacket);

		// Update stats.
		++m_nPktsSent;
	}
	catch (CException& /*e*/)
	{
	}

	// Close the connection.
	pService->m_oConnection.Close();

	// Reset the server conversation handle.
	pService->m_hSvrConv = NULL;
}
