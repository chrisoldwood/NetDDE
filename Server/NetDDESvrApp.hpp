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

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/App.hpp>
#include <NCL/DefDDEClientListener.hpp>
#include <NCL/IServerSocketListener.hpp>
#include <NCL/IClientSocketListener.hpp>
#include <Legacy/TArray.hpp>
#include <WCL/File.hpp>
#include <WCL/IniFile.hpp>
#include "AppWnd.hpp"
#include "AppCmds.hpp"
#include "LinkCache.hpp"
#include <NCL/TCPSvrSocket.hpp>
#include <NCL/DDEClient.hpp>

// Forward declarations.
class CNetDDEService;
class CNetDDEPacket;
class CNetDDESvrSocket;

/******************************************************************************
** 
** The application class.
**
*******************************************************************************
*/

class CNetDDESvrApp : public CApp, public CDefDDEClientListener,
						public IServerSocketListener, public IClientSocketListener
{
public:
	// Template shorthands.
	typedef TPtrArray<CNetDDESvrSocket> CSockets;

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

	DDE::ClientPtr	m_pDDEClient;		// The DDE Client.
	uint			m_nServerPort;		// The port for connections.
	CSockets		m_aoConnections;	// The client connections.
	CTCPSvrSocket	m_oSvrSocket;		// The listening server socket.
	CLinkCache		m_oLinkCache;		// Cache of links values.
	uint			m_nNextConvID;		// Conversation ID generator.

	uint			m_nTimerID;			// The background timer ID.

	bool			m_bTrayIcon;		// Show system tray indicator?
	bool			m_bMinToTray;		// Minimise to system tray?
	uint			m_nDDETimeOut;		// DDE transaction time-out.
	bool			m_bDiscardDups;		// Discard duplicate updates?

	bool			m_bTraceConvs;		// Trace conversation create/destroy.
	bool			m_bTraceRequests;	// Trace requests.
	bool			m_bTraceAdvises;	// Trace advise start/stop.
	bool			m_bTraceUpdates;	// Trace advise updates.
	bool			m_bTraceNetConns;	// Trace net connections.
	bool			m_bTraceToWindow;	// Trace output to window.
	uint			m_nTraceLines;		// Trace lines in window.
	bool			m_bTraceToFile;		// Trace output to file.
	CString			m_strTraceFile;		// Trace filename.

	CPath			m_strTracePath;		// The trace file path.
	CFile			m_fTraceFile;		// Trace file.

	CRect			m_rcLastPos;		// Main window position.

	CIniFile		m_oIniFile;			// .INI FIle

	uint			m_nPktsSent;		// Packets sent in last second.
	uint			m_nPktsRecv;		// Packets recieved in last second;

	//
	// Methods.
	//
	void Trace(const tchar* pszMsg, ...);

	void CloseConnection(CNetDDESvrSocket* pConnection);

	//
	// Constants.
	//
	static const tchar* VERSION;

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
	static const tchar* INI_FILE_VER;
	static const uint  BG_TIMER_FREQ;
	static const bool  DEF_TRAY_ICON;
	static const bool  DEF_MIN_TO_TRAY;
	static const uint  DEF_DDE_TIMEOUT;
	static const bool  DEF_DISCARD_DUPS;
	static const bool  DEF_TRACE_CONVS;
	static const bool  DEF_TRACE_REQUESTS;
	static const bool  DEF_TRACE_ADVISES;
	static const bool  DEF_TRACE_UPDATES;
	static const bool  DEF_TRACE_NET_CONNS;
	static const bool  DEF_TRACE_TO_WINDOW;
	static const int   DEF_TRACE_LINES;
	static const bool  DEF_TRACE_TO_FILE;
	static const tchar* DEF_TRACE_FILE;
	static const uint  DEF_MAX_TRACE;

	static const uint  WM_POLL_SOCKETS;

	//
	// IDDEClientListener methods.
	//
	virtual void OnDisconnect(CDDECltConv* pConv);
	virtual void OnAdvise(CDDELink* pLink, const CDDEData* pData);

	//
	// IClient/IServerSocketListener methods.
	//
	virtual void OnAcceptReady(CTCPSvrSocket* pSvrSocket);
	virtual void OnReadReady(CSocket* pSocket);
	virtual void OnClosed(CSocket* pSocket, int nReason);
	virtual void OnError(CSocket* pSocket, int nEvent, int nError);

	//
	// Message handlers.
	//
	virtual void OnTimer(uint nTimerID);
	virtual void OnThreadMsg(UINT nMsg, WPARAM wParam, LPARAM lParam);

	//
	// Background processing methods.
	//
	void UpdateStats();
	void OnPollSockets();

	//
	// Packet handlers.
	//
	void OnNetDDEClientConnect(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket);
	void OnNetDDEClientDisconnect(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDECreateConversation(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDEDestroyConversation(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDERequest(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDEStartAdvise(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDEStopAdvise(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDEExecute(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket);
	void OnDDEPoke(CNetDDESvrSocket& oConnection, CNetDDEPacket& oReqPacket);
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
