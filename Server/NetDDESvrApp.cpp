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
const char* CNetDDESvrApp::VERSION      = "v1.0 [Debug]";
#else
const char* CNetDDESvrApp::VERSION      = "v1.0";
#endif

const char* CNetDDESvrApp::INI_FILE_VER  = "1.0";
const uint  CNetDDESvrApp::BG_TIMER_FREQ = 10;
const uint  CNetDDESvrApp::DEF_MAX_TRACE = 25;

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
	, m_nMaxTrace(DEF_MAX_TRACE)
	, m_bLogTrace(true)
	, m_strLogFile(CPath::ApplicationDir(), "NetDDEServer.log")
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
	m_strTitle = "Net DDE Server";

	// Load settings.
	LoadConfig();

	// Clear the log file.
	m_fLogFile.Create(m_strLogFile);
	m_fLogFile.Close();

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

				App.Trace("NETDDE_SERVER_DISCONNECT:");

				// Send disconnect message.
				CNetDDEPacket oPacket(CNetDDEPacket::NETDDE_SERVER_DISCONNECT, NULL, 0);

				pConnection->SendPacket(oPacket);
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
	CString strMsg; //, strTime;

	// Setup arguments.
	va_list	args;
	va_start(args, pszMsg);
	
	// Format message.
	strMsg.FormatEx(pszMsg, args);
//	strTime.Format("0x%08X ", ::GetTickCount());

	// Send to trace window.	
	m_AppWnd.m_AppDlg.Trace(/*strTime +*/ strMsg);

	// Write trace to log file.
	if (m_bLogTrace)
	{
		m_fLogFile.Open(m_strLogFile, GENERIC_WRITE);
		m_fLogFile.Seek(0, FILE_END);
		m_fLogFile.WriteLine(strMsg);
		m_fLogFile.Close();
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
	m_nMaxTrace = m_oIniFile.ReadInt ("Trace", "MaxTrace", m_nMaxTrace);
	m_bLogTrace = m_oIniFile.ReadBool("Trace", "LogTrace", m_bLogTrace);

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
	m_oIniFile.WriteInt ("Trace", "MaxTrace", m_nMaxTrace);
	m_oIniFile.WriteBool("Trace", "LogTrace", m_bLogTrace);

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
				App.Trace("DDE_DISCONNECT: %s, %s", pConv->Service(), pConv->Topic());

				// Send disconnect message.
				pConnection->SendPacket(oPacket);
			}
			catch (CException& /*e*/)
			{
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
	ASSERT(pLink != NULL);
	ASSERT(pData != NULL);

	HCONV      hConv = pLink->Conversation()->Handle();
	CBuffer    oBuffer;
	CMemStream oStream(oBuffer);

	oStream.Create();

	oStream.Write(&hConv, sizeof(hConv));
	oStream << pLink->Item();
	oStream << (uint32) pLink->Format();
	oStream << pData->GetBuffer();

	oStream.Close();

	CNetDDEPacket oPacket(CNetDDEPacket::DDE_ADVISE, oBuffer);

	CBuffer* pBuffer = NULL;

	// Allocate a cache for the link data, if required.
	if (!m_oLinksData.Find(pLink, pBuffer))
	{
		pBuffer = new CBuffer();

		m_oLinksData.Add(pLink, pBuffer);
	}

	// Cache link data.
	*pBuffer = oBuffer;

	// Notify all NetDDEClients...
	for (int i = 0; i < m_aoConnections.Size(); ++i)
	{
		CNetDDESvrPipe* pConnection = m_aoConnections[i];

		if (pConnection->UsesLink(pLink))
		{
			try
			{
				App.Trace("DDE_ADVISE: %s %s", pLink->Item(), CClipboard::FormatName(pLink->Format()));

				// Send advise message.
				pConnection->SendPacket(oPacket);
			}
			catch (CException& /*e*/)
			{
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

void CNetDDESvrApp::OnTimer(uint nTimerID)
{
	ASSERT(nTimerID == m_nTimerID);

	// Guard against re-entrancy.
	if (!g_bInBgProcessing)
	{
		g_bInBgProcessing = true;

		HandleConnects();
		HandleRequests();
		HandleDisconnects();

		g_bInBgProcessing = false;
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
			// Add to pipe collection and reset.
			m_aoConnections.Add(m_pConnection);
			m_pConnection = NULL;
		}
	}
	catch (CPipeException& e)
	{
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

		try
		{
			CNetDDEPacket oPacket;

			if (pConnection->RecvPacket(oPacket))
			{
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
					default:										ASSERT(false);										break;
				}
			}
		}
		catch (CException& /*e*/)
		{
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

	uint16  nProtocol;
	CString strService;
	CString strComputer;
	CString strUser;

	// Decode request message.
	CMemStream oStream(oReqPacket.Buffer());

	oStream.Open();
	oStream.Seek(sizeof(CNetDDEPacket::Header));

	oStream >> nProtocol;
	oStream >> strService;
	oStream >> strComputer;
	oStream >> strUser;

	oStream.Close();

	App.Trace("NETDDE_CLIENT_CONNECT: %u %s %s %s", nProtocol, strService, strComputer, strUser);

	// Save connection details.
	oConnection.Service(strService);
	oConnection.Computer(strComputer);
	oConnection.User(strUser);

	// Send response message.
	CNetDDEPacket oRspPacket(CNetDDEPacket::NETDDE_CLIENT_CONNECT, NULL, 0);

	oConnection.SendPacket(oRspPacket);
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

	App.Trace("DDE_CREATE_CONVERSATION: %s %s", strService, strTopic);

	try
	{
		// Call DDE to create the conversation.
		CDDECltConv* pConv = m_pDDEClient->CreateConversation(strService, strTopic);

		bResult = true;
		hConv   = pConv->Handle();

		// Attach to the connection.
		oConnection.AddConversation(pConv);
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

	App.Trace("DDE_DESTROY_CONVERSATION: 0x%p", hConv);

	try
	{
		// Locate the conversation.
		CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

		if (pConv != NULL)
		{
			// Call DDE to terminate the conversation.
			m_pDDEClient->DestroyConversation(pConv);

			bResult = true;

			// Detach from the connection.
			oConnection.RemoveConversation(pConv);
		}
	}
	catch (CDDEException& /*e*/)
	{
	}

	// Create response message.
	CMemStream oRspStream(oReqPacket.Buffer());

	oRspStream.Create();

	oRspStream << bResult;

	oRspStream.Close();

	// Send response message.
	CNetDDEPacket oRspPacket(CNetDDEPacket::DDE_DESTROY_CONVERSATION, oReqPacket.Buffer());

	oConnection.SendPacket(oRspPacket);
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
	ASSERT(oReqPacket.DataType() == CNetDDEPacket::DDE_START_ADVISE);

	bool      bResult = false;
	CDDELink* pLink = NULL;

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

	App.Trace("DDE_START_ADVISE: %s %s", strItem, CClipboard::FormatName(nFormat));

	try
	{
		// Locate the conversation.
		CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

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

	// Create response message.
	CMemStream oRspStream(oReqPacket.Buffer());

	oRspStream.Create();

	oRspStream << bResult;

	oRspStream.Close();

	// Send response message.
	CNetDDEPacket oRspPacket(CNetDDEPacket::DDE_START_ADVISE, oReqPacket.Buffer());

	oConnection.SendPacket(oRspPacket);

	// Link established?
	if (bResult)
	{
		CBuffer* pBuffer = NULL;

		// Is links' last data cached?
		if (m_oLinksData.Find(pLink, pBuffer))
		{
			CBuffer    oBuffer;
			CMemStream oStream(oBuffer);

			oStream.Create();

			oStream.Write(&hConv, sizeof(hConv));
			oStream << strItem;
			oStream << nFormat;
			oStream << *pBuffer;

			oStream.Close();

			CNetDDEPacket oPacket(CNetDDEPacket::DDE_ADVISE, oBuffer);

			App.Trace("DDE_ADVISE: %s %u", strItem, nFormat);

			// Send links' last advise data.
			oConnection.SendPacket(oPacket);
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

	App.Trace("DDE_STOP_ADVISE: %s %s", strItem, CClipboard::FormatName(nFormat));

	try
	{
		// Locate the conversation.
		CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

		if (pConv != NULL)
		{
			CDDELink* pLink = pConv->FindLink(strItem, nFormat);

			// Call DDE to destroy the link.
			pConv->DestroyLink(pLink);

			// Detach from the connection.
			oConnection.RemoveLink(pLink);

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

	oRspStream.Close();

	// Send response message.
	CNetDDEPacket oRspPacket(CNetDDEPacket::DDE_STOP_ADVISE, oReqPacket.Buffer());

	oConnection.SendPacket(oRspPacket);
}
