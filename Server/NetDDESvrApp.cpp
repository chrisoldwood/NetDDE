/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESVRAPP.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CNetDDESvrApp class definition.
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
CNetDDESvrApp App;

/******************************************************************************
**
** Class members.
**
*******************************************************************************
*/

#ifdef _DEBUG
const char* CNetDDESvrApp::VERSION      = "v1.2 [Debug]";
#else
const char* CNetDDESvrApp::VERSION      = "v1.2";
#endif

const char* CNetDDESvrApp::INI_FILE_VER  = "1.0";
const uint  CNetDDESvrApp::BG_TIMER_FREQ =  1;

const bool  CNetDDESvrApp::DEF_TRAY_ICON         = true;
const bool  CNetDDESvrApp::DEF_MIN_TO_TRAY       = false;
const bool  CNetDDESvrApp::DEF_TRACE_CONVS       = true;
const bool  CNetDDESvrApp::DEF_TRACE_REQUESTS    = false;
const bool  CNetDDESvrApp::DEF_TRACE_ADVISES     = false;
const bool  CNetDDESvrApp::DEF_TRACE_UPDATES     = false;
const bool  CNetDDESvrApp::DEF_TRACE_NET_CONNS   = true;
const bool  CNetDDESvrApp::DEF_TRACE_TO_WINDOW   = true;
const int   CNetDDESvrApp::DEF_TRACE_LINES       = 100;
const bool  CNetDDESvrApp::DEF_TRACE_TO_FILE     = false;
const char* CNetDDESvrApp::DEF_TRACE_FILE        = "NetDDEServer.log";

// Background processing re-entrancy flag.
bool CNetDDESvrApp::g_bInBgProcessing = false;

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

CNetDDESvrApp::CNetDDESvrApp()
	: CApp(m_AppWnd, m_AppCmds)
	, m_pDDEClient(CDDEClient::Instance())
	, m_pConnection(NULL)
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

CNetDDESvrApp::~CNetDDESvrApp()
{
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

bool CNetDDESvrApp::OnOpen()
{
	// Set the app title.
	m_strTitle = "NetDDE Server";

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
		// Initialise the DDE client.
		m_pDDEClient->Initialise();
		m_pDDEClient->AddListener(this);
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

bool CNetDDESvrApp::OnClose()
{
	// Start the background timer.
	StopTimer(m_nTimerID);

	// Close the waiting connection, if one.
	if (m_pConnection != NULL)
		delete m_pConnection;

	// Close all client connections...
	for (int i = 0; i < m_aoConnections.Size(); ++i)
	{
		try
		{
			CNetDDESvrPipe* pConnection = m_aoConnections[i]; 

			if (pConnection->IsOpen())
			{
				CConvs aoConvs;

				// Delete the connections conversation list.
				pConnection->GetConversations(aoConvs);

				for (int j = 0; j < aoConvs.Size(); ++j)
					m_pDDEClient->DestroyConversation(aoConvs[j]);

				if (App.m_bTraceNetConns)
					App.Trace("NETDDE_SERVER_DISCONNECT:");

				// Send disconnect message.
				CNetDDEPacket oPacket(CNetDDEPacket::NETDDE_SERVER_DISCONNECT, NULL, 0);

				pConnection->SendPacket(oPacket);

				// Update stats.
				++m_nPktsSent;
			}
		}
		catch (CException& /*e*/)
		{
		}
	}

	// Close all client connections.
	m_aoConnections.DeleteAll();

	// Unnitialise the DDE client.
	m_pDDEClient->RemoveListener(this);
	m_pDDEClient->Uninitialise();

	// Empty the link cache.
	m_oLinkCache.Clear();

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

void CNetDDESvrApp::Trace(const char* pszMsg, ...)
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

void CNetDDESvrApp::LoadConfig()
{
	// Read the file version.
	CString strVer = m_oIniFile.ReadString("Version", "Version", INI_FILE_VER);

	// Read the pipe name.
	m_strPipeName = m_oIniFile.ReadString("Server", "Pipe", NETDDE_PIPE_DEFAULT);

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

void CNetDDESvrApp::SaveConfig()
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
** Method:		OnDisconnect()
**
** Description:	A conversation was terminated by the server.
**
** Parameters:	pConv	The conversion.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnDisconnect(CDDECltConv* pConv)
{
	HCONV      hConv = pConv->Handle();
	CBuffer    oBuffer;
	CMemStream oStream(oBuffer);

	oStream.Create();
	oStream.Write(&hConv, sizeof(hConv));
	oStream.Close();

	CNetDDEPacket oPacket(CNetDDEPacket::DDE_DISCONNECT, oBuffer);

	// Notify all NetDDEClients...
	for (int i = 0; i < m_aoConnections.Size(); ++i)
	{
		CNetDDESvrPipe* pConnection = m_aoConnections[i];

		if (pConnection->UsesConversation(pConv))
		{
			// Remove conversation from connection.
			pConnection->RemoveConversation(pConv);

			try
			{
				if (App.m_bTraceConvs)
					App.Trace("DDE_DISCONNECT: %s, %s", pConv->Service(), pConv->Topic());

				// Send disconnect message.
				pConnection->SendPacket(oPacket);

				// Update stats.
				++m_nPktsSent;
			}
			catch (CPipeException& e)
			{
				App.Trace("PIPE_ERROR: %s", e.ErrorText());

				// Pipe disconnected?.
				pConnection->Close();
			}
		}
	}
}

/******************************************************************************
** Method:		OnAdvise()
**
** Description:	A linked has been updated by the server.
**
** Parameters:	pLink	The link.
**				pData	The updated data, if provided.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnAdvise(CDDELink* pLink, const CDDEData* pData)
{
	ASSERT(pData != NULL);

	// Ignore Advise, if during an Advise Start.
	if (pLink == NULL)
		return;

	CDDEConv*  pConv = pLink->Conversation();
	HCONV      hConv = pConv->Handle();
	CBuffer    oBuffer;
	CMemStream oStream(oBuffer);

	oStream.Create();

	oStream.Write(&hConv, sizeof(hConv));
	oStream << pLink->Item();
	oStream << (uint32) pLink->Format();
	oStream << pData->GetBuffer();

	oStream.Close();

	CNetDDEPacket oPacket(CNetDDEPacket::DDE_ADVISE, oBuffer);

	CLinkValue* pValue = NULL;

	// Find the links' value cache.
	if ((pValue = m_oLinkCache.Find(pConv, pLink)) == NULL)
		pValue = m_oLinkCache.Create(pConv, pLink);

	ASSERT(pValue != NULL);

	// Update links' value.
	pValue->m_oLastValue  = pData->GetBuffer();
	pValue->m_tLastUpdate = CDateTime::Current();

	// Notify all NetDDEClients...
	for (int i = 0; i < m_aoConnections.Size(); ++i)
	{
		CNetDDESvrPipe* pConnection = m_aoConnections[i];

		if (pConnection->UsesLink(pLink))
		{
			try
			{
				if (App.m_bTraceUpdates)
				{
					uint    nFormat = pLink->Format();
					CString strData;

					if (nFormat == CF_TEXT)
						strData = (const char*)(const void*)pData->GetBuffer().Buffer();
					else
						strData = CClipboard::FormatName(nFormat);

					App.Trace("DDE_ADVISE: %s %s [%s]", pConv->Service(), pLink->Item(), strData);
				}

				// Send advise message.
				pConnection->SendPacket(oPacket);

				// Update stats.
				++m_nPktsSent;
			}
			catch (CPipeException& e)
			{
				App.Trace("PIPE_ERROR: %s", e.ErrorText());

				// Pipe disconnected?.
				pConnection->Close();
			}
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

void CNetDDESvrApp::OnTimer(uint /*nTimerID*/)
{
	try
	{
		// Guard against re-entrancy.
		if (!g_bInBgProcessing)
		{
			g_bInBgProcessing = true;

			HandleConnects();
			HandleRequests();
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
** Method:		HandleConnects()
**
** Description:	Handle connection attempts by NetDDEClients.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::HandleConnects()
{
	try
	{
		// No server pipe for connections?
		if (m_pConnection == NULL)
		{
			CString strPipeName;

			strPipeName.Format(NETDDE_PIPE_FORMAT, ".", m_strPipeName);

			// Create pipe instance.
			m_pConnection = new CNetDDESvrPipe();
			m_pConnection->Create(strPipeName);
		}

		// Connection waiting?
		if (m_pConnection->Accept())
		{
			if (App.m_bTraceNetConns)
				App.Trace("PIPE_STATUS: Connection accepted");

			// Add to pipe collection and reset.
			m_aoConnections.Add(m_pConnection);
			m_pConnection = NULL;
		}
	}
	catch (CPipeException& e)
	{
		App.Trace("PIPE_ERROR: %s", e.ErrorText());

		// Kill background processing.
		::KillTimer(NULL, m_nTimerID);

		FatalMsg(e.ErrorText());
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

void CNetDDESvrApp::HandleRequests()
{
	// Any packets waiting?
	for (int i = 0; i < m_aoConnections.Size(); ++i)
	{
		CNetDDESvrPipe* pConnection = m_aoConnections[i]; 

		// Ignore if connection closed.
		if (!pConnection->IsOpen())
			continue;

		try
		{
			CNetDDEPacket oPacket;

			while (pConnection->RecvPacket(oPacket))
			{
				// Update stats.
				++m_nPktsRecv;

				// Decode packet type.
				switch (oPacket.DataType())
				{
					case CNetDDEPacket::NETDDE_CLIENT_CONNECT:		OnNetDDEClientConnect(*pConnection, oPacket);		break;
					case CNetDDEPacket::NETDDE_CLIENT_DISCONNECT:	OnNetDDEClientDisconnect(*pConnection, oPacket);	break;
					case CNetDDEPacket::DDE_CREATE_CONVERSATION:	OnDDECreateConversation(*pConnection, oPacket);		break;
					case CNetDDEPacket::DDE_DESTROY_CONVERSATION:	OnDDEDestroyConversation(*pConnection, oPacket);	break;
					case CNetDDEPacket::DDE_REQUEST:				OnDDERequest(*pConnection, oPacket);				break;
					case CNetDDEPacket::DDE_START_ADVISE:			OnDDEStartAdvise(*pConnection, oPacket);			break;
					case CNetDDEPacket::DDE_STOP_ADVISE:			OnDDEStopAdvise(*pConnection, oPacket);				break;
					case CNetDDEPacket::DDE_START_ADVISE_ASYNC:		OnDDEStartAdvise(*pConnection, oPacket);			break;
					default:										ASSERT_FALSE();										break;
				}

				// If discconnect received, stop polling.
				if (!pConnection->IsOpen())
					break;
			}
		}
		catch (CPipeException& e)
		{
			App.Trace("PIPE_ERROR: %s", e.ErrorText());

			// Pipe disconnected?.
			pConnection->Close();
		}
	}
}

/******************************************************************************
** Method:		HandleDisconnects()
**
** Description:	Handle disconnects by NetDDEClients.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::HandleDisconnects()
{
	// Any pipes now closed?
	for (int i = 0; i < m_aoConnections.Size(); ++i)
	{
		CNetDDESvrPipe* pConnection = m_aoConnections[i]; 

		if (!pConnection->IsOpen())
		{
			CConvs aoConvs;

			// Delete the connections conversation list.
			pConnection->GetConversations(aoConvs);

			for (int j = 0; j < aoConvs.Size(); ++j)
				m_pDDEClient->DestroyConversation(aoConvs[j]);

			// Delete from collection.
			m_aoConnections.Delete(m_aoConnections.Find(pConnection));
		}
	}
}

/******************************************************************************
** Method:		OnNetDDEClientConnect()
**
** Description:	NetDDEClient establishing a connection.
**
** Parameters:	oConnection		The connection the packet came from.
**				oReqPacket		The request packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnNetDDEClientConnect(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::NETDDE_CLIENT_CONNECT);

	bool bResult = false;

	uint16  nProtocol;
	CString strService;
	CString strComputer;
	CString strUser;

	// Decode request message.
	CMemStream oReqStream(oReqPacket.Buffer());

	oReqStream.Open();
	oReqStream.Seek(sizeof(CNetDDEPacket::Header));

	oReqStream >> nProtocol;
	oReqStream >> strService;
	oReqStream >> strComputer;
	oReqStream >> strUser;

	oReqStream.Close();

	if (App.m_bTraceNetConns)
		App.Trace("NETDDE_CLIENT_CONNECT: %u %s %s %s", nProtocol, strService, strComputer, strUser);

	// Save connection details.
	oConnection.Service(strService);
	oConnection.Computer(strComputer);
	oConnection.User(strUser);

	if (nProtocol == NETDDE_PROTOCOL)
		bResult = true;

	// Create response message.
	CMemStream oRspStream(oReqPacket.Buffer());

	oRspStream.Create();

	oRspStream << bResult;

	oRspStream.Close();

	// Send response message.
	CNetDDEPacket oRspPacket(CNetDDEPacket::NETDDE_CLIENT_CONNECT, oReqPacket.Buffer());

	oConnection.SendPacket(oRspPacket);

	// Update stats.
	++m_nPktsSent;
}

/******************************************************************************
** Method:		OnNetDDEClientDisconnect()
**
** Description:	NetDDEClient terminating a connection.
**
** Parameters:	oConnection		The connection the packet came from.
**				oReqPacket		The request packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnNetDDEClientDisconnect(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::NETDDE_CLIENT_DISCONNECT);

	CString strService;
	CString strComputer;

	// Decode request message.
	CMemStream oStream(oReqPacket.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	oStream >> strService;
	oStream >> strComputer;

	oStream.Close();

	if (App.m_bTraceNetConns)
		App.Trace("NETDDE_CLIENT_DISCONNECT: %s %s", strService, strComputer);

	// Close the connection.
	oConnection.Close();
}

/******************************************************************************
** Method:		OnDDECreateConversation()
**
** Description:	NetDDEClient establishing a DDE conversation.
**
** Parameters:	oConnection		The connection the packet came from.
**				oReqPacket		The request packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnDDECreateConversation(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_CREATE_CONVERSATION);

	bool bResult = false;

	CString strService;
	CString strTopic;
	HCONV   hConv = NULL;

	// Decode request message.
	CMemStream oReqStream(oReqPacket.Buffer());

	oReqStream.Open();
	oReqStream.Seek(sizeof(CNetDDEPacket::Header));

	oReqStream >> strService;
	oReqStream >> strTopic;

	oReqStream.Close();

	if (App.m_bTraceConvs)
		App.Trace("DDE_CREATE_CONVERSATION: %s %s", strService, strTopic);

	try
	{
/**/
		// Call DDE to create the conversation.
		CDDECltConv* pConv = m_pDDEClient->CreateConversation(strService, strTopic);

		bResult = true;
		hConv   = pConv->Handle();

		// Attach to the connection.
		oConnection.AddConversation(pConv);
/**/
	}
	catch (CDDEException& /*e*/)
	{
	}

	// Create response message.
	CMemStream oRspStream(oReqPacket.Buffer());

	oRspStream.Create();

	oRspStream << bResult;
	oRspStream.Write(&hConv, sizeof(hConv));

	oRspStream.Close();

	// Send response message.
	CNetDDEPacket oRspPacket(CNetDDEPacket::DDE_CREATE_CONVERSATION, oReqPacket.Buffer());

	oConnection.SendPacket(oRspPacket);

	// Update stats.
	++m_nPktsSent;
}

/******************************************************************************
** Method:		OnDDEDestroyConversation()
**
** Description:	NetDDEClient terminating a DDE conversation.
**
** Parameters:	oConnection		The connection the packet came from.
**				oReqPacket		The request packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnDDEDestroyConversation(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_DESTROY_CONVERSATION);

	bool bResult = false;

	HCONV	 hConv;

	// Decode message.
	CMemStream oStream(oReqPacket.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	oStream.Read(&hConv, sizeof(hConv));

	oStream.Close();

	if (App.m_bTraceConvs)
		App.Trace("DDE_DESTROY_CONVERSATION: 0x%p", hConv);

	try
	{
		// Locate the conversation.
		CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

		if (pConv != NULL)
		{
			// Detach from the connection.
			oConnection.RemoveConversation(pConv);

			// Call DDE to terminate the conversation.
			m_pDDEClient->DestroyConversation(pConv);

			bResult = true;
		}
	}
	catch (CDDEException& /*e*/)
	{
	}
}

/******************************************************************************
** Method:		OnDDERequest()
**
** Description:	NetDDEClient requesting an item.
**
** Parameters:	oConnection		The connection the packet came from.
**				oReqPacket		The request packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnDDERequest(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_REQUEST);

	bool     bResult = false;

	HCONV	 hConv;
	CString  strItem;
	uint32   nFormat;
	CDDEData oData(m_pDDEClient);

	// Decode message.
	CMemStream oStream(oReqPacket.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	oStream.Read(&hConv, sizeof(hConv));
	oStream >> strItem;
	oStream >> nFormat;

	oStream.Close();

	if (App.m_bTraceRequests)
		App.Trace("DDE_REQUEST: %s %s", strItem, CClipboard::FormatName(nFormat));

	try
	{
		// Locate the conversation.
		CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

		if (pConv != NULL)
		{
			// Call DDE to make the request.
			oData = pConv->Request(strItem, nFormat);

			bResult = true;
		}
	}
	catch (CDDEException& /*e*/)
	{
	}

	// Create response message.
	CMemStream oRspStream(oReqPacket.Buffer());

	oRspStream.Create();

	oRspStream << bResult;
	oRspStream << oData.GetBuffer();
	oRspStream << "DDE_REQUEST";

	oRspStream.Close();

	// Send response message.
	CNetDDEPacket oRspPacket(CNetDDEPacket::DDE_REQUEST, oReqPacket.Buffer());

	oConnection.SendPacket(oRspPacket);

	// Update stats.
	++m_nPktsSent;
}

/******************************************************************************
** Method:		OnDDEStartAdvise()
**
** Description:	NetDDEClient starting an advise loop on an item.
**
** Parameters:	oConnection		The connection the packet came from.
**				oReqPacket		The request packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnDDEStartAdvise(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket)
{
	int nPktType = oReqPacket.DataType();

	ASSERT((nPktType == CNetDDEPacket::DDE_START_ADVISE) || (nPktType == CNetDDEPacket::DDE_START_ADVISE_ASYNC));

	bool         bResult = false;
	CDDECltConv* pConv = NULL;
	CDDELink*    pLink = NULL;

	HCONV	 hConv;
	CString  strItem;
	uint32   nFormat;
	bool	 bReqVal;

	// Decode message.
	CMemStream oStream(oReqPacket.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	oStream.Read(&hConv, sizeof(hConv));
	oStream >> strItem;
	oStream >> nFormat;
	oStream >> bReqVal;

	oStream.Close();

	if (App.m_bTraceAdvises)
	{
		CString strMsg = (nPktType == CNetDDEPacket::DDE_START_ADVISE) ? "DDE_START_ADVISE" : "DDE_START_ADVISE_ASYNC";

		App.Trace("%s: %s %s", strMsg, strItem, CClipboard::FormatName(nFormat));
	}

	try
	{
		// Locate the conversation.
		pConv = m_pDDEClient->FindConversation(hConv);

		if (pConv != NULL)
		{
			// Call DDE to create the link.
			pLink = pConv->CreateLink(strItem, nFormat);

			// Attach to the connection.
			oConnection.AddLink(pLink);

			bResult = true;
		}
	}
	catch (CDDEException& /*e*/)
	{
	}

	// Synchronous advise start?
	if (nPktType == CNetDDEPacket::DDE_START_ADVISE)
	{
		// Create response message.
		CMemStream oRspStream(oReqPacket.Buffer());

		oRspStream.Create();

		oRspStream << bResult;

		oRspStream.Close();

		// Send response message.
		CNetDDEPacket oRspPacket(CNetDDEPacket::DDE_START_ADVISE, oReqPacket.Buffer());

		oConnection.SendPacket(oRspPacket);

		// Update stats.
		++m_nPktsSent;
	}

	CLinkValue* pLinkValue = NULL;

	// Link established AND 1st link AND need to request value?
	if ( (bResult) && (pLink->RefCount() == 1) && (bReqVal) )
	{
		try
		{
			// Request links current value.
			CDDEData oData = pConv->Request(strItem, nFormat);

			// Find the links' value cache.
			if ((pLinkValue = m_oLinkCache.Find(pConv, pLink)) == NULL)
				pLinkValue = m_oLinkCache.Create(pConv, pLink);

			ASSERT(pLinkValue != NULL);

			// Update links' value cache.
			pLinkValue->m_oLastValue  = oData.GetBuffer();;
			pLinkValue->m_tLastUpdate = CDateTime::Current();
		}
		catch (CDDEException& /*e*/)
		{ }
	}
	// Link established AND not 1st real link?
	if ( (bResult) && (pLink->RefCount() > 1) )
	{
		// Find last advise value.
		pLinkValue = m_oLinkCache.Find(pConv, pLink);
	}

	// Send initial advise?
	if (pLinkValue != NULL)
	{
		CBuffer    oBuffer;
		CMemStream oStream(oBuffer);

		oStream.Create();

		oStream.Write(&hConv, sizeof(hConv));
		oStream << strItem;
		oStream << nFormat;
		oStream << pLinkValue->m_oLastValue;

		oStream.Close();

		CNetDDEPacket oPacket(CNetDDEPacket::DDE_ADVISE, oBuffer);

		// Send links' last advise data.
		oConnection.SendPacket(oPacket);

		// Update stats.
		++m_nPktsSent;

		if (App.m_bTraceUpdates)
		{
			CString strData;

			if (nFormat == CF_TEXT)
				strData = pLinkValue->m_oLastValue.ToString();
			else
				strData = CClipboard::FormatName(nFormat);

			App.Trace("DDE_ADVISE: %s %u", strItem, nFormat);
		}
	}
}

/******************************************************************************
** Method:		OnDDEStopAdvise()
**
** Description:	NetDDEClient stopping an advise loop on an item.
**
** Parameters:	oConnection		The connection the packet came from.
**				oReqPacket		The request packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnDDEStopAdvise(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_STOP_ADVISE);

	bool     bResult = false;

	HCONV	 hConv;
	CString  strItem;
	uint32   nFormat;

	// Decode message.
	CMemStream oStream(oReqPacket.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	oStream.Read(&hConv, sizeof(hConv));
	oStream >> strItem;
	oStream >> nFormat;

	oStream.Close();

	if (App.m_bTraceAdvises)
		App.Trace("DDE_STOP_ADVISE: %s %s", strItem, CClipboard::FormatName(nFormat));

	try
	{
		// Locate the conversation.
		CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

		if (pConv != NULL)
		{
			// Locate the link. (May not exist, if async advised).
			CDDELink* pLink = pConv->FindLink(strItem, nFormat);

			if (pLink != NULL)
			{
				// Detach from the connection.
				oConnection.RemoveLink(pLink);

				// Call DDE to destroy the link.
				pConv->DestroyLink(pLink);
			}

			bResult = true;
		}
	}
	catch (CDDEException& /*e*/)
	{
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

void CNetDDESvrApp::UpdateStats()
{
	// 1 second elapsed?
	if ((::GetTickCount() - m_dwTickCount) >= 1000)
	{
		uint nIconID = IDI_NET_IDLE;

		// Which Icon to show?
		if ( (m_nPktsSent > 0) && (m_nPktsRecv > 0) )
			nIconID = IDI_NET_BOTH;
		else if (m_nPktsSent > 0)
			nIconID = IDI_NET_SEND;
		else if (m_nPktsRecv > 0) 
			nIconID = IDI_NET_RECV;

		// Update tray icon.
		if (m_AppWnd.m_oTrayIcon.IsVisible())
			m_AppWnd.m_oTrayIcon.Modify(nIconID);

		// Reset for next update.
		m_nPktsSent   = 0;
		m_nPktsRecv   = 0;
		m_dwTickCount = ::GetTickCount();
	}
}
