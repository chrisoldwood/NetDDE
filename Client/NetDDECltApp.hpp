/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDECLTAPP.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CNetDDECltApp class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDECLTAPP_HPP
#define NETDDECLTAPP_HPP

/******************************************************************************
** 
** The application class.
**
*******************************************************************************
*/

class CNetDDECltApp : public CApp, public CDefDDEServerListener
{
public:
	// Template shorthands.
	typedef TPtrArray<CNetDDEService> CServices;

	//
	// Constructors/Destructor.
	//
	CNetDDECltApp();
	~CNetDDECltApp();

	//
	// Members
	//
	CAppWnd		m_AppWnd;			// Main window.
	CAppCmds	m_AppCmds;			// Command handler.

	CDDEServer*	m_pDDEServer;		// The DDE Server.
	CServices	m_aoServices;		// The DDE services to bridge.
	CLinkCache 	m_oLinkCache;		// Cache of links values.
	bool		m_bPostedAdviseMsg;	// Posted WM_POST_INITAL_UPDATES?

	uint		m_nTimerID;			// The background timer ID.

	bool		m_bTrayIcon;		// Show system tray indicator?
	bool		m_bMinToTray;		// Minimise to system tray?

	bool		m_bTraceConvs;		// Trace conversation create/destroy?
	bool		m_bTraceRequests;	// Trace requests?
	bool		m_bTraceAdvises;	// Trace advise start/stop?
	bool		m_bTraceUpdates;	// Trace advise updates?
	bool		m_bTraceNetConns;	// Trace net connections?
	bool		m_bTraceToWindow;	// Trace output to window?
	int			m_nTraceLines;		// Trace lines in window.
	bool		m_bTraceToFile;		// Trace output to file?
	CString		m_strTraceFile;		// Trace filename.

	CPath		m_strTracePath;		// The trace file path.
	CFile		m_fTraceFile;		// Trace file.

	CRect		m_rcLastPos;		// Main window position.

	CIniFile	m_oIniFile;			// .INI FIle

	uint		m_nPktsSent;		// Packets sent in last second.
	uint		m_nPktsRecv;		// Packets recieved in last second;
	DWORD		m_dwTickCount;		// Status update previous tick count.

	//
	// Methods.
	//
	CNetDDEService* FindService(const char* pszService) const;
	CNetDDEService* FindService(HCONV hSvrConv) const;
	CNetDDEService* FindService(CDDESvrConv* pConv) const;

	void Trace(const char* pszMsg, ...);

	void Disconnect(CDDESvrConv* pConv);

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

	static const uint  WM_POST_INITIAL_UPDATES;

	//
	// IDDEClientListener methods.
	//
	virtual bool OnWildConnect(CStrArray& astrServices, CStrArray& astrTopics);
	virtual bool OnWildConnectService(const char* pszService, CStrArray& astrTopics);
	virtual bool OnWildConnectTopic(const char* pszTopic, CStrArray& astrServices);
	virtual bool OnConnect(const char* pszService, const char* pszTopic);
	virtual void OnConnectConfirm(CDDESvrConv* pConv);
	virtual void OnDisconnect(CDDESvrConv* pConv);
	virtual bool OnRequest(CDDESvrConv* pConv, const char* pszItem, uint nFormat, CDDEData& oData);
	virtual bool OnAdviseStart(CDDESvrConv* pConv, const char* pszItem, uint nFormat);
	virtual void OnAdviseConfirm(CDDESvrConv* pConv, CDDELink* pLink);
	virtual bool OnAdviseRequest(CDDESvrConv* pConv, CDDELink* pLink, CDDEData& oData);
	virtual void OnAdviseStop(CDDESvrConv* pConv, CDDELink* pLink);
	virtual bool OnExecute(CDDESvrConv* pConv, const CString& strCmd);
	virtual bool OnPoke(CDDESvrConv* pConv, const char* pszItem, uint nFormat, const CDDEData& oData);

	//
	// Message handlers.
	//
	virtual void OnTimer(uint nTimerID);
	virtual void OnThreadMsg(UINT nMsg, WPARAM wParam, LPARAM lParam);

	// Background processing re-entrancy flag.
	static bool g_bInBgProcessing;

	//
	// Background processing methods.
	//
	void HandleNotifications();
	void HandleDisconnects();
	void UpdateStats();
	void OnPostInitalUpdates();

	//
	// Connection handlers.
	//
	void ServerConnect(CNetDDEService* pService);
	void ServerDisconnect(CNetDDEService* pService);

	//
	// Packet handlers.
	//
	void OnNetDDEServerDisconnect(CNetDDEService& oService, CNetDDEPacket& oNfyPacket);
	void OnDDEDisconnect(CNetDDEService& oService, CNetDDEPacket& oNfyPacket);
	void OnDDEAdvise(CNetDDEService& oService, CNetDDEPacket& oNfyPacket);
	void OnDDEStartFailed(CNetDDEService& oService, CNetDDEPacket& oNfyPacket);
};

/******************************************************************************
**
** Global variables.
**
*******************************************************************************
*/

// The application object.
extern CNetDDECltApp App;

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/


#endif //NETDDECLTAPP_HPP
