/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		CLIENTCONNSDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CClientConnsDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef CLIENTCONNSDLG_HPP
#define CLIENTCONNSDLG_HPP

/******************************************************************************
** 
** The dialog used to display the NetDDE Client connections.
**
*******************************************************************************
*/

class CClientConnsDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CClientConnsDlg();
	
protected:
	//
	// Members.
	//
	
	//
	// Controls.
	//
	CListView	m_lvGrid;

	// Columns.
	enum
	{
		COMPUTER_NAME,
		USER_NAME,
		SERVICE_NAME,
		CONV_COUNT,
		LINK_COUNT,
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

#endif // CLIENTCONNSDLG_HPP
