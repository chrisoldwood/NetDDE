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
const uint  CNetDDESvrApp::BG_TIMER_FREQ = 100;

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
	m_AppWnd.Show(m_iCmdShow);

	// Update UI.
	m_AppCmds.UpdateUI();

	// Start the background timer.
	m_nTimerID = ::SetTimer(NULL, 0, BG_TIMER_FREQ, TimerProc);

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
	::KillTimer(NULL, m_nTimerID);

	// Close the waiting connection, if one.
	if (m_pConnection != NULL)
		delete m_pConnection;

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

void CNetDDESvrApp::OnDisconnect(CDDEConv* pConv)
{
	TRACE2("OnDisconnect(%s, %s)\n", pConv->Service(), pConv->Topic());
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

	TRACE2("OnAdviseData(%s, %s)\n", pLink->Item(), pData->GetString());
}

/******************************************************************************
** Method:		TimerProc()
**
** Description:	The timer callback function.
**
** Parameters:	See TimerProc.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CALLBACK CNetDDESvrApp::TimerProc(HWND hWnd, UINT uMsg, UINT nTimerID, DWORD dwTime)
{
	// Forward to instance method.
	if (nTimerID == App.m_nTimerID)
		App.OnTimer();
}

/******************************************************************************
** Method:		OnTimer()
**
** Description:	The timer has gone off, process background tasks.
**				NB: Re-entrancy can be caused when performing DDE requests.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::OnTimer()
{
	// Guard against re-entrancy.
	if (!g_bInBgProcessing)
	{
		g_bInBgProcessing = true;

		HandleConnects();
		HandleRequests();
		HandleAdvises();
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
				ProcessPacket(*pConnection, oPacket);
		}
		catch (CException& /*e*/)
		{
			// Pipe disconnected?.
			pConnection->Close();
		}
	}
}

/******************************************************************************
** Method:		HandleAdvises()
**
** Description:	Handle advises to be sent to NetDDEClients.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::HandleAdvises()
{
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
			// Delete from collection.
			m_aoConnections.Delete(m_aoConnections.Find(pConnection));
		}
	}
}

/******************************************************************************
** Method:		ProcessPacket()
**
** Description:	Process the incoming packet.
**
** Parameters:	oConnection		The connection the packet came from.
**				oReqPacket		The request packet.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrApp::ProcessPacket(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket)
{
	// Decode packet type.
	switch (oReqPacket.DataType())
	{
		// New connection from a client?
		case CNetDDEPacket::NETDDE_CONNECT:
		{
			uint16  nProtocol;
			CString strService;
			CString strComputer;
			CString strUser;

			// Decode message.
			CMemStream oStream(oReqPacket.Buffer());

			oStream.Open();
			oStream.Seek(sizeof(CNetDDEPacket::Header));

			oStream >> nProtocol;
			oStream >> strService;
			oStream >> strComputer;
			oStream >> strUser;

			oStream.Close();

			TRACE4("NETDDE_CONNECT: %u %s %s %s\n", nProtocol, strService, strComputer, strUser);

			// Send response message.
			CNetDDEPacket oRspPacket(CNetDDEPacket::NETDDE_CONNECT, NULL, 0);

			oConnection.SendPacket(oRspPacket);
		}
		break;

		// Client connection closing?
		case CNetDDEPacket::NETDDE_DISCONNECT:
		{
			TRACE("NETDDE_DISCONNECT\n");
		}
		break;

		// Create a new onversation?
		case CNetDDEPacket::DDE_CREATE_CONVERSATION:
		{
			bool bAccept = false;

			CString strService;
			CString strTopic;

			// Decode message.
			CMemStream oStream(oReqPacket.Buffer());

			oStream.Open();
			oStream.Seek(sizeof(CNetDDEPacket::Header));

			oStream >> strService;
			oStream >> strTopic;

			oStream.Close();

			TRACE2("CREATE_CONVERSATION: %s %s\n", strService, strTopic);

			try
			{
//				CDDECltConv* pConv = m_pDDEClient->CreateConversation(strService, strTopic);

				bAccept = true;

//				pConv = NULL;
			}
			catch (CDDEException& /*e*/)
			{
			}

			// Send response message.
			CNetDDEPacket oRspPacket(CNetDDEPacket::DDE_CREATE_CONVERSATION, &bAccept, sizeof(bAccept));

			oConnection.SendPacket(oRspPacket);
		}
		break;

		// Request an item?
		case CNetDDEPacket::DDE_REQUEST:
		{
			bool bError = true;

			HCONV	 hConv;
			CString  strItem;
			uint32   nFormat;
			CDDEData oData;

			// Decode message.
			CMemStream oStream(oReqPacket.Buffer());

			oStream.Open();
			oStream.Seek(sizeof(CNetDDEPacket::Header));

			oStream.Read(&hConv, sizeof(hConv));
			oStream >> strItem;
			oStream >> nFormat;

			oStream.Close();

			TRACE2("DDE_REQUEST: %s %u\n", strItem, nFormat);

			try
			{
				CDDECltConv* pConv = m_pDDEClient->FindConversation(hConv);

				if (pConv != NULL)
					oData = pConv->Request(strItem, nFormat);
			}
			catch (CDDEException& /*e*/)
			{
			}

			// Send response message.
			CNetDDEPacket oRspPacket(CNetDDEPacket::DDE_REQUEST, NULL, 0);

			oConnection.SendPacket(oRspPacket);
		}
		break;

		// Unknown.
		default:
		{
			ASSERT(false);
		}
		break;
	}
}
