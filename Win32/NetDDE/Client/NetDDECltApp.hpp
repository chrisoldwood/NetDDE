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
	typedef TMap<CDDELink*, CBuffer*> CLinksData;

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
	CLinksData	m_oLinksData;		// Cache of links data.

	uint		m_nTimerID;			// The background timer ID.

	uint		m_nMaxTrace;		// Maximum lines of trace.

	CRect		m_rcLastPos;		// Main window position.

	CIniFile	m_oIniFile;			// .INI FIle

	//
	// Methods.
	//
	CNetDDEService* FindService(const char* pszService) const;
	CNetDDEService* FindService(HCONV hSvrConv) const;

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
	virtual bool OnConnect(const char* pszService, const char* pszTopic);
	virtual void OnConnectConfirm(CDDESvrConv* pConv);
	virtual void OnDisconnect(CDDESvrConv* pConv);
	virtual bool OnRequest(CDDESvrConv* pConv, const char* pszItem, uint nFormat, CDDEData& oData);
	virtual bool OnAdviseStart(CDDESvrConv* pConv, const char* pszItem, uint nFormat);
	virtual void OnAdviseConfirm(CDDESvrConv* pConv, CDDELink* pLink);
	virtual bool OnAdviseRequest(CDDESvrConv* pConv, CDDELink* pLink, CDDEData& oData);
	virtual void OnAdviseStop(CDDESvrConv* pConv, CDDELink* pLink);

	//
	// The backgound timer methods.
	//
	virtual void OnTimer(uint nTimerID);

	// Background processing re-entrancy flag.
	static bool g_bInBgProcessing;

	//
	// Background processing methods.
	//
	void HandleNotifications();

	//
	// Packet handlers.
	//
	void OnNetDDEServerDisconnect(CNetDDEService& oService, CNetDDEPacket& oNfyPacket);
	void OnDDEDisconnect(CNetDDEService& oService, CNetDDEPacket& oNfyPacket);
	void OnDDEAdvise(CNetDDEService& oService, CNetDDEPacket& oNfyPacket);
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
