/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		CLIENTCONNSDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CConnsDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
#include "ClientConnsDlg.hpp"

#ifdef _DEBUG
// For memory leak detection.
#define new DBGCRT_NEW
#endif

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

CClientConnsDlg::CClientConnsDlg()
	: CDialog(IDD_CLIENT_CONNS)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_GRID,	&m_lvGrid)
	END_CTRL_TABLE

//	DEFINE_CTRLMSG_TABLE
//	END_CTRLMSG_TABLE
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

void CClientConnsDlg::OnInitDialog()
{
	// Set grid style.
	m_lvGrid.FullRowSelect(true);

	// Create grid columns.
	m_lvGrid.InsertColumn(COMPUTER_NAME, "Computer", 100, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(USER_NAME,     "User",     100, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(SERVICE_NAME,  "Service",  100, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(CONV_COUNT,    "Convs",     70, LVCFMT_RIGHT);
	m_lvGrid.InsertColumn(LINK_COUNT,    "Links",     70, LVCFMT_RIGHT);

	// Load grid data.
	for (int i = 0; i < App.m_aoConnections.Size(); ++i)
	{
		char szValue[50];

		CNetDDESvrPipe* pConnection = App.m_aoConnections[i];

		m_lvGrid.InsertItem(i,               pConnection->Computer());
		m_lvGrid.ItemText  (i, USER_NAME,    pConnection->User());
		m_lvGrid.ItemText  (i, SERVICE_NAME, pConnection->Service());
		m_lvGrid.ItemText  (i, CONV_COUNT,   itoa(pConnection->NumConversations(), szValue, 10));
		m_lvGrid.ItemText  (i, LINK_COUNT,   itoa(pConnection->NumLinks(), szValue, 10));
	}
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

bool CClientConnsDlg::OnOk()
{
	return true;
}
