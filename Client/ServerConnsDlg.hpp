/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SERVERCONNSDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CServerConnsDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef SERVERCONNSDLG_HPP
#define SERVERCONNSDLG_HPP

/******************************************************************************
** 
** The dialog used to display the NetDDE Server connections.
**
*******************************************************************************
*/

class CServerConnsDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CServerConnsDlg();
	
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
		SERVICE_NAME,
		COMPUTER_NAME,
		CONV_COUNT,
		LINK_COUNT,
	};

	//
	// Message handlers.
	//
	virtual void OnInitDialog();
	virtual bool OnOk();

	LRESULT OnDblClkServer(NMHDR& oMsgHdr);
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // SERVERCONNSDLG_HPP
