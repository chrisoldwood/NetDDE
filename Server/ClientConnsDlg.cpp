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
		CNetDDESvrPipe* pConnection = App.m_aoConnections[i];

		int nLinks = 0;

		// Sum links for all conversations.
		for (int j = 0; j < pConnection->m_aoNetConvs.Size(); ++j)
			nLinks += pConnection->m_aoNetConvs[j]->m_aoLinks.Size();

		int n = m_lvGrid.ItemCount();

		m_lvGrid.InsertItem(n,               pConnection->m_strComputer);
		m_lvGrid.ItemText  (n, USER_NAME,    pConnection->m_strUser);
		m_lvGrid.ItemText  (n, SERVICE_NAME, pConnection->m_strService);
		m_lvGrid.ItemText  (n, CONV_COUNT,   CStrCvt::FormatInt(pConnection->m_aoNetConvs.Size()));
		m_lvGrid.ItemText  (n, LINK_COUNT,   CStrCvt::FormatInt(nLinks));
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
