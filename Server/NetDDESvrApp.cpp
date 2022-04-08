/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESVRAPP.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CNetDDESvrApp class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "NetDDESvrApp.hpp"
#include <NCL/DDEClient.hpp>
#include <WCL/FileException.hpp>
#include <NCL/WinSock.hpp>
#include "NetDDESvrSocket.hpp"
#include <NCL/SocketException.hpp>
#include "NetDDEConv.hpp"
#include <WCL/DateTime.hpp>
#include "NetDDEDefs.hpp"
#include "NetDDEPacket.hpp"
#include <NCL/DDECltConv.hpp>
#include <NCL/DDELink.hpp>
#include <NCL/DDEData.hpp>
#include "LinkValue.hpp"
#include "PacketCodec.hpp"
#include <WCL/Clipboard.hpp>
#include <Core/StringUtils.hpp>
#include <WCL/AutoBool.hpp>
#include <NCL/DDEException.hpp>
#include <Core/Algorithm.hpp>

#ifdef _MSC_VER
// declaration of 'Xxx' hides previous local declaration (MemStream).
#pragma warning(disable : 4456)
#endif

using namespace NetDDE;

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

const tchar* CNetDDESvrApp::INI_FILE_VER  = TXT("1.0");
const uint  CNetDDESvrApp::BG_TIMER_FREQ =  1000;

const bool  CNetDDESvrApp::DEF_TRAY_ICON         = true;
const bool  CNetDDESvrApp::DEF_MIN_TO_TRAY       = false;
const uint  CNetDDESvrApp::DEF_DDE_TIMEOUT       = 30000;
const bool  CNetDDESvrApp::DEF_DISCARD_DUPS      = true;
const bool  CNetDDESvrApp::DEF_TRACE_CONVS       = true;
const bool  CNetDDESvrApp::DEF_TRACE_REQUESTS    = false;
const bool  CNetDDESvrApp::DEF_TRACE_ADVISES     = false;
const bool  CNetDDESvrApp::DEF_TRACE_UPDATES     = false;
const bool  CNetDDESvrApp::DEF_TRACE_NET_CONNS   = true;
const bool  CNetDDESvrApp::DEF_TRACE_TO_WINDOW   = true;
const int   CNetDDESvrApp::DEF_TRACE_LINES       = 100;
const bool  CNetDDESvrApp::DEF_TRACE_TO_FILE     = false;
const tchar* CNetDDESvrApp::DEF_TRACE_FILE        = TXT("NetDDEServer.log");

const uint  CNetDDESvrApp::WM_POLL_SOCKETS = WM_APP + 1;

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
	, m_AppWnd()
	, m_AppCmds(m_AppWnd)
	, m_oSvrSocket(CSocket::ASYNC)
	, m_nNextConvID(1)
	, m_nTimerID(0)
	, m_bTrayIcon(DEF_TRAY_ICON)
	, m_bMinToTray(DEF_MIN_TO_TRAY)
	, m_nDDETimeOut(DEF_DDE_TIMEOUT)
	, m_bDiscardDups(DEF_DISCARD_DUPS)
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
	m_strTitle = TXT("NetDDE Server");

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
		catch (const CFileException& e)
		{
			AlertMsg(TXT("Failed to truncate trace file:\n\n%s"), e.twhat());

			m_bTraceToFile = false;
		}
	}

	try
	{
		// Initialise WinSock.
		int nResult = CWinSock::Startup(1, 1);

		if (nResult != 0)
		{
			FatalMsg(TXT("Failed to initialise WinSock layer: %d."), nResult);
			return false;
		}

		// Initialise the DDE client.
		m_pDDEClient = DDE::ClientPtr(new CDDEClient);
		m_pDDEClient->AddListener(this);

		// Open the listening socket.
		m_oSvrSocket.Listen(m_nServerPort);

		// Attach event handler.
		m_oSvrSocket.AddServerListener(this);
	}
	catch (const Core::Exception& e)
	{
		FatalMsg(TXT("%s"), e.twhat());
		return false;
	}

	// Create the main window.
	if (!m_AppWnd.Create())
		return false;

	// Show it.
	if (!m_rcLastPos.Empty())
		m_AppWnd.Move(m_rcLastPos);

	m_AppWnd.Show(m_iCmdShow);

	// Update UI.
	m_AppCmds.UpdateUI();

	// Start the background timer.
	m_nTimerID = StartTimer(BG_TIMER_FREQ);

	App.Trace(TXT("SERVER_STATUS: Server started"));
	App.Trace(TXT("SERVER_STATUS: Server listening on port: %d"), m_nServerPort);

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

	// Close the listening socket.
	m_oSvrSocket.Close();

	// Close all client connections...
	for (size_t i = 0; i < m_aoConnections.size(); ++i)
	{
		try
		{
			CNetDDESvrSocket* pConnection = m_aoConnections[i]; 

			if (pConnection->IsOpen())
			{
				// Delete the conversation list.
				for (size_t j = 0; j < pConnection->m_aoNetConvs.size(); ++j)
					m_pDDEClient->DestroyConversation(pConnection->m_aoNetConvs[j]->m_pSvrConv);

				if (App.m_bTraceNetConns)
					App.Trace(TXT("NETDDE_SERVER_DISCONNECT:"));

				// Send disconnect message.
				CNetDDEPacket oPacket(CNetDDEPacket::NETDDE_SERVER_DISCONNECT);

				pConnection->SendPacket(oPacket);

				// Update stats.
				++m_nPktsSent;
			}
		}
		catch (const Core::Exception& /*e*/)
		{
		}
	}

	// Close all client connections.
	Core::deleteAll(m_aoConnections);

	// Unnitialise the DDE client.
	m_pDDEClient->RemoveListener(this);
	m_pDDEClient.reset();

	// Empty the link cache.
	m_oLinkCache.Purge();

	// Terminate WinSock.
	CWinSock::Cleanup();

	// Save settings.
	SaveConfig();

	App.Trace(TXT("SERVER_STATUS: Server stopped"));

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

void CNetDDESvrApp::Trace(const tchar* pszMsg, ...)
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
	strMsg = CDateTime::Current().ToString() + TXT(" ") + strMsg;

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

			m_fTraceFile.Seek(0, WCL::IStreamBase::END);
			m_fTraceFile.WriteLine(strMsg, ANSI_TEXT);
			m_fTraceFile.Close();
		}
		catch (const CFileException& e)
		{
			m_bTraceToFile = false;

			AlertMsg(TXT("Failed to write to trace file:\n\n%s"), e.twhat());
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
	CString strVer = m_oIniFile.ReadString(TXT("Version"), TXT("Version"), INI_FILE_VER);

	// Read the server settings.
	m_nServerPort = m_oIniFile.ReadInt(TXT("Server"), TXT("Port"), NETDDE_PORT_DEFAULT);

	// Read the trace settings.
	m_bTraceConvs    = m_oIniFile.ReadBool  (TXT("Trace"), TXT("Conversations"),  m_bTraceConvs   );
	m_bTraceRequests = m_oIniFile.ReadBool  (TXT("Trace"), TXT("Requests"),       m_bTraceRequests);
	m_bTraceAdvises  = m_oIniFile.ReadBool  (TXT("Trace"), TXT("Advises"),        m_bTraceAdvises );
	m_bTraceUpdates  = m_oIniFile.ReadBool  (TXT("Trace"), TXT("Updates"),        m_bTraceUpdates );
	m_bTraceNetConns = m_oIniFile.ReadBool  (TXT("Trace"), TXT("NetConnections"), m_bTraceNetConns);
	m_bTraceToWindow = m_oIniFile.ReadBool  (TXT("Trace"), TXT("ToWindow"),       m_bTraceToWindow);
	m_nTraceLines    = m_oIniFile.ReadInt   (TXT("Trace"), TXT("Lines"),          m_nTraceLines   );
	m_bTraceToFile   = m_oIniFile.ReadBool  (TXT("Trace"), TXT("ToFile"),         m_bTraceToFile  );
	m_strTraceFile   = m_oIniFile.ReadString(TXT("Trace"), TXT("FileName"),       m_strTraceFile  );

	// Read the general settings.
	m_bTrayIcon    = m_oIniFile.ReadBool(TXT("Main"), TXT("TrayIcon"),     m_bTrayIcon   );
	m_bMinToTray   = m_oIniFile.ReadBool(TXT("Main"), TXT("MinToTray"),    m_bMinToTray  );
	m_nDDETimeOut  = m_oIniFile.ReadInt (TXT("Main"), TXT("DDETimeOut"),   m_nDDETimeOut );
	m_bDiscardDups = m_oIniFile.ReadBool(TXT("Main"), TXT("NoDuplicates"), m_bDiscardDups);

	// Read the window pos and size.
	m_rcLastPos.left   = m_oIniFile.ReadInt(TXT("UI"), TXT("Left"),   0);
	m_rcLastPos.top    = m_oIniFile.ReadInt(TXT("UI"), TXT("Top"),    0);
	m_rcLastPos.right  = m_oIniFile.ReadInt(TXT("UI"), TXT("Right"),  0);
	m_rcLastPos.bottom = m_oIniFile.ReadInt(TXT("UI"), TXT("Bottom"), 0);
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
	m_oIniFile.WriteString(TXT("Version"), TXT("Version"), INI_FILE_VER);

	// Write the server settings.
	m_oIniFile.WriteInt(TXT("Server"), TXT("Port"), m_nServerPort);

	// Write the trace settings.
	m_oIniFile.WriteBool  (TXT("Trace"), TXT("Conversations"),  m_bTraceConvs   );
	m_oIniFile.WriteBool  (TXT("Trace"), TXT("Requests"),       m_bTraceRequests);
	m_oIniFile.WriteBool  (TXT("Trace"), TXT("Advises"),        m_bTraceAdvises );
	m_oIniFile.WriteBool  (TXT("Trace"), TXT("Updates"),        m_bTraceUpdates );
	m_oIniFile.WriteBool  (TXT("Trace"), TXT("NetConnections"), m_bTraceNetConns);
	m_oIniFile.WriteBool  (TXT("Trace"), TXT("ToWindow"),       m_bTraceToWindow);
	m_oIniFile.WriteInt   (TXT("Trace"), TXT("Lines"),          m_nTraceLines   );
	m_oIniFile.WriteBool  (TXT("Trace"), TXT("ToFile"),         m_bTraceToFile  );
	m_oIniFile.WriteString(TXT("Trace"), TXT("FileName"),       m_strTraceFile  );

	// Write the general settings.
	m_oIniFile.WriteBool(TXT("Main"), TXT("TrayIcon"),     m_bTrayIcon   );
	m_oIniFile.WriteBool(TXT("Main"), TXT("MinToTray"),    m_bMinToTray  );
	m_oIniFile.WriteInt (TXT("Main"), TXT("DDETimeOut"),   m_nDDETimeOut );
	m_oIniFile.WriteBool(TXT("Main"), TXT("NoDuplicates"), m_bDiscardDups);

	// Write the window pos and size.
	m_oIniFile.WriteInt(TXT("UI"), TXT("Left"),   m_rcLastPos.left  );
	m_oIniFile.WriteInt(TXT("UI"), TXT("Top"),    m_rcLastPos.top   );
	m_oIniFile.WriteInt(TXT("UI"), TXT("Right"),  m_rcLastPos.right );
	m_oIniFile.WriteInt(TXT("UI"), TXT("Bottom"), m_rcLastPos.bottom);
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
	NetDDEPacketPtr packet = EncodeConversationDisconnectPacket(pConv->Handle());

	// For all NetDDEClients...
	for (size_t i = 0; i < m_aoConnections.size(); ++i)
	{
		CNetDDESvrSocket* pConnection = m_aoConnections[i];
		bool              bNotifyConn = true;

		// Clean-up all client conversations...
		for (int j = static_cast<int>(pConnection->m_aoNetConvs.size())-1; j >= 0; --j)
		{
			CNetDDEConv* pNetConv = pConnection->m_aoNetConvs[j];

			if (pNetConv->m_pSvrConv == pConv)
			{
				try
				{
					// Only send once per client.
					if (bNotifyConn)
					{
						bNotifyConn = false;

						if (App.m_bTraceConvs)
							App.Trace(TXT("DDE_DISCONNECT: %s %s"), pConv->Service().c_str(), pConv->Topic().c_str());

						// Send disconnect message.
						pConnection->SendPacket(packet.getRef());

						// Update stats.
						++m_nPktsSent;
					}
				}
				catch (const CSocketException& e)
				{
					App.Trace(TXT("SOCKET_ERROR: %s"), e.twhat());
				}

				Core::deleteAt(pConnection->m_aoNetConvs, j);
			}
		}
	}

	// Purge link cache.
	m_oLinkCache.Purge(pConv);

	uint nRefCount = pConv->RefCount();

	// Free conversation.
	while (nRefCount--)
		m_pDDEClient->DestroyConversation(pConv);
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
	ASSERT(pData != nullptr);

	// Ignore Advise, if during an Advise Start.
	if (pLink == nullptr)
		return;

	CDDEConv* pConv = pLink->Conversation();
	CBuffer   oData = pData->GetBuffer();

	// Find the links' value in the cache.
	CLinkValue* pValue = m_oLinkCache.Find(pConv, pLink);

	// Discard duplicate updates.
	if ((App.m_bDiscardDups) && (pValue != nullptr)
	 && (pValue->m_oLastValue == oData))
	{
		if (App.m_bTraceUpdates)
			App.Trace(TXT("DDE_ADVISE: %s %s (ignored)"), pConv->Service().c_str(), pLink->Item().c_str());

		return;
	}

	// Create a cache entry, if a new link.
	if (pValue == nullptr)
		pValue = m_oLinkCache.Create(pConv, pLink);

	ASSERT(pValue != nullptr);

	// Update links' cached value.
	pValue->m_oLastValue  = oData;
	pValue->m_tLastUpdate = CDateTime::Current();

	// Create advise packet.
	NetDDEPacketPtr packet = EncodeAdvisePacket(pConv->Handle(),
	                                            pLink->Item(),
	                                            pLink->Format(),
	                                            oData);

	// Notify all NetDDEClients...
	for (size_t i = 0; i < m_aoConnections.size(); ++i)
	{
		CNetDDESvrSocket* pConnection = m_aoConnections[i];

		// Ignore, if connection severed.
		if (!pConnection->IsOpen())
			continue;

		// Connection references link?
		if (pConnection->IsLinkUsed(pLink))
		{
			try
			{
				if (App.m_bTraceUpdates)
				{
					uint    nFormat = pLink->Format();
					CString strData;

					if (nFormat == CF_TEXT)
						strData = oData.ToString(ANSI_TEXT);
					else if (nFormat == CF_UNICODETEXT)
						strData = oData.ToString(UNICODE_TEXT);
					else
						strData = CClipboard::FormatName(nFormat);

					App.Trace(TXT("DDE_ADVISE: %s %s [%s]"), pConv->Service().c_str(), pLink->Item().c_str(), strData.c_str());
				}

				// Send advise message.
				pConnection->SendPacket(packet.getRef());

				// Update stats.
				++m_nPktsSent;
			}
			catch (const CSocketException& e)
			{
				App.Trace(TXT("SOCKET_ERROR: %s"), e.twhat());
			}
		}
	}
}

/******************************************************************************
** Method:		OnAcceptReady()
**
** Description:	New connection from a client.
**
** Parameters:	pSocket		The server socket.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnAcceptReady(CTCPSvrSocket* pSvrSocket)
{
	// Create server end of socket.
	CNetDDESvrSocket* pCltSocket = new CNetDDESvrSocket();

	try
	{
		// Accept connection.
		pSvrSocket->Accept(pCltSocket);

		if (App.m_bTraceNetConns)
			App.Trace(TXT("SOCKET_STATUS: Connection accepted from %s"), pCltSocket->PeerAddress().c_str());

		// Add to collection.
		m_aoConnections.push_back(pCltSocket);

		// Attach event handler.
		pCltSocket->AddClientListener(this);
	}
	catch (const CSocketException& e)
	{
		App.Trace(TXT("SOCKET_ERROR: %s"), e.twhat());

		delete pCltSocket;
	}
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

void CNetDDESvrApp::OnReadReady(CSocket* /*pSocket*/)
{
	// Handle later.
	m_MainThread.PostMessage(WM_POLL_SOCKETS);
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

void CNetDDESvrApp::OnClosed(CSocket* pSocket, int /*nReason*/)
{
	// NetDDE connection?
	if (pSocket != &m_oSvrSocket)
	{
		CNetDDESvrSocket* pConnection = static_cast<CNetDDESvrSocket*>(pSocket);

		if (App.m_bTraceNetConns)
			App.Trace(TXT("SOCKET_STATUS: Connection closed from %s"), pConnection->Host().c_str());
	}
	// Listening socket.
	else // (pSocket == &m_oSvrSocket)
	{
		FatalMsg(TXT("Server listening socket closed."));

		m_AppWnd.Destroy();
	}

	// Cleanup later.
	m_MainThread.PostMessage(WM_POLL_SOCKETS);
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

void CNetDDESvrApp::OnError(CSocket* /*pSocket*/, int nEvent, int nError)
{
	Trace(TXT("SOCKET_ERROR: %s [%s]"), CWinSock::ErrorToSymbol(nError).c_str(), CSocket::AsyncEventStr(nEvent).c_str());
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

void CNetDDESvrApp::OnTimer(uint /*nTimerID*/)
{
	UpdateStats();
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

void CNetDDESvrApp::OnNetDDEClientConnect(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::NETDDE_CLIENT_CONNECT);

	bool bResult = false;

	uint16  nProtocol;
	CString strService;
	CString strComputer;
	CString strUser;
	CString strProcess;
	CString strVersion;

	// Decode request message.
	DecodeClientConnectPacket(oReqPacket,
                              nProtocol,
                              strService,
                              strComputer,
                              strUser,
                              strProcess,
                              strVersion);

	if (App.m_bTraceNetConns)
	{
		App.Trace(TXT("NETDDE_CLIENT_CONNECT: %u %s %s %s"), nProtocol, strService.c_str(), strComputer.c_str(), strUser.c_str());
		App.Trace(TXT("NETDDE_CLIENT_VERSION: %s %s"), strProcess.c_str(), strVersion.c_str());
	}

	// Save connection details.
	oConnection.m_strService  = strService;
	oConnection.m_strComputer = strComputer;
	oConnection.m_strUser     = strUser;

	if (nProtocol == NETDDE_PROTOCOL)
		bResult = true;

	// Send response message.
	NetDDEPacketPtr replyPacket = EncodeClientConnectReplyPacket(oReqPacket.PacketID(), bResult);

	oConnection.SendPacket(replyPacket.getRef());

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

void CNetDDESvrApp::OnNetDDEClientDisconnect(CNetDDESvrSocket& /*oConnection*/, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::NETDDE_CLIENT_DISCONNECT);

	CString strService;
	CString strComputer;

	// Decode request message.
	DecodeClientDisconnectPacket(oReqPacket, strService, strComputer);

	if (App.m_bTraceNetConns)
		App.Trace(TXT("NETDDE_CLIENT_DISCONNECT: %s %s"), strService.c_str(), strComputer.c_str());
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

void CNetDDESvrApp::OnDDECreateConversation(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_CREATE_CONVERSATION);

	bool bResult = false;

	CString strService;
	CString strTopic;
	HCONV   hConv   = NULL;
	uint32  nConvID = m_nNextConvID++;

	// Decode request message.
	DecodeCreateConversationPacket(oReqPacket,
	                               strService,
	                               strTopic);

	if (App.m_bTraceConvs)
		App.Trace(TXT("DDE_CREATE_CONVERSATION: %s %s [#%u]"), strService.c_str(), strTopic.c_str(), nConvID);

	try
	{
		// Call DDE to create the conversation.
		CDDECltConv* pConv = m_pDDEClient->CreateConversation(strService, strTopic);

		bResult = true;
		hConv   = pConv->Handle();

		// Attach to the connection.
		oConnection.m_aoNetConvs.push_back(new CNetDDEConv(pConv, nConvID));

		// Apply settings.
		pConv->SetTimeout(App.m_nDDETimeOut);
	}
	catch (const CDDEException& e)
	{
		App.Trace(TXT("DDE_ERROR: %s"), e.twhat());
	}

	// Send response message.
	NetDDEPacketPtr replyPacket = EncodeCreateConversationReplyPacket(oReqPacket.PacketID(),
	                                                                  bResult,
	                                                                  hConv,
	                                                                  nConvID);

	oConnection.SendPacket(replyPacket.getRef());

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

void CNetDDESvrApp::OnDDEDestroyConversation(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_DESTROY_CONVERSATION);

	HCONV  hConv;
	uint32 nConvID;

	// Decode message.
	DecodeDestroyConversationPacket(oReqPacket,
	                                hConv,
	                                nConvID);

	if (App.m_bTraceConvs)
		App.Trace(TXT("DDE_DESTROY_CONVERSATION: 0x%p [#%u]"), hConv, nConvID);

	// Locate the conversation.
	CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

	if (pConv != nullptr)
	{
		CNetDDEConv* pNetConv = oConnection.FindNetConv(pConv, nConvID);

		ASSERT(pNetConv != nullptr);

		try
		{
			// Not final reference?
			if (pNetConv->m_pSvrConv->RefCount() != 1)
			{
				// Destroy NetDDE conversations' links.
				for (size_t i = 0; i < pNetConv->m_aoLinks.size(); ++i)
					pConv->DestroyLink(pNetConv->m_aoLinks[i]);
			}

			// Purge link cache, if last reference.
			if (pNetConv->m_pSvrConv->RefCount() == 1)
				m_oLinkCache.Purge(pConv);

			// Call DDE to terminate the conversation.
			m_pDDEClient->DestroyConversation(pConv);
		}
		catch (const CDDEException& e)
		{
			App.Trace(TXT("DDE_ERROR: %s"), e.twhat());
		}

		// Detach from the connection.
		Core::deleteValue(oConnection.m_aoNetConvs, pNetConv);
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

void CNetDDESvrApp::OnDDERequest(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_REQUEST);

	bool     bResult = false;

	HCONV	 hConv;
	uint32   nConvID;
	CString  strItem;
	uint32   nFormat;
	CBuffer  oBuffer;

	// Decode message.
	DecodeRequestItemPacket(oReqPacket,
	                        hConv,
	                        nConvID,
	                        strItem,
	                        nFormat);

	if (App.m_bTraceRequests)
		App.Trace(TXT("DDE_REQUEST: %s %s"), strItem.c_str(), CClipboard::FormatName(nFormat).c_str());

	try
	{
		// Locate the conversation.
		CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

		if (pConv != nullptr)
		{
			// Call DDE to make the request.
			CDDEData oData = pConv->Request(strItem, nFormat);

			oBuffer = oData.GetBuffer();

			bResult = true;
		}
	}
	catch (const CDDEException& e)
	{
		App.Trace(TXT("DDE_ERROR: %s"), e.twhat());
	}

	// Send response message.
	NetDDEPacketPtr replyPacket = EncodeRequestItemReplyPacket(oReqPacket.PacketID(),
	                                                           bResult,
	                                                           oBuffer);
	oConnection.SendPacket(replyPacket.getRef());

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

void CNetDDESvrApp::OnDDEStartAdvise(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_START_ADVISE);

	bool         bResult = false;
	CDDECltConv* pConv = nullptr;
	CDDELink*    pLink = nullptr;

	HCONV	 hConv;
	uint32   nConvID;
	CString  strItem;
	uint32   nFormat;
	bool     bAsync;
	bool	 bReqVal;

	// Decode message.
	DecodeStartAdvisePacket(oReqPacket,
	                        hConv,
	                        nConvID,
	                        strItem,
	                        nFormat,
	                        bAsync,
	                        bReqVal);

	if (App.m_bTraceAdvises)
		App.Trace(TXT("DDE_START_ADVISE: %s %s %s"), strItem.c_str(), CClipboard::FormatName(nFormat).c_str(), (bAsync) ? TXT("[ASYNC]") : TXT(""));

	try
	{
		// Locate the conversation.
		pConv = m_pDDEClient->FindConversation(hConv);

		if (pConv != nullptr)
		{
			CNetDDEConv* pNetConv = oConnection.FindNetConv(pConv, nConvID);

			ASSERT(pNetConv != nullptr);

			// Call DDE to create the link.
			pLink = pConv->CreateLink(strItem, nFormat);

			// Attach to the connection.
			pNetConv->m_aoLinks.push_back(pLink);

			bResult = true;
		}
	}
	catch (const CDDEException& e)
	{
		App.Trace(TXT("DDE_ERROR: %s"), e.twhat());
	}

	// Sync advise start?
	if (!bAsync)
	{
		// Send response message.
		NetDDEPacketPtr replyPacket = EncodeStartAdviseReplyPacket(oReqPacket.PacketID(), bResult);
		oConnection.SendPacket(replyPacket.getRef());

		// Update stats.
		++m_nPktsSent;
	}

	// Failed async advise start?
	if ((bAsync) && (!bResult))
	{
		// Send response message.
		NetDDEPacketPtr replyPacket = EncodeAdviseStartFailedPacket(hConv,
		                                                            strItem,
		                                                            nFormat);
		oConnection.SendPacket(replyPacket.getRef());

		// Update stats.
		++m_nPktsSent;
	}

	CLinkValue* pLinkValue = nullptr;

	// Link established AND 1st link AND need to request value?
	if ( (bResult) && (pLink->RefCount() == 1) && (bReqVal) )
	{
		try
		{
			// Request links current value.
			CDDEData oData = pConv->Request(strItem, nFormat);

			// Find the links' value cache.
			if ((pLinkValue = m_oLinkCache.Find(pConv, pLink)) == nullptr)
				pLinkValue = m_oLinkCache.Create(pConv, pLink);

			ASSERT(pLinkValue != nullptr);

			// Update links' value cache.
			pLinkValue->m_oLastValue  = oData.GetBuffer();;
			pLinkValue->m_tLastUpdate = CDateTime::Current();
		}
		catch (const CDDEException& e)
		{
			App.Trace(TXT("DDE_ERROR: %s"), e.twhat());
		}
	}
	// Link established AND not 1st real link?
	else if ( (bResult) && (pLink->RefCount() > 1) )
	{
		// Find last advise value.
		pLinkValue = m_oLinkCache.Find(pConv, pLink);
	}

	// Send initial advise?
	if (pLinkValue != nullptr)
	{
		// Send links' last advise data.
		NetDDEPacketPtr packet = EncodeAdvisePacket(hConv,
		                                            strItem,
		                                            nFormat,
		                                            pLinkValue->m_oLastValue);
		oConnection.SendPacket(packet.getRef());

		// Update stats.
		++m_nPktsSent;

		if (App.m_bTraceUpdates)
		{
			CString strData;

			if (nFormat == CF_TEXT)
				strData = pLinkValue->m_oLastValue.ToString(ANSI_TEXT);
			else if (nFormat == CF_UNICODETEXT)
				strData = pLinkValue->m_oLastValue.ToString(UNICODE_TEXT);
			else
				strData = CClipboard::FormatName(nFormat);

			App.Trace(TXT("DDE_ADVISE: %s %u"), strItem.c_str(), nFormat);
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

void CNetDDESvrApp::OnDDEStopAdvise(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_STOP_ADVISE);

	HCONV	 hConv;
	uint32   nConvID;
	CString  strItem;
	uint32   nFormat;

	// Decode message.
	DecodeStopAdvisePacket(oReqPacket,
	                       hConv,
	                       nConvID,
	                       strItem,
	                       nFormat);

	if (App.m_bTraceAdvises)
		App.Trace(TXT("DDE_STOP_ADVISE: %s %s"), strItem.c_str(), CClipboard::FormatName(nFormat).c_str());

	// Locate the conversation.
	CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

	if (pConv != nullptr)
	{
		// Locate the link. (May not exist, if async advised).
		CDDELink* pLink = pConv->FindLink(strItem, nFormat);

		if (pLink != nullptr)
		{
			CNetDDEConv* pNetConv = oConnection.FindNetConv(pConv, nConvID);

			ASSERT(pNetConv != nullptr);

			try
			{
				// Call DDE to destroy the link.
				pConv->DestroyLink(pLink);
			}
			catch (const CDDEException& e)
			{
				App.Trace(TXT("DDE_ERROR: %s"), e.twhat());
			}

			// Detach from the connection.
			Core::eraseValue(pNetConv->m_aoLinks, pLink);
		}
	}
}

/******************************************************************************
** Method:		OnDDEExecute()
**
** Description:	NetDDEClient executing a command.
**
** Parameters:	oConnection		The connection the packet came from.
**				oReqPacket		The request packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnDDEExecute(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_EXECUTE);

	bool     bResult = false;

	HCONV	hConv;
	uint32  nConvID;
	CString strCmd;

	// Decode message.
	DecodeExecuteCommandPacket(oReqPacket,
	                           hConv,
	                           nConvID,
	                           strCmd);

	if (App.m_bTraceRequests)
		App.Trace(TXT("DDE_EXECUTE: 0x%p [%s]"), hConv, strCmd.c_str());

	try
	{
		// Locate the conversation.
		CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

		if (pConv != nullptr)
		{
			// Call DDE to do the execute.
			pConv->ExecuteString(strCmd);

			bResult = true;
		}
	}
	catch (const CDDEException& e)
	{
		App.Trace(TXT("DDE_ERROR: %s"), e.twhat());
	}

	// Send response message.
	NetDDEPacketPtr replyPacket = EncodeExecuteCommandReplyPacket(oReqPacket.PacketID(), bResult);
	oConnection.SendPacket(replyPacket.getRef());

	// Update stats.
	++m_nPktsSent;
}

/******************************************************************************
** Method:		OnDDEPoke()
**
** Description:	NetDDEClient pokeing a value into an item.
**
** Parameters:	oConnection		The connection the packet came from.
**				oReqPacket		The request packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnDDEPoke(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket)
{
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_POKE);

	bool     bResult = false;

	HCONV	 hConv;
	uint32   nConvID;
	CString  strItem;
	uint32   nFormat;
	CBuffer  oData;

	// Decode message.
	DecodePokeItemPacket(oReqPacket,
	                     hConv,
	                     nConvID,
	                     strItem,
	                     nFormat,
	                     oData);

	if (App.m_bTraceRequests)
	{
		CString strData;

		if (nFormat == CF_TEXT)
			strData = oData.ToString(ANSI_TEXT);
		else if (nFormat == CF_UNICODETEXT)
			strData = oData.ToString(UNICODE_TEXT);
		else
			strData = CClipboard::FormatName(nFormat);

		App.Trace(TXT("DDE_POKE: %s %s [%s]"), strItem.c_str(), CClipboard::FormatName(nFormat).c_str(), strData.c_str());
	}

	try
	{
		// Locate the conversation.
		CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

		if (pConv != nullptr)
		{
			// Call DDE to do the poke.
			pConv->Poke(strItem, nFormat, oData.Buffer(), oData.Size());

			bResult = true;
		}
	}
	catch (const CDDEException& e)
	{
		App.Trace(TXT("DDE_ERROR: %s"), e.twhat());
	}

	// Send response message.
	NetDDEPacketPtr replyPacket = EncodePokeItemReplyPacket(oReqPacket.PacketID(), bResult);
	oConnection.SendPacket(replyPacket.getRef());

	// Update stats.
	++m_nPktsSent;
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
	uint nIconID = IDI_NET_IDLE;

	// Which Icon to show?
	if ( (m_nPktsSent > 0) && (m_nPktsRecv > 0) )
		nIconID = IDI_NET_BOTH;
	else if (m_nPktsSent > 0)
		nIconID = IDI_NET_SEND;
	else if (m_nPktsRecv > 0) 
		nIconID = IDI_NET_RECV;
	else if (m_aoConnections.size() == 0)
		nIconID = IDI_NET_LOST;

	// Format tooltip.
	CString strTip = TXT("NetDDE Server");

	if (m_aoConnections.size() > 0)
	{
		strTip += TXT("\nConnections: ")   + Core::format(m_aoConnections.size());
		strTip += TXT("\nConversations: ") + Core::format(m_pDDEClient->GetNumConversations());
	}

	// Update tray icon.
	if (m_AppWnd.m_oTrayIcon.IsVisible())
		m_AppWnd.m_oTrayIcon.Modify(nIconID, strTip);

	// Reset for next update.
	m_nPktsSent   = 0;
	m_nPktsRecv   = 0;
}

/******************************************************************************
** Method:		CloseConnection()
**
** Description:	Close the connection to the client.
**
** Parameters:	pConnection		The connection.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::CloseConnection(CNetDDESvrSocket* pConnection)
{
	try
	{
		// Send disconnect message.
		CNetDDEPacket oPacket(CNetDDEPacket::NETDDE_SERVER_DISCONNECT);

		pConnection->SendPacket(oPacket);

		// Update stats.
		++App.m_nPktsSent;
	}
	catch (const CSocketException& /*e*/)
	{ }

	pConnection->Close();

	// Cleanup.
	OnClosed(pConnection, 0);
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
void CNetDDESvrApp::OnThreadMsg(UINT nMsg, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	// Forward to real handler.
	if (nMsg == WM_POLL_SOCKETS)
		OnPollSockets();
}

/******************************************************************************
** Method:		OnPollSockets()
**
** Description:	Poll the sockets and process any new requests.
**				NB: Can be re-entered if in the middle of a DDE transaction.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnPollSockets()
{
	// Re-entrancy guard.
	static bool bGuarded = false;

	if (!bGuarded)
	{
		CAutoBool l(&bGuarded);

		bool bRecvPacket = true;

		// Until no packets left...
		while (bRecvPacket)
		{
			bRecvPacket = false;

			// For all connections...
			for (size_t i = 0; i < m_aoConnections.size(); ++i)
			{
				CNetDDESvrSocket* pConnection = m_aoConnections[i];

				// Ignore, if no longer open.
				if (!pConnection->IsOpen())
					continue;

				try
				{
					CNetDDEPacket oPacket;

					// No packet waiting?
					if (!pConnection->RecvPacket(oPacket))
						continue;

					bRecvPacket = true;

					// Dispatch packet to handler.
					switch (oPacket.DataType())
					{
						case CNetDDEPacket::NETDDE_CLIENT_CONNECT:		OnNetDDEClientConnect(*pConnection, oPacket);		break;
						case CNetDDEPacket::NETDDE_CLIENT_DISCONNECT:	OnNetDDEClientDisconnect(*pConnection, oPacket);	break;
						case CNetDDEPacket::DDE_CREATE_CONVERSATION:	OnDDECreateConversation(*pConnection, oPacket);		break;
						case CNetDDEPacket::DDE_DESTROY_CONVERSATION:	OnDDEDestroyConversation(*pConnection, oPacket);	break;
						case CNetDDEPacket::DDE_REQUEST:				OnDDERequest(*pConnection, oPacket);				break;
						case CNetDDEPacket::DDE_START_ADVISE:			OnDDEStartAdvise(*pConnection, oPacket);			break;
						case CNetDDEPacket::DDE_STOP_ADVISE:			OnDDEStopAdvise(*pConnection, oPacket);				break;
						case CNetDDEPacket::DDE_EXECUTE:				OnDDEExecute(*pConnection, oPacket);				break;
						case CNetDDEPacket::DDE_POKE:					OnDDEPoke(*pConnection, oPacket);					break;
						default:										ASSERT_FALSE();										break;
					}

					// Update stats.
					++m_nPktsRecv;
				}
				catch (const CSocketException& e)
				{
					App.Trace(TXT("SOCKET_ERROR: %s"), e.twhat());
				}
			}
		}

		// For all connections...
		for (size_t i = 0; i < m_aoConnections.size(); ++i)
		{
			CNetDDESvrSocket* pConnection = m_aoConnections[i];

			// Connection now closed?
			if (!pConnection->IsOpen())
			{
				// Close all DDE conversations.
				for (size_t j = 0; j < pConnection->m_aoNetConvs.size(); ++j)
				{
					CNetDDEConv* pNetConv = pConnection->m_aoNetConvs[j];

					// Purge link cache, if last reference.
					if (pNetConv->m_pSvrConv->RefCount() == 1)
						m_oLinkCache.Purge(pNetConv->m_pSvrConv);

					m_pDDEClient->DestroyConversation(pNetConv->m_pSvrConv);
				}

				// Delete from collection.
				Core::deleteValue(m_aoConnections, pConnection);
			}
		}

		// Flush value cache, if all connections closed.
		if ( (m_aoConnections.size() == 0) && (m_oLinkCache.Size() > 0) )
			m_oLinkCache.Purge();
	}
}
