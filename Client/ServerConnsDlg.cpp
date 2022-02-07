/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SERVERCONNSDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CServerConnsDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "ServerConnsDlg.hpp"
#include "NetDDECltApp.hpp"
#include "NetDDEService.hpp"
#include <WCL/StrCvt.hpp>
#include <Core/StringUtils.hpp>

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
		NFY_CTRLMSG(IDC_GRID,  NM_DBLCLK,  &CServerConnsDlg::OnDblClkServer)
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
	m_lvGrid.InsertColumn(SERVICE_NAME,  TXT("Service"),  100, LVCFMT_LEFT);
	m_lvGrid.InsertColumn(REMOTE_NAME,   TXT("Remote"),   100, LVCFMT_LEFT);
	m_lvGrid.InsertColumn(SERVER_NAME,   TXT("Server"),   175, LVCFMT_LEFT);
	m_lvGrid.InsertColumn(PORT_NUMBER,   TXT("Port"),      50, LVCFMT_RIGHT);
	m_lvGrid.InsertColumn(CONV_COUNT,    TXT("Convs"),     50, LVCFMT_RIGHT);
	m_lvGrid.InsertColumn(LINK_COUNT,    TXT("Links"),     50, LVCFMT_RIGHT);

	// Load grid data.
	for (size_t i = 0; i < App.m_aoServices.size(); ++i)
	{
		CNetDDEService* pConnection = App.m_aoServices[i];

		// Only add if connection active.
		if (pConnection->m_aoNetConvs.size() > 0)
		{
			size_t nLinks = 0;

			// Sum links for all conversations.
			for (size_t j = 0; j < pConnection->m_aoNetConvs.size(); ++j)
				nLinks += pConnection->m_aoNetConvs[j]->m_aoLinks.size();

			size_t n = m_lvGrid.ItemCount();

			m_lvGrid.InsertItem(n,                pConnection->m_oCfg.m_strLocName);
			m_lvGrid.ItemText  (n, REMOTE_NAME,   pConnection->m_oCfg.m_strRemName);
			m_lvGrid.ItemText  (n, SERVER_NAME,   pConnection->m_oCfg.m_strServer);
			m_lvGrid.ItemText  (n, PORT_NUMBER,   Core::format(pConnection->m_oCfg.m_nServerPort));
			m_lvGrid.ItemText  (n, CONV_COUNT,    Core::format(pConnection->m_aoNetConvs.size()));
			m_lvGrid.ItemText  (n, LINK_COUNT,    Core::format(nLinks));
		}
	}

	// Select 1st by default.
	if (m_lvGrid.ItemCount() > 0)
		m_lvGrid.Select(0);
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
