/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		CLIENTCONNSDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CConnsDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "ClientConnsDlg.hpp"
#include "NetDDESvrApp.hpp"
#include "NetDDESvrSocket.hpp"
#include "NetDDEConv.hpp"
#include <Core/StringUtils.hpp>
#include <Core/Algorithm.hpp>

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

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_DISCONNECT, BN_CLICKED, &CClientConnsDlg::OnCloseConnection)
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

void CClientConnsDlg::OnInitDialog()
{
	// Set grid style.
	m_lvGrid.FullRowSelect(true);

	// Create grid columns.
	m_lvGrid.InsertColumn(SERVICE_NAME,  TXT("DDE Service"), 100, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(CLIENT_NAME,   TXT("Client Host"), 125, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(USER_NAME,     TXT("Client User"), 100, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(CONV_COUNT,    TXT("Convs"),        50, LVCFMT_RIGHT);
	m_lvGrid.InsertColumn(LINK_COUNT,    TXT("Links"),        50, LVCFMT_RIGHT);

	// Populate.
	Refresh();
}

/******************************************************************************
** Method:		Refresh()
**
** Description:	Refresh the list of connections.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CClientConnsDlg::Refresh()
{
	// Clear old contents.
	m_lvGrid.DeleteAllItems();

	// Load grid data.
	for (size_t i = 0; i < App.m_aoConnections.size(); ++i)
	{
		CNetDDESvrSocket* pConnection = App.m_aoConnections[i];

		// Ignore, if now closed.
		if (!pConnection->IsOpen())
			continue;

		size_t nLinks = 0;

		// Sum links for all conversations.
		for (size_t j = 0; j < pConnection->m_aoNetConvs.size(); ++j)
			nLinks += pConnection->m_aoNetConvs[j]->m_aoLinks.size();

		size_t n = m_lvGrid.ItemCount();

		m_lvGrid.InsertItem(n,               pConnection->m_strService);
		m_lvGrid.ItemText  (n, CLIENT_NAME,  pConnection->m_strComputer);
		m_lvGrid.ItemText  (n, USER_NAME,    pConnection->m_strUser);
		m_lvGrid.ItemText  (n, CONV_COUNT,   Core::format(pConnection->m_aoNetConvs.size()));
		m_lvGrid.ItemText  (n, LINK_COUNT,   Core::format(nLinks));
		m_lvGrid.ItemPtr   (n, pConnection);
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

bool CClientConnsDlg::OnOk()
{
	return true;
}

/******************************************************************************
** Method:		OnCloseConnection()
**
** Description:	Disconnect button pressed. Close the selected connection.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CClientConnsDlg::OnCloseConnection()
{
	// Ignore, if no selection.
	if (!m_lvGrid.IsSelection())
		return;

	CNetDDESvrSocket* pConnection = static_cast<CNetDDESvrSocket*>(m_lvGrid.ItemPtr(m_lvGrid.Selection()));

	// Close connection, if still active.
	if (Core::exists(App.m_aoConnections, pConnection) && pConnection->IsOpen())
		App.CloseConnection(pConnection);

	// Re-populate.
	Refresh();
}
