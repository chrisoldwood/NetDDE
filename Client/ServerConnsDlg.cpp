/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SERVERCONNSDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CServerConnsDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
#include "ServerConnsDlg.hpp"

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

CServerConnsDlg::CServerConnsDlg()
	: CDialog(IDD_SERVER_CONNS)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_GRID, &m_lvGrid)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		NFY_CTRLMSG(IDC_GRID,  NM_DBLCLK,  OnDblClkServer)
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

void CServerConnsDlg::OnInitDialog()
{
	// Set grid style.
	m_lvGrid.FullRowSelect(true);

	// Create grid columns.
	m_lvGrid.InsertColumn(SERVICE_NAME,  "Service",  100, LVCFMT_LEFT);
	m_lvGrid.InsertColumn(COMPUTER_NAME, "Computer", 100, LVCFMT_LEFT);
	m_lvGrid.InsertColumn(CONV_COUNT,    "Convs",     70, LVCFMT_RIGHT);
	m_lvGrid.InsertColumn(LINK_COUNT,    "Links",     70, LVCFMT_RIGHT);

	// Load grid data.
	for (int i = 0; i < App.m_aoServices.Size(); ++i)
	{
		char szValue[50];

		CNetDDEService* pConnection = App.m_aoServices[i];

		m_lvGrid.InsertItem(i,                pConnection->m_oCfg.m_strService);
		m_lvGrid.ItemText  (i, COMPUTER_NAME, pConnection->m_oCfg.m_strServer);
		m_lvGrid.ItemText  (i, CONV_COUNT,    itoa(pConnection->m_aoConvs.Size(), szValue, 10));
		m_lvGrid.ItemText  (i, LINK_COUNT,    itoa(pConnection->m_aoLinks.Size(), szValue, 10));
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

bool CServerConnsDlg::OnOk()
{
	return true;
}

/******************************************************************************
** Method:		OnDblClkServer()
**
** Description:	Double-clicked a server. Show the links.
**
** Parameters:	rMsgHdr		The WM_NOTIFY msg header.
**
** Returns:		0.
**
*******************************************************************************
*/

LRESULT CServerConnsDlg::OnDblClkServer(NMHDR& /*oMsgHdr*/)
{
	return 0;
}
