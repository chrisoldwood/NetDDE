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
	CString	m_strRemName;
	CString	m_strLocName;
	bool	m_bTextOnly;
	CString	m_strServer;
	uint	m_nPort;
	CString	m_strDefaultVal;
	bool	m_bReqInitVal;
	bool	m_bAsyncAdvises;
	CString	m_strFailedVal;
	
protected:
	//
	// Controls.
	//
	CEditBox	m_ebRemName;
	CEditBox	m_ebLocName;
	CCheckBox	m_ckTextOnly;
	CEditBox	m_ebServer;
	CDecimalBox	m_ebPort;
	CEditBox	m_ebDefaultVal;
	CCheckBox	m_ckReqVal;
	CCheckBox	m_ckAsync;
	CEditBox	m_ebFailedVal;

	//
	// Message handlers.
	//
	virtual void OnInitDialog();
	virtual bool OnOk();

	void OnClickedAsync();
	void OnClickedReqVal();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // SERVICEDLG_HPP
