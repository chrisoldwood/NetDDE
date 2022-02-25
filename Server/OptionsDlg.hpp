/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		OPTIONSDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The COptionsDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef OPTIONSDLG_HPP
#define OPTIONSDLG_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/CommonUI.hpp>

/******************************************************************************
** 
** The dialog used to configure the general settings.
**
*******************************************************************************
*/

class COptionsDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	COptionsDlg();
	
protected:
	//
	// Members.
	//
	
	//
	// Controls.
	//
	CCheckBox	m_ckTrayIcon;
	CCheckBox	m_ckMinToTray;
	CDecimalBox	m_ebDDETimeOut;
	CCheckBox	m_ckDiscard;
	CDecimalBox	m_ebServerPort;

	//
	// Message handlers.
	//
	virtual void OnInitDialog();
	virtual bool OnOk();

	void OnMinToTrayClicked();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // OPTIONSDLG_HPP
