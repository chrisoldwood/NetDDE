/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		DDECONVSDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CDDEConvsDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "DDEConvsDlg.hpp"
#include "LinksDlg.hpp"
#include "NetDDESvrApp.hpp"
#include <NCL/DDEClient.hpp>
#include <NCL/DDECltConv.hpp>
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

CDDEConvsDlg::CDDEConvsDlg()
	: CDialog(IDD_DDE_CONVS)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_GRID,	&m_lvGrid)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_LINKS, BN_CLICKED, &CDDEConvsDlg::OnViewLinks)
		NFY_CTRLMSG(IDC_GRID,  NM_DBLCLK,  &CDDEConvsDlg::OnDblClkConv)
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

void CDDEConvsDlg::OnInitDialog()
{
	// Set grid style.
	m_lvGrid.FullRowSelect(true);

	// Create grid columns.
	m_lvGrid.InsertColumn(SERVICE_NAME, TXT("Service"), 125, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(TOPIC_NAME,   TXT("Topic"),   125, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(LINK_COUNT,   TXT("Links"),    70, LVCFMT_RIGHT);

	// Populate.
	Refresh();
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

bool CDDEConvsDlg::OnOk()
{
	return true;
}

/******************************************************************************
** Method:		Refresh()
**
** Description:	Refresh the list of conversations.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CDDEConvsDlg::Refresh()
{
	// Clear old contents.
	m_lvGrid.DeleteAllItems();

	CDDECltConvs aoConvs;

	// Get the list of conversations.
	App.m_pDDEClient->GetAllConversations(aoConvs);

	// Load grid data.
	for (size_t i = 0, n = aoConvs.size(); i != n; ++i)
	{
		CDDECltConv* pConv = aoConvs[i];

		m_lvGrid.InsertItem(i,             pConv->Service());
		m_lvGrid.ItemText  (i, TOPIC_NAME, pConv->Topic());
		m_lvGrid.ItemText  (i, LINK_COUNT, Core::format(pConv->NumLinks()));
		m_lvGrid.ItemData  (i,             reinterpret_cast<LPARAM>(pConv->Handle()));
	}

	// Select 1st by default.
	if (m_lvGrid.ItemCount() > 0)
		m_lvGrid.Select(0);
}

/******************************************************************************
** Method:		OnViewLinks()
**
** Description:	Links button pressed. Show all links for the selected
**				conversation.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CDDEConvsDlg::OnViewLinks()
{
	// Ignore, if not selection.
	if (!m_lvGrid.IsSelection())
		return;

	// Get the selected conversation.
	HCONV hConv = (HCONV) m_lvGrid.ItemData(m_lvGrid.Selection());

	// Find the conversation.
	CDDECltConv* pConv = App.m_pDDEClient->FindConversation(hConv);

	if (pConv == NULL)
	{
		AlertMsg(TXT("The conversation has been terminated."));
		return;
	}

	CLinksDlg Dlg;

	Dlg.m_pConv = pConv;

	// Show dialog.
	Dlg.RunModal(*this);

	// Re-populate.
	Refresh();
}

/******************************************************************************
** Method:		OnDblClkConv()
**
** Description:	Double-clicked a conversation. Show the links.
**
** Parameters:	rMsgHdr		The WM_NOTIFY msg header.
**
** Returns:		0.
**
*******************************************************************************
*/

LRESULT CDDEConvsDlg::OnDblClkConv(NMHDR& /*oMsgHdr*/)
{
	// If a selection, show links.
	if (m_lvGrid.IsSelection())
		OnViewLinks();

	return 0;
}
