/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESVRAPP.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CNetDDESvrApp class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDESVRAPP_HPP
#define NETDDESVRAPP_HPP

/******************************************************************************
** 
** The application class.
**
*******************************************************************************
*/

class CNetDDESvrApp : public CApp, public CDefDDEClientListener
{
public:
	// Template shorthands.
	typedef TPtrArray<CNetDDESvrPipe> CPipes;
	typedef TMap<CDDELink*, CBuffer*> CLinksData;

	//
	// Constructors/Destructor.
	//
	CNetDDESvrApp();
	~CNetDDESvrApp();

	//
	// Members.
	//
	CAppWnd			m_AppWnd;			// Main window.
	CAppCmds		m_AppCmds;			// Command handler.

	CDDEClient*		m_pDDEClient;		// The DDE Client.
	CString			m_strPipeName;		// The server pipe name.
	CPipes			m_aoConnections;	// The client connections.
	CNetDDESvrPipe*	m_pConnection;		// The waiting server connection.
	CLinksData		m_oLinksData;		// Cache of links' data.

	uint			m_nTimerID;			// The background timer ID.

	uint			m_nMaxTrace;		// Maximum lines of trace.
	bool			m_bLogTrace;		// Log trace to file flag.
	CPath			m_strLogFile;		// The log file path.
	CFile			m_fLogFile;			// Trace log file.

	CRect			m_rcLastPos;		// Main window position.

	CIniFile		m_oIniFile;			// .INI FIle

	//
	// Methods.
	//
	void Trace(const char* pszMsg, ...);

	//
	// Constants.
	//
	static const char* VERSION;

protected:
	//
	// Startup and Shutdown template methods.
	//
	virtual	bool OnOpen();
	virtual	bool OnClose();

	//
	// Internal methods.
	//
	void LoadConfig();
	void SaveConfig();

	//
	// Constants.
	//
	static const char* INI_FILE_VER;
	static const uint  BG_TIMER_FREQ;
	static const uint  DEF_MAX_TRACE;

	//
	// IDDEClientListener methods.
	//
	virtual void OnDisconnect(CDDECltConv* pConv);
	virtual void OnAdvise(CDDELink* pLink, const CDDEData* pData);

	//
	// The backgound timer methods.
	//
	virtual void OnTimer(uint nTimerID);

	// Background processing re-entrancy flag.
	static bool g_bInBgProcessing;

	//
	// Background processing methods.
	//
	void HandleConnects();
	void HandleRequests();
	void HandleDisconnects();

	//
	// Packet handlers.
	//
	void OnNetDDEClientConnect(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket);
	void OnNetDDEClientDisconnect(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDECreateConversation(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDEDestroyConversation(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDERequest(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDEStartAdvise(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDEStopAdvise(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket);
};

/******************************************************************************
**
** Global variables.
**
*******************************************************************************
*/

// The application object.
extern CNetDDESvrApp App;

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/


#endif //NETDDESVRAPP_HPP
