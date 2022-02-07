/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SERVICESDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CServicesDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef SERVICESDLG_HPP
#define SERVICESDLG_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/CommonUI.hpp>
#include <vector>

// Forward declarations.
class CNetDDESvcCfg;

// Template shorthands.
typedef std::vector<CNetDDESvcCfg*> CSvcConfigs;

/******************************************************************************
** 
** The dialog used to configure the services.
**
*******************************************************************************
*/

class CServicesDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CServicesDlg();
	~CServicesDlg();
	
	//
	// Members.
	//
	CSvcConfigs	m_aoServices;
	bool		m_bModified;
	
protected:
	//
	// Controls.
	//
	CListView	m_lvServices;
	
	// Columns.
	enum
	{
		LOCAL_NAME,
		REMOTE_NAME,
		SERVER_NAME,
		PORT_NUMBER,
	};

	//
	// Message handlers.
	//
	virtual void OnInitDialog();
	virtual bool OnOk();

	void    OnAdd();
	void    OnEdit();
	void    OnRemove();
	LRESULT OnDblClkServices(NMHDR& oMsgHdr);

	//
	// Internal methods.
	//
	void UpdateButtons();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // SERVICESDLG_HPP
