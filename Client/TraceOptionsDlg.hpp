/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		TRACEOPTIONSDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CTraceOptionsDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef TRACEOPTIONSDLG_HPP
#define TRACEOPTIONSDLG_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/CommonUI.hpp>
#include "NetDDECltApp.hpp"
#include <WCL/StrCvt.hpp>

/******************************************************************************
** 
** The dialog used to configure the trace settings.
**
*******************************************************************************
*/

class CTraceOptionsDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CTraceOptionsDlg();
	
protected:
	//
	// Controls.
	//
	CCheckBox	m_ckTraceConvs;
	CCheckBox	m_ckTraceRequests;
	CCheckBox	m_ckTraceAdvises;
	CCheckBox	m_ckTraceUpdates;
	CCheckBox	m_ckTraceNetConns;
	CCheckBox	m_ckTraceToWindow;
	CEditBox	m_ebTraceLines;
	CCheckBox	m_ckTraceToFile;
	CEditBox	m_ebTraceFile;

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

/******************************************************************************
** Method:		Default constructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

inline CTraceOptionsDlg::CTraceOptionsDlg()
	: CDialog(IDD_TRACE_OPTIONS)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_CONVERSATIONS,	&m_ckTraceConvs)
		CTRL(IDC_REQUESTS,		&m_ckTraceRequests)
		CTRL(IDC_ADVISES,		&m_ckTraceAdvises)
		CTRL(IDC_UPDATES,		&m_ckTraceUpdates)
		CTRL(IDC_CONNECTIONS,	&m_ckTraceNetConns)
		CTRL(IDC_WINDOW,		&m_ckTraceToWindow)
		CTRL(IDC_MAX_LINES,		&m_ebTraceLines)
		CTRL(IDC_FILE,			&m_ckTraceToFile)
		CTRL(IDC_FILE_NAME,		&m_ebTraceFile)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
	END_CTRLMSG_TABLE
}

/******************************************************************************
** Method:		OnInitDialog()
**
** Description:	Initialise the dialog.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

inline void CTraceOptionsDlg::OnInitDialog()
{
	// Initialise controls.
	m_ckTraceConvs.Check(App.m_bTraceConvs);
	m_ckTraceRequests.Check(App.m_bTraceRequests);
	m_ckTraceAdvises.Check(App.m_bTraceAdvises);
	m_ckTraceUpdates.Check(App.m_bTraceUpdates);
	m_ckTraceNetConns.Check(App.m_bTraceNetConns);
	m_ckTraceToWindow.Check(App.m_bTraceToWindow);
	m_ebTraceLines.Text(CStrCvt::FormatUInt(App.m_nTraceLines));
	m_ckTraceToFile.Check(App.m_bTraceToFile);
	m_ebTraceFile.Text(App.m_strTraceFile);
}

/******************************************************************************
** Method:		OnOk()
**
** Description:	The OK button was pressed.
**
** Parameters:	None.
**
** Returns:		true or false.
**
*******************************************************************************
*/

inline bool CTraceOptionsDlg::OnOk()
{
	// Get control values.
	App.m_bTraceConvs      = m_ckTraceConvs.IsChecked();
	App.m_bTraceRequests   = m_ckTraceRequests.IsChecked();
	App.m_bTraceAdvises    = m_ckTraceAdvises.IsChecked();
	App.m_bTraceUpdates    = m_ckTraceUpdates.IsChecked();
	App.m_bTraceNetConns   = m_ckTraceNetConns.IsChecked();
	App.m_bTraceToWindow   = m_ckTraceToWindow.IsChecked();
	App.m_nTraceLines      = CStrCvt::ParseUInt(m_ebTraceLines.Text());
	App.m_bTraceToFile     = m_ckTraceToFile.IsChecked();
	App.m_strTraceFile     = m_ebTraceFile.Text();

	return true;
}

#endif // TRACEOPTIONSDLG_HPP
