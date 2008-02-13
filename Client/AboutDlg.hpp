/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		ABOUTDLG.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CAboutDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef ABOUTDLG_HPP
#define ABOUTDLG_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/CommonUI.hpp>

/******************************************************************************
** 
** This is the dialog that display program version and copyright information.
**
*******************************************************************************
*/

class CAboutDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CAboutDlg();
	
protected:
	//
	// Members.
	//
	CLabel		m_txtVersion;
	CURLLabel	m_txtEmail;
	CURLLabel	m_txtWebSite;
	
	//
	// Message processors.
	//
	virtual void OnInitDialog();
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

inline CAboutDlg::CAboutDlg()
	: CDialog(IDD_ABOUT)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_VERSION,	&m_txtVersion)
		CTRL(IDC_EMAIL,		&m_txtEmail  )
		CTRL(IDC_WEBSITE,	&m_txtWebSite)
	END_CTRL_TABLE

	// Set the URL label protocols.
	m_txtEmail.Protocol(TXT("mailto:"));
	m_txtWebSite.Protocol(TXT("http://"));
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

inline void CAboutDlg::OnInitDialog()
{
	m_txtVersion.Text(App.VERSION);
}

#endif //ABOUTDLG_HPP
