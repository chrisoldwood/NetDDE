/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		LINKSDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CLinksDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef LINKSDLG_HPP
#define LINKSDLG_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/CommonUI.hpp>

// Forward declarations.
class CDDECltConv;

/******************************************************************************
** 
** The dialog used to display the links for a conversation.
**
*******************************************************************************
*/

class CLinksDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CLinksDlg();
	
	//
	// Members.
	//
	CDDECltConv*	m_pConv;
	
protected:
	//
	// Controls.
	//
	CListView	m_lvGrid;

	// Columns.
	enum
	{
		ITEM_NAME,
		UPDATE_TIME,
		UPDATE_VALUE,
	};

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

#endif // LINKSDLG_HPP
