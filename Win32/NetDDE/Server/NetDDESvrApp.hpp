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
	CLinkCache		m_oLinkCache;		// Cache of links values.
	uint			m_nNextConvID;		// Conversation ID generator.

	uint			m_nTimerID;			// The background timer ID.

	bool			m_bTrayIcon;		// Show system tray indicator?
	bool			m_bMinToTray;		// Minimise to system tray?

	bool			m_bTraceConvs;		// Trace conversation create/destroy.
	bool			m_bTraceRequests;	// Trace requests.
	bool			m_bTraceAdvises;	// Trace advise start/stop.
	bool			m_bTraceUpdates;	// Trace advise updates.
	bool			m_bTraceNetConns;	// Trace net connections.
	bool			m_bTraceToWindow;	// Trace output to window.
	int				m_nTraceLines;		// Trace lines in window.
	bool			m_bTraceToFile;		// Trace output to file.
	CString			m_strTraceFile;		// Trace filename.

	CPath			m_strTracePath;		// The trace file path.
	CFile			m_fTraceFile;		// Trace file.

	CRect			m_rcLastPos;		// Main window position.

	CIniFile		m_oIniFile;			// .INI FIle

	uint			m_nPktsSent;		// Packets sent in last second.
	uint			m_nPktsRecv;		// Packets recieved in last second;
	DWORD			m_dwTickCount;		// Status update previous tick count.

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
	static const bool  DEF_TRAY_ICON;
	static const bool  DEF_MIN_TO_TRAY;
	static const bool  DEF_TRACE_CONVS;
	static const bool  DEF_TRACE_REQUESTS;
	static const bool  DEF_TRACE_ADVISES;
	static const bool  DEF_TRACE_UPDATES;
	static const bool  DEF_TRACE_NET_CONNS;
	static const bool  DEF_TRACE_TO_WINDOW;
	static const int   DEF_TRACE_LINES;
	static const bool  DEF_TRACE_TO_FILE;
	static const char* DEF_TRACE_FILE;
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
	void UpdateStats();

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
	void OnDDEExecute(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDEPoke(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket);
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
