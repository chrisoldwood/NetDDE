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
	// Members
	//
	CAppWnd			m_AppWnd;			// Main window.
	CAppCmds		m_AppCmds;			// Command handler.

	CDDEClient*		m_pDDEClient;		// The DDE Client.
	CString			m_strPipeName;		// The server pipe name.
	CPipes			m_aoConnections;	// The client connections.
	CNetDDESvrPipe*	m_pConnection;		// The waiting server connection.

	uint			m_nTimerID;			// The background timer ID.

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
	// Preferences.
	//
	CIniFile	m_oIniFile;		// .INI FIle

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

	//
	// IDDEClientListener methods.
	//
	virtual void OnDisconnect(CDDEConv* pConv);
	virtual void OnAdvise(CDDELink* pLink, const CDDEData* pData);

	//
	// The backgound timer methods.
	//
	void OnTimer();
	static void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT nTimerID, DWORD dwTime);

	// Background processing re-entrancy flag.
	static bool g_bInBgProcessing;

	//
	// Background processing methods.
	//
	void HandleConnects();
	void HandleRequests();
	void HandleAdvises();
	void HandleDisconnects();
	void ProcessPacket(CNetDDESvrPipe& oConnection, CNetDDEPacket& oReqPacket);
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
