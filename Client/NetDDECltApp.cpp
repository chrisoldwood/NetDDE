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
const char* CNetDDECltApp::VERSION      = "v2.0 [Debug]";
#else
const char* CNetDDECltApp::VERSION      = "v2.0";
#endif

const char* CNetDDECltApp::INI_FILE_VER  = "1.0";
const uint  CNetDDECltApp::BG_TIMER_FREQ =  1000;

const bool  CNetDDECltApp::DEF_TRAY_ICON         = true;
const bool  CNetDDECltApp::DEF_MIN_TO_TRAY       = false;
const uint  CNetDDECltApp::DEF_NET_TIMEOUT       = 30000;
const bool  CNetDDECltApp::DEF_TRACE_CONVS       = true;
const bool  CNetDDECltApp::DEF_TRACE_REQUESTS    = false;
const bool  CNetDDECltApp::DEF_TRACE_ADVISES     = false;
const bool  CNetDDECltApp::DEF_TRACE_UPDATES     = false;
const bool  CNetDDECltApp::DEF_TRACE_NET_CONNS   = true;
const bool  CNetDDECltApp::DEF_TRACE_TO_WINDOW   = true;
const int   CNetDDECltApp::DEF_TRACE_LINES       = 100;
const bool  CNetDDECltApp::DEF_TRACE_TO_FILE     = false;
const char* CNetDDECltApp::DEF_TRACE_FILE        = "NetDDEClient.log";

const uint  CNetDDECltApp::WM_POST_INITIAL_UPDATES = WM_APP + 1;

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
	, m_bPostedAdviseMsg(false)
	, m_nTimerID(0)
	, m_bTrayIcon(DEF_TRAY_ICON)
	, m_bMinToTray(DEF_MIN_TO_TRAY)
	, m_nNetTimeOut(DEF_NET_TIMEOUT)
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
	HWND hPrevWnd = NULL;

	// Only allow a single instance.
	if ((hPrevWnd = ::FindWindow(CAppWnd::WNDCLASS_NAME, NULL)) != NULL)
	{
		// If not visible OR minimised, restore it.
		if (!::IsWindowVisible(hPrevWnd) || ::IsIconic(hPrevWnd))
		{
			::ShowWindow(hPrevWnd, SW_RESTORE);
			::SetForegroundWindow(hPrevWnd);
		}

		return false;
	}

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
		// Initialise WinSock.
		int nResult = CWinSock::Startup(1, 1);

		if (nResult != 0)
		{
			FatalMsg("Failed to initialise WinSock layer: %d.", nResult);
			return false;
		}

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
	if (ShowNormal() && !m_rcLastPos.Empty())
		m_AppWnd.Move(m_rcLastPos);

	m_AppWnd.Show(m_iCmdShow);

	// Update UI.
	m_AppCmds.UpdateUI();

	App.Trace("SERVER_STATUS: Server started");

	// Register DDE services.
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		try
		{
			App.Trace("DDE_STATUS: Registering service: %s [%s]", pService->m_oCfg.m_strLocName, pService->m_oCfg.m_strServer);

			// Register the DDE service name.
			m_pDDEServer->Register(pService->m_oCfg.m_strLocName);
		}
		catch (CException& e)
		{
			AlertMsg("Failed to register DDE service: %s\n\n%s", pService->m_oCfg.m_strLocName, e.ErrorText());
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
		m_pDDEServer->Unregister(pService->m_oCfg.m_strLocName);

		// Disconnect from server.
		ServerDisconnect(pService);

		// Cleanup.
		pService->m_aoNetConvs.DeleteAll();
	}

	// Unnitialise the DDE client.
	m_pDDEServer->RemoveListener(this);
	m_pDDEServer->Uninitialise();

	// Empty the link cache.
	m_oLinkCache.Purge();

	// Terminate WinSock.
	CWinSock::Cleanup();

	// Save settings.
	SaveConfig();
	
	App.Trace("SERVER_STATUS: Server stopped");

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

	// Convert all non-printable chars.
	strMsg.RepCtrlChars();

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
			m_bTraceToFile = false;

			AlertMsg("Failed to write to trace file:\n\n%s", e.ErrorText());
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

	// Read the general settings.
	m_bTrayIcon   = m_oIniFile.ReadBool("Main", "TrayIcon",     m_bTrayIcon  );
	m_bMinToTray  = m_oIniFile.ReadBool("Main", "MinToTray",    m_bMinToTray );
	m_nNetTimeOut = m_oIniFile.ReadInt ("Main", "NetTimeOut",   m_nNetTimeOut);

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
			CString strRemName = m_oIniFile.ReadString(strSection, "RemoteName", "");
			CString strLocName = m_oIniFile.ReadString(strSection, "LocalName",  "");
			CString strServer  = m_oIniFile.ReadString(strSection, "Server",     "");

			// Section valid?
			if ( (strRemName != "") && (strLocName != "") && (strServer != "") )
			{
				// Add to collection.
				CNetDDEService* pService = new CNetDDEService;

				pService->m_oCfg.m_strRemName    = strRemName;
				pService->m_oCfg.m_strLocName    = strLocName;
				pService->m_oCfg.m_strServer     = strServer;
				pService->m_oCfg.m_nServerPort   = m_oIniFile.ReadInt   (strSection, "Port",         NETDDE_PORT_DEFAULT);
				pService->m_oCfg.m_bAsyncAdvises = m_oIniFile.ReadBool  (strSection, "AsyncAdvises", pService->m_oCfg.m_bAsyncAdvises);
				pService->m_oCfg.m_bTextOnly     = m_oIniFile.ReadBool  (strSection, "TextOnly",     pService->m_oCfg.m_bTextOnly    );
				pService->m_oCfg.m_strInitialVal = m_oIniFile.ReadString(strSection, "InitialValue", pService->m_oCfg.m_strInitialVal);
				pService->m_oCfg.m_strFailedVal  = m_oIniFile.ReadString(strSection, "FailedValue",  pService->m_oCfg.m_strFailedVal );
				pService->m_oCfg.m_bReqInitalVal = m_oIniFile.ReadBool  (strSection, "ReqInitValue", pService->m_oCfg.m_bReqInitalVal);

				m_aoServices.Add(pService);

				// Attach event handler.
				pService->m_oConnection.AddClientListener(this);
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

	// Write the service descriptions.
	m_oIniFile.WriteInt("Services", "Count", m_aoServices.Size());

	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		CString strEntry;
		CString strSection = pService->m_oCfg.m_strRemName;

		strEntry.Format("Service[%d]", i);

		m_oIniFile.WriteString("Services", strEntry,       strSection);
		m_oIniFile.WriteString(strSection, "RemoteName",   pService->m_oCfg.m_strRemName   );
		m_oIniFile.WriteString(strSection, "LocalName",    pService->m_oCfg.m_strLocName   );
		m_oIniFile.WriteString(strSection, "Server",       pService->m_oCfg.m_strServer    );
		m_oIniFile.WriteInt   (strSection, "Port",         pService->m_oCfg.m_nServerPort  );
		m_oIniFile.WriteBool  (strSection, "AsyncAdvises", pService->m_oCfg.m_bAsyncAdvises);
		m_oIniFile.WriteBool  (strSection, "TextOnly",     pService->m_oCfg.m_bTextOnly    );
		m_oIniFile.WriteString(strSection, "InitialValue", pService->m_oCfg.m_strInitialVal);
		m_oIniFile.WriteString(strSection, "FailedValue",  pService->m_oCfg.m_strFailedVal );
		m_oIniFile.WriteBool  (strSection, "ReqInitValue", pService->m_oCfg.m_bReqInitalVal);
	}

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

	// Write the general settings.
	m_oIniFile.WriteBool("Main", "TrayIcon",     m_bTrayIcon  );
	m_oIniFile.WriteBool("Main", "MinToTray",    m_bMinToTray );
	m_oIniFile.WriteInt ("Main", "NetTimeOut",   m_nNetTimeOut);

	// Write the window pos and size.
	m_oIniFile.WriteInt("UI", "Left",   m_rcLastPos.left  );
	m_oIniFile.WriteInt("UI", "Top",    m_rcLastPos.top   );
	m_oIniFile.WriteInt("UI", "Right",  m_rcLastPos.right );
	m_oIniFile.WriteInt("UI", "Bottom", m_rcLastPos.bottom);
}

/******************************************************************************
** Method:		FindService()
**
** Description:	Finds the NetDDEService by its local name.
**
** Parameters:	pszService	The local service name.
**
** Returns:		The service or NULL.
**
*******************************************************************************
*/

CNetDDEService* CNetDDECltApp::FindService(const char* pszService) const
{
	// For all services...
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		// Matches local service name?
		if (pService->m_oCfg.m_strLocName == pszService)
			return pService;
	}

	return NULL;
}

/******************************************************************************
** Method:		FindService()
**
** Description:	Finds the NetDDEService by the server side conversation handle.
**
** Parameters:	hSvrConv	The server side conversation handle.
**
** Returns:		The service or NULL.
**
*******************************************************************************
*/

CNetDDEService* CNetDDECltApp::FindService(HCONV hSvrConv) const
{
	// For all services...
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		// For all service conversations.
		for (int j = 0; j < pService->m_aoNetConvs.Size(); ++j)
		{
			CNetDDEConv* pNetConv = pService->m_aoNetConvs[j];

			// Matches?
			if (pNetConv->m_hSvrConv == hSvrConv)
				return pService;
		}
	}

	return NULL;
}

/******************************************************************************
** Method:		FindService()
**
** Description:	Finds the NetDDEService by the client side conversation.
**
** Parameters:	pConv	The client side conversation.
**
** Returns:		The service or NULL.
**
*******************************************************************************
*/

CNetDDEService* CNetDDECltApp::FindService(CDDESvrConv* pConv) const
{
	// For all services...
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		// For all service conversations.
		for (int j = 0; j < pService->m_aoNetConvs.Size(); ++j)
		{
			CNetDDEConv* pNetConv = pService->m_aoNetConvs[j];

			// Matches?
			if (pNetConv->m_pCltConv == pConv)
				return pService;
		}
	}

	return NULL;
}

/******************************************************************************
** Method:		Disconnect()
**
** Description:	Terminate the client-side DDE conversation prematurely.
**
** Parameters:	pConv	The client side conversation.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::Disconnect(CDDESvrConv* pConv)
{
	ASSERT(pConv != NULL);

	// Fake a client-side disconnection.
	OnDisconnect(pConv);

	// Cleanup.
	m_pDDEServer->DestroyConversation(pConv);
}

/******************************************************************************
** Methods:		OnWildConnect*()
**
** Description:	Multiple connection request from a DDE Client. The DDE Client
**				will create a connection for each returned service & topic.
**
** Parameters:	pszService		The service name.
**				pszTopic		The topic name.
**				astrServices	The returned list of service names.
**				astrTopics		The returned list of topic names.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDECltApp::OnWildConnect(CStrArray& /*astrServices*/, CStrArray& /*astrTopics*/)
{
	if (m_bTraceConvs)
		App.Trace("DDE_WILDCONNECT (ignored)");

	return false;
}

bool CNetDDECltApp::OnWildConnectService(const char* pszService, CStrArray& /*astrTopics*/)
{
	ASSERT(pszService != NULL);

	if (m_bTraceConvs)
		App.Trace("DDE_WILDCONNECT_SERVICE: %s (ignored)", pszService);

	return false;
}

bool CNetDDECltApp::OnWildConnectTopic(const char* pszTopic, CStrArray& /*astrServices*/)
{
	ASSERT(pszTopic != NULL);

	if (m_bTraceConvs)
		App.Trace("DDE_WILDCONNECT_TOPIC: %s (ignored)", pszTopic);

	return false;
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
			if (pService->m_aoNetConvs.Size() == 0)
				ServerConnect(pService);

			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream << pService->m_oCfg.m_strRemName;
			oReqStream << pszTopic;

			oReqStream.Close();

			CNetDDEPacket oReqPacket(CNetDDEPacket::DDE_CREATE_CONVERSATION, oBuffer);
			CNetDDEPacket oRspPacket;

			if (m_bTraceConvs)
				App.Trace("DDE_CREATE_CONVERSATION: %s %s", pszService, pszTopic);

			// Send it.
			pService->m_oConnection.SendPacket(oReqPacket);

			// Wait for response.
			pService->m_oConnection.ReadResponsePacket(oRspPacket, oReqPacket.PacketID());

			HCONV  hSvrConv;
			uint32 nConvID;

			CMemStream oRspStream(oRspPacket.Buffer());

			oRspStream.Open();
			oRspStream.Seek(sizeof(CNetDDEPacket::Header));

			// Get result.
			oRspStream >> bAccept;
			oRspStream.Read(&hSvrConv, sizeof(HCONV));
			oRspStream >> nConvID;

			oRspStream.Close();

			// If accepted, attach to service.
			if (bAccept)
				pService->m_aoNetConvs.Add(new CNetDDEConv(hSvrConv, nConvID));
		}
		catch (CSocketException& e)
		{
			App.Trace("SOCKET_ERROR: %s", e.ErrorText());

			CloseConnection(pService);
		}

		// Close connection, if last conversation.
		if (pService->m_aoNetConvs.Size() == 0)
			ServerDisconnect(pService);

		// Update stats.
		++m_nPktsSent;
		++m_nPktsRecv;
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

	// Set the client side of the NetDDE conversation.
	for (int i = 0; i < pService->m_aoNetConvs.Size(); ++i)
	{
		CNetDDEConv* pNetConv = pService->m_aoNetConvs[i];

		// Server side handle was set in OnConnect().
		if (pNetConv->m_pCltConv == NULL)
		{
			pNetConv->m_pCltConv = pConv;
			break;
		}
	}
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

	ASSERT(pService != NULL);

	// Valid service name?
	if (pService != NULL)
	{
		CNetDDEConv* pNetConv = pService->FindNetConv(pConv);

		ASSERT(pNetConv != NULL);

		try
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pNetConv->m_hSvrConv, sizeof(HCONV));
			oReqStream << pNetConv->m_nSvrConvID;

			oReqStream.Close();

			CNetDDEPacket oPacket(CNetDDEPacket::DDE_DESTROY_CONVERSATION, oBuffer);

			if (m_bTraceConvs)
				App.Trace("DDE_DESTROY_CONVERSATION: %s %s", pConv->Service(), pConv->Topic());

			// Send it.
			pService->m_oConnection.SendPacket(oPacket);
		}
		catch (CSocketException& e)
		{
			App.Trace("SOCKET_ERROR: %s", e.ErrorText());

			CloseConnection(pService);
		}

		// Remove from NetDDE conversation list.
		pService->m_aoNetConvs.Delete(pService->m_aoNetConvs.Find(pNetConv));

		// Close connection, if last conversation.
		if (pService->m_aoNetConvs.Size() == 0)
			ServerDisconnect(pService);

		// Update stats.
		++m_nPktsSent;
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
		// Service only supports CF_TEXT?
		if ( (pService->m_oCfg.m_bTextOnly) && (nFormat != CF_TEXT) )
			return false;

		CDDELink* pLink = pConv->FindLink(pszItem, nFormat);

		// Already linked to item?
		if (pLink != NULL)
		{
			// Service request using link cache.
			CLinkValue* pValue = m_oLinkCache.Find(pConv, pLink);

			ASSERT(pValue != NULL);

			oData.SetBuffer(pValue->m_oLastValue);

			if (m_bTraceRequests)
				App.Trace("DDE_REQUEST: %s %s %s %s [%s]", pConv->Service(), pConv->Topic(), pszItem, CClipboard::FormatName(nFormat), oData.GetString());

			return true;
		}

		CNetDDEConv* pNetConv = pService->FindNetConv(pConv);

		ASSERT(pNetConv != NULL);

		try
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pNetConv->m_hSvrConv, sizeof(HCONV));
			oReqStream << pNetConv->m_nSvrConvID;
			oReqStream << pszItem;
			oReqStream << (uint32) nFormat;

			oReqStream.Close();

			CNetDDEPacket oReqPacket(CNetDDEPacket::DDE_REQUEST, oBuffer);
			CNetDDEPacket oRspPacket;

			if (m_bTraceRequests)
				App.Trace("DDE_REQUEST: %s %s %s %s", pConv->Service(), pConv->Topic(), pszItem, CClipboard::FormatName(nFormat));

			// Send it.
			pService->m_oConnection.SendPacket(oReqPacket);

			// Wait for response.
			pService->m_oConnection.ReadResponsePacket(oRspPacket, oReqPacket.PacketID());

			CBuffer    oDDEData;
			CMemStream oRspStream(oRspPacket.Buffer());

			oRspStream.Open();
			oRspStream.Seek(sizeof(CNetDDEPacket::Header));

			// Get result.
			oRspStream >> bResult;
			oRspStream >> oDDEData;

			oRspStream.Close();

			if (bResult)
				oData.SetBuffer(oDDEData);
		}
		catch (CSocketException& e)
		{
			App.Trace("SOCKET_ERROR: %s", e.ErrorText());

			CloseConnection(pService);
		}

		// Update stats.
		++m_nPktsSent;
		++m_nPktsRecv;
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
		// Service only supports CF_TEXT?
		if ( (pService->m_oCfg.m_bTextOnly) && (nFormat != CF_TEXT) )
			return false;

		CNetDDEConv* pNetConv = pService->FindNetConv(pConv);

		ASSERT(pNetConv != NULL);

		try
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pNetConv->m_hSvrConv, sizeof(HCONV));
			oReqStream << pNetConv->m_nSvrConvID;
			oReqStream << pszItem;
			oReqStream << (uint32) nFormat;
			oReqStream << pService->m_oCfg.m_bAsyncAdvises;
			oReqStream << pService->m_oCfg.m_bReqInitalVal;

			oReqStream.Close();

			CNetDDEPacket oReqPacket(CNetDDEPacket::DDE_START_ADVISE, oBuffer);
			CNetDDEPacket oRspPacket;

			if (App.m_bTraceAdvises)
				App.Trace("DDE_START_ADVISE: %s %s %s %s", pConv->Service(), pConv->Topic(), pszItem, CClipboard::FormatName(nFormat));

			// Send it.
			pService->m_oConnection.SendPacket(oReqPacket);

			// Expecting response?
			if (!pService->m_oCfg.m_bAsyncAdvises)
			{
				// Wait for response.
				pService->m_oConnection.ReadResponsePacket(oRspPacket, oReqPacket.PacketID());

				CMemStream oRspStream(oRspPacket.Buffer());

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
		}
		catch (CSocketException& e)
		{
			App.Trace("SOCKET_ERROR: %s", e.ErrorText());

			CloseConnection(pService);
		}

		// Update stats.
		++m_nPktsSent;
		++m_nPktsRecv;
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
	CNetDDEService* pService = FindService(pConv);

	ASSERT(pService != NULL);

	CNetDDEConv* pNetConv = pService->FindNetConv(pConv);

	ASSERT(pNetConv != NULL);

	// Add link to service links' list.
	pNetConv->m_aoLinks.Add(pLink);

	// Set the initial link value.
	if ((m_oLinkCache.Find(pConv, pLink)) == NULL)
		m_oLinkCache.Create(pConv, pLink, pService->m_oCfg.m_strInitialVal);

	// If we're not requesting an inital value, send the default one.
	if (!pService->m_oCfg.m_bReqInitalVal)
	{							  
		pNetConv->m_oNewLinks.push_back(pLink);

		if (!m_bPostedAdviseMsg)
		{
			m_MainThread.PostMessage(WM_POST_INITIAL_UPDATES);
			m_bPostedAdviseMsg = true;
		}
	}
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
	// Fetch link data from cache.
	CLinkValue* pLinkValue = m_oLinkCache.Find(pConv, pLink);

	ASSERT(pLinkValue != NULL);

	oData.SetBuffer(pLinkValue->m_oLastValue);

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
		CNetDDEConv* pNetConv = pService->FindNetConv(pConv);

		ASSERT(pNetConv != NULL);

		try
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pNetConv->m_hSvrConv, sizeof(HCONV));
			oReqStream << pNetConv->m_nSvrConvID;
			oReqStream << pLink->Item();
			oReqStream << (uint32) pLink->Format();

			oReqStream.Close();

			CNetDDEPacket oPacket(CNetDDEPacket::DDE_STOP_ADVISE, oBuffer);

			if (App.m_bTraceAdvises)
				App.Trace("DDE_STOP_ADVISE: %s %s %s %s", pConv->Service(), pConv->Topic(), pLink->Item(), CClipboard::FormatName(pLink->Format()));

			// Send it.
			pService->m_oConnection.SendPacket(oPacket);
		}
		catch (CSocketException& e)
		{
			App.Trace("SOCKET_ERROR: %s", e.ErrorText());

			CloseConnection(pService);
		}

		// Remove from connections links list.
		pNetConv->m_aoLinks.Remove(pNetConv->m_aoLinks.Find(pLink));

		// Update stats.
		++m_nPktsSent;
	}
}

/******************************************************************************
** Method:		OnExecute()
**
** Description:	Execute a command.
**
** Parameters:	pConv		The conversation.
**				oData		The command.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDECltApp::OnExecute(CDDESvrConv* pConv, const CString& strCmd)
{
	bool bResult = false;

	CNetDDEService* pService = FindService(pConv->Service());

	// Valid service name?
	if ( (pService != NULL) && (pService->m_oConnection.IsOpen()) )
	{
		CNetDDEConv* pNetConv = pService->FindNetConv(pConv);

		ASSERT(pNetConv != NULL);

		try
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pNetConv->m_hSvrConv, sizeof(HCONV));
			oReqStream << pNetConv->m_nSvrConvID;
			oReqStream << strCmd;

			oReqStream.Close();

			CNetDDEPacket oReqPacket(CNetDDEPacket::DDE_EXECUTE, oBuffer);
			CNetDDEPacket oRspPacket;

			if (App.m_bTraceRequests)
				App.Trace("DDE_EXECUTE: %s %s [%s]", pConv->Service(), pConv->Topic(), strCmd);

			// Send it.
			pService->m_oConnection.SendPacket(oReqPacket);

			// Wait for response.
			pService->m_oConnection.ReadResponsePacket(oRspPacket, oReqPacket.PacketID());

			CMemStream oRspStream(oRspPacket.Buffer());

			oRspStream.Open();
			oRspStream.Seek(sizeof(CNetDDEPacket::Header));

			// Get result.
			oRspStream >> bResult;

			oRspStream.Close();
		}
		catch (CSocketException& e)
		{
			App.Trace("SOCKET_ERROR: %s", e.ErrorText());

			CloseConnection(pService);
		}

		// Update stats.
		++m_nPktsSent;
		++m_nPktsRecv;
	}

	return bResult;
}

/******************************************************************************
** Method:		OnPoke()
**
** Description:	Poke a value into an item.
**
** Parameters:	pConv		The conversation.
**				pszItem		The item being poked.
**				nFormat		The data format.
**				oData		The data.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDECltApp::OnPoke(CDDESvrConv* pConv, const char* pszItem, uint nFormat, const CDDEData& oData)
{
	// Custom formats not supported.
	if (nFormat >= 0xC000)
		return false;

	bool bResult = false;

	CNetDDEService* pService = FindService(pConv->Service());

	// Valid service name?
	if ( (pService != NULL) && (pService->m_oConnection.IsOpen()) )
	{
		// Service only supports CF_TEXT?
		if ( (pService->m_oCfg.m_bTextOnly) && (nFormat != CF_TEXT) )
			return false;

		CNetDDEConv* pNetConv = pService->FindNetConv(pConv);

		ASSERT(pNetConv != NULL);

		try
		{
			// Create conversation message.
			CBuffer    oBuffer;
			CMemStream oReqStream(oBuffer);

			oReqStream.Create();

			oReqStream.Write(&pNetConv->m_hSvrConv, sizeof(HCONV));
			oReqStream << pNetConv->m_nSvrConvID;
			oReqStream << pszItem;
			oReqStream << (uint32) nFormat;
			oReqStream << oData.GetBuffer();

			oReqStream.Close();

			CNetDDEPacket oReqPacket(CNetDDEPacket::DDE_POKE, oBuffer);
			CNetDDEPacket oRspPacket;

			if (App.m_bTraceRequests)
				App.Trace("DDE_POKE: %s %s %s %s [%s]", pConv->Service(), pConv->Topic(), pszItem, CClipboard::FormatName(nFormat), oData.GetString());

			// Send it.
			pService->m_oConnection.SendPacket(oReqPacket);

			// Wait for response.
			pService->m_oConnection.ReadResponsePacket(oRspPacket, oReqPacket.PacketID());

			CMemStream oRspStream(oRspPacket.Buffer());

			oRspStream.Open();
			oRspStream.Seek(sizeof(CNetDDEPacket::Header));

			// Get result.
			oRspStream >> bResult;

			oRspStream.Close();
		}
		catch (CSocketException& e)
		{
			App.Trace("SOCKET_ERROR: %s", e.ErrorText());

			CloseConnection(pService);
		}

		// Update stats.
		++m_nPktsSent;
		++m_nPktsRecv;
	}

	return bResult;
}

/******************************************************************************
** Method:		OnReadReady()
**
** Description:	Data received on the socket.
**
** Parameters:	pSocket		The socket.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnReadReady(CSocket* pSocket)
{
	CNetDDECltSocket* pConnection = static_cast<CNetDDECltSocket*>(pSocket);
	CNetDDEService*   pService    = pConnection->Service();

	try
	{
		CNetDDEPacket oPacket; 

		// For all packets...
		while (pService->m_oConnection.RecvPacket(oPacket))
		{
			// Notification packet?
			if ((oPacket.DataType() & CNetDDEPacket::PACKET_SYNC_MASK) == CNetDDEPacket::ASYNC_PACKET)
			{
				ASSERT(oPacket.PacketID() == CNetDDEPacket::ASYNC_PACKET_ID);

				// Decode packet type.
				switch (oPacket.DataType())
				{
					case CNetDDEPacket::NETDDE_SERVER_DISCONNECT:	OnNetDDEServerDisconnect(*pService, oPacket);	break;
					case CNetDDEPacket::DDE_DISCONNECT:				OnDDEDisconnect(*pService, oPacket);			break;
					case CNetDDEPacket::DDE_ADVISE:					OnDDEAdvise(*pService, oPacket);				break;
					case CNetDDEPacket::DDE_START_ADVISE_FAILED:	OnDDEStartFailed(*pService, oPacket);			break;
					default:										ASSERT_FALSE();									break;
				}

				// Update stats.
				++m_nPktsRecv;

				// If disconnect received, stop polling.
				if (!pService->m_oConnection.IsOpen())
					break;
			}
			// Response packet.
			else
			{
				ASSERT(oPacket.PacketID() != CNetDDEPacket::ASYNC_PACKET_ID);

				// Append to response queue.
				pService->m_oConnection.QueueResponsePacket(new CNetDDEPacket(oPacket));
			}
		}
	}
	catch (CSocketException& e)
	{
		App.Trace("SOCKET_ERROR: %s", e.ErrorText());

		CloseConnection(pService);
	}
}

/******************************************************************************
** Method:		OnClosed()
**
** Description:	The socket was closed by the remote end.
**
** Parameters:	pSocket		The socket.
**				nReason		The reason for closure.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnClosed(CSocket* pSocket, int /*nReason*/)
{
	CNetDDECltSocket* pConnection = static_cast<CNetDDECltSocket*>(pSocket);
	CNetDDEService*   pService    = pConnection->Service();

	// Outstanding conversations?
	if (pService->m_aoNetConvs.Size() > 0)
	{
		for (int j = 0; j < pService->m_aoNetConvs.Size(); ++j)
		{
			CNetDDEConv* pNetConv = pService->m_aoNetConvs[j];

			// Disconnect from DDE client.
			m_pDDEServer->DestroyConversation(pNetConv->m_pCltConv);
		}

		// Discard NetDDE conversations.
		pService->m_aoNetConvs.DeleteAll();
	}

	// Flush link cache, if all connections closed.
	if ( (m_pDDEServer->GetNumConversations() == 0) && (m_oLinkCache.Size() > 0) )
		m_oLinkCache.Purge();
}

/******************************************************************************
** Method:		OnError()
**
** Description:	An error has occured on the socket.
**
** Parameters:	pSocket		The socket.
**				nEvent		The event that caused the error.
**				nError		The error code.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnError(CSocket* /*pSocket*/, int nEvent, int nError)
{
	Trace("SOCKET_ERROR: %s [%s]", CWinSock::ErrorToSymbol(nError), CSocket::AsyncEventStr(nEvent));
}

/******************************************************************************
** Method:		OnTimer()
**
** Description:	The timer has gone off, process background tasks.
**
** Parameters:	nTimerID	The timer ID.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnTimer(uint /*nTimerID*/)
{
	UpdateStats();
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

void CNetDDECltApp::OnNetDDEServerDisconnect(CNetDDEService& oService, CNetDDEPacket& /*oNfyPacket*/)
{
	if (m_bTraceNetConns)
		App.Trace("NETDDE_SERVER_DISCONNECT: %s", oService.m_oCfg.m_strServer);

	// Close connection to server.
	oService.m_oConnection.Close();

	// Cleanup.
	OnClosed(&oService.m_oConnection, 0);
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

void CNetDDECltApp::OnDDEDisconnect(CNetDDEService& /*oService*/, CNetDDEPacket& oNfyPacket)
{
	ASSERT(oNfyPacket.DataType() == CNetDDEPacket::DDE_DISCONNECT);

	HCONV hSvrConv;

	CMemStream oStream(oNfyPacket.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	// Get conversation parameters.
	oStream.Read(&hSvrConv, sizeof(HCONV));

	oStream.Close();

	// Find the service for the conversation handle.
	CNetDDEService* pService = FindService(hSvrConv);

	if (pService != NULL)
	{
		if (m_bTraceConvs)
			App.Trace("DDE_DISCONNECT: %s", pService->m_oCfg.m_strRemName);

		// Cleanup all client conversations...
		for (int i = pService->m_aoNetConvs.Size()-1; i >= 0; --i)
		{
			CNetDDEConv* pNetConv = pService->m_aoNetConvs[i];

			// Is client-side NetDDE conversation?
			if (pNetConv->m_hSvrConv == hSvrConv)
			{
				// Disconnect from DDE client.
				m_pDDEServer->DestroyConversation(pNetConv->m_pCltConv);

				pService->m_aoNetConvs.Delete(i);
			}
		}

		// Close connection, if last conversation.
		if (pService->m_aoNetConvs.Size() == 0)
			ServerDisconnect(pService);
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

	HCONV   hSvrConv;
	CString strItem;
	uint32  nFormat;
	CBuffer oData;
	bool    bEoP = false;

	CMemStream oStream(oNfyPacket.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	// For all updates...
	while (!bEoP)
	{
		// Get advise parameters.
		oStream.Read(&hSvrConv, sizeof(HCONV));
		oStream >> strItem;
		oStream >> nFormat;
		oStream >> oData;
		oStream >> bEoP;

		ASSERT(bEoP);

		if (App.m_bTraceUpdates)
		{
			CString strData;

			if (nFormat == CF_TEXT)
				strData = oData.ToString();
			else
				strData = CClipboard::FormatName(nFormat);

			App.Trace("DDE_ADVISE: %s %s [%s]", oService.m_oCfg.m_strLocName, strItem, strData);
		}

		// Find the service for the conversation handle.
		CNetDDEService* pService = FindService(hSvrConv);

		if (pService != NULL)
		{
			// For all NetDDE conversations...
			for (int j = 0; j < pService->m_aoNetConvs.Size(); ++j)
			{
				CNetDDEConv* pNetConv = pService->m_aoNetConvs[j];

				// Ignore, if different conversation.
				if (pNetConv->m_hSvrConv != hSvrConv)
					continue;

				// For all links....
				for (int i = 0; i < pNetConv->m_aoLinks.Size(); ++i)
				{
					CDDELink* pLink = pNetConv->m_aoLinks[i];
					CDDEConv* pConv = pLink->Conversation();

					// Post advise, if it's the link that has been updated.
					if ( (pLink->Item() == strItem) && (pLink->Format() == nFormat) )
					{
						CLinkValue* pValue = NULL;

						// Find the links' value cache.
						if ((pValue = m_oLinkCache.Find(pConv, pLink)) == NULL)
							pValue = m_oLinkCache.Create(pConv, pLink);

						ASSERT(pValue != NULL);

						// Update links' value.
						pValue->m_oLastValue  = oData;
						pValue->m_tLastUpdate = CDateTime::Current();

						// Notify DDE Client of advise.
						CDDESvrConv* pConv = static_cast<CDDESvrConv*>(pLink->Conversation());

						pConv->PostLinkUpdate(pLink);
					}
				}
			}
		}
	}

	oStream.Close();
}

/******************************************************************************
** Method:		OnDDEStartFailed()
**
** Description:	Async DDE Start Advise failed.
**
** Parameters:	oService		The service the packet came from.
**				oNfyPacket		The notification packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnDDEStartFailed(CNetDDEService& oService, CNetDDEPacket& oNfyPacket)
{
	ASSERT(oNfyPacket.DataType() == CNetDDEPacket::DDE_START_ADVISE_FAILED);

	HCONV   hSvrConv;
	CString strItem;
	uint32  nFormat;
	bool    bEoP = false;

	CMemStream oStream(oNfyPacket.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	// For all updates...
	while (!bEoP)
	{
		// Get advise parameters.
		oStream.Read(&hSvrConv, sizeof(HCONV));
		oStream >> strItem;
		oStream >> nFormat;
		oStream >> bEoP;

		if (App.m_bTraceAdvises)
			App.Trace("DDE_START_ADVISE_FAILED: %s %s", oService.m_oCfg.m_strLocName, strItem);

		// Find the service for the conversation handle.
		CNetDDEService* pService = FindService(hSvrConv);

		if (pService != NULL)
		{
			// For all NetDDE conversations...
			for (int j = 0; j < pService->m_aoNetConvs.Size(); ++j)
			{
				CNetDDEConv* pNetConv = pService->m_aoNetConvs[j];

				// Ignore, if different conversation.
				if (pNetConv->m_hSvrConv != hSvrConv)
					continue;

				// For all links....
				for (int i = 0; i < pNetConv->m_aoLinks.Size(); ++i)
				{
					CDDELink* pLink = pNetConv->m_aoLinks[i];
					CDDEConv* pConv = pLink->Conversation();

					// Post advise, if it's the link that has been updated.
					if ( (pLink->Item() == strItem) && (pLink->Format() == nFormat) )
					{
						CLinkValue* pValue = NULL;

						// Find the links' value cache.
						if ((pValue = m_oLinkCache.Find(pConv, pLink)) == NULL)
							pValue = m_oLinkCache.Create(pConv, pLink);

						ASSERT(pValue != NULL);

						// Update links' value.
						pValue->m_oLastValue.FromString(pService->m_oCfg.m_strFailedVal);
						pValue->m_tLastUpdate = CDateTime::Current();

						// Notify DDE Client of advise.
						CDDESvrConv* pConv = static_cast<CDDESvrConv*>(pLink->Conversation());

						pConv->PostLinkUpdate(pLink);
					}
				}
			}
		}
	}

	oStream.Close();
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
		int nConns = 0;

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

		// Format tooltip.
		CString strTip = "NetDDE Client";

		if (nConns > 0)
		{
			strTip += "\nConnections: "   + CStrCvt::FormatInt(nConns);
			strTip += "\nConversations: " + CStrCvt::FormatInt(m_pDDEServer->GetNumConversations());
		}

		// Update tray icon.
		if (m_AppWnd.m_oTrayIcon.IsVisible())
			m_AppWnd.m_oTrayIcon.Modify(nIconID, strTip);

		// Reset for next update.
		m_nPktsSent   = 0;
		m_nPktsRecv   = 0;
		m_dwTickCount = ::GetTickCount();
	}
}

/******************************************************************************
** Method:		OnPostInitalUpdates()
**
** Description:	Post inital values for all new DDE links.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::OnPostInitalUpdates()
{
	// For all services...
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDEService* pService = m_aoServices[i];

		// For all NetDDE conversations...
		for (int j = 0; j < pService->m_aoNetConvs.Size(); ++j)
		{
			// Template shorthands.
			typedef CNetDDEConv::CLinkList::const_iterator CIter;

			CNetDDEConv* pNetConv = pService->m_aoNetConvs[j];

			// Post update on all new links...
			for (CIter oIter = pNetConv->m_oNewLinks.begin(); oIter != pNetConv->m_oNewLinks.end(); ++oIter)
			{
				CDDELink*    pLink = *oIter;
				CDDESvrConv* pConv = static_cast<CDDESvrConv*>(pLink->Conversation());

				if (!pConv->PostLinkUpdate(pLink))
				{
					TRACE3("PostLinkUpdate('%s|%s', '%s') - Failed\n", pConv->Service(), pConv->Topic(), pLink->Item());
				}
			}

			// Clear new links list.
			pNetConv->m_oNewLinks.clear();
		}
	}

	// Reset 'posted' flag.
	m_bPostedAdviseMsg = false;
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
** Exceptions:	CSocketException.
**
*******************************************************************************
*/

void CNetDDECltApp::ServerConnect(CNetDDEService* pService)
{
	// Ignore, if already open.
	if (pService->m_oConnection.IsOpen())
		return;

	if (m_bTraceNetConns)
		App.Trace("SOCKET_STATUS: Connecting to %s:%u", pService->m_oCfg.m_strServer, pService->m_oCfg.m_nServerPort);

	// Open the connection to the server
	pService->m_oConnection.Connect(pService->m_oCfg.m_strServer, pService->m_oCfg.m_nServerPort);

	bool bAccept;

	// Create connect message.
	CBuffer    oBuffer;
	CMemStream oReqStream(oBuffer);

	oReqStream.Create();

	oReqStream << NETDDE_PROTOCOL;
	oReqStream << pService->m_oCfg.m_strRemName;
	oReqStream << CSysInfo::ComputerName();
	oReqStream << CSysInfo::UserName();
	oReqStream << CPath::Application().FileName();
	oReqStream << CString(App.VERSION);

	oReqStream.Close();

	// Send client connect message.
	CNetDDEPacket oReqPacket(CNetDDEPacket::NETDDE_CLIENT_CONNECT, oBuffer);
	CNetDDEPacket oRspPacket;

	if (m_bTraceNetConns)
		App.Trace("NETDDE_CLIENT_CONNECT: %u %s %s %s", NETDDE_PROTOCOL, pService->m_oCfg.m_strRemName, CSysInfo::ComputerName(), CSysInfo::UserName());

	pService->m_oConnection.SendPacket(oReqPacket);

	// Wait for response.
	pService->m_oConnection.ReadResponsePacket(oRspPacket, oReqPacket.PacketID());

	CString    strVersion;
	CMemStream oRspStream(oRspPacket.Buffer());

	oRspStream.Open();
	oRspStream.Seek(sizeof(CNetDDEPacket::Header));

	// Get result.
	oRspStream >> bAccept;
	oRspStream >> strVersion;

	oRspStream.Close();

	if (m_bTraceNetConns)
		App.Trace("NETDDE_SERVER_VERSION: %s", strVersion);

	if (!bAccept)
		throw CSocketException(CSocketException::E_BAD_PROTOCOL, 0);

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

		oStream << pService->m_oCfg.m_strRemName;
		oStream << CSysInfo::ComputerName();

		oStream.Close();

		// Send disconnect message.
		CNetDDEPacket oPacket(CNetDDEPacket::NETDDE_CLIENT_DISCONNECT, oBuffer);

		if (m_bTraceNetConns)
			App.Trace("NETDDE_CLIENT_DISCONNECT: %s %s", pService->m_oCfg.m_strRemName, CSysInfo::ComputerName());

		pService->m_oConnection.SendPacket(oPacket);

		// Update stats.
		++m_nPktsSent;
	}
	catch (CException& /*e*/)
	{
	}

	// Close the connection.
	pService->m_oConnection.Close();
}

/******************************************************************************
** Method:		OnThreadMsg()
**
** Description:	Message handler for general thread messages.
**
** Parameters:	Standard thread message parameters.
**
** Returns:		Nothing.
**
*******************************************************************************
*/
void CNetDDECltApp::OnThreadMsg(UINT nMsg, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	// Forward to real handler.
	if (nMsg == WM_POST_INITIAL_UPDATES)
		OnPostInitalUpdates();
}

/******************************************************************************
** Method:		CloseConnection()
**
** Description:	Close the connection to the server.
**
** Parameters:	pService		The service to terminate.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltApp::CloseConnection(CNetDDEService* pService)
{
	// Graceful close, if possible.
	ServerDisconnect(pService);

	// Cleanup.
	OnClosed(&pService->m_oConnection, 0);
}
