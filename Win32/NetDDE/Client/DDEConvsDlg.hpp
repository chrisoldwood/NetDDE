/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		DDECONVSDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CDDEConvsDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef DDECONVSDLG_HPP
#define DDECONVSDLG_HPP

/******************************************************************************
** 
** The dialog used to display the list of conversations.
**
*******************************************************************************
*/

class CDDEConvsDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CDDEConvsDlg();
	
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
		TOPIC_NAME,
		LINK_COUNT,
	};

	//
	// Internal methods.
	//
	void Refresh();

	//
	// Message handlers.
	//
	virtual void OnInitDialog();
	virtual bool OnOk();

	void    OnViewLinks();
	void    OnCloseConv();
	LRESULT OnDblClkConv(NMHDR& oMsgHdr);
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // DDECONVSDLG_HPP
