/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SERVICEDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CServiceDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef SERVICEDLG_HPP
#define SERVICEDLG_HPP

/******************************************************************************
** 
** The dialof for editing a service configuration.
**
*******************************************************************************
*/

class CServiceDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CServiceDlg();
	
	//
	// Members.
	//
	CString	m_strService;
	CString	m_strServer;
	CString	m_strPipe;
	
protected:
	//
	// Controls.
	//
	CEditBox	m_ebService;
	CEditBox	m_ebServer;
	CEditBox	m_ebPipe;

	//
	// Message handlers.
	//
	virtual void OnInitDialog();
	virtual bool OnOk();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // SERVICEDLG_HPP
