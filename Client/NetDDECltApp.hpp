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

	//
	// IDDEClientListener methods.
	//
	virtual bool OnConnect(const char* pszService, const char* pszTopic);
	virtual void OnConnectConfirm(CDDESvrConv* pConv);
	virtual void OnDisconnect(CDDESvrConv* pConv);
	virtual bool OnRequest(CDDEConv* pConv, const char* pszItem, uint nFormat, CDDEData& oData);
	virtual bool OnAdviseStart(CDDEConv* pConv, const char* pszItem, uint nFormat);
	virtual void OnAdviseConfirm(CDDEConv* pConv, CDDELink* pLink);
	virtual void OnAdviseStop(CDDEConv* pConv, CDDELink* pLink);
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
