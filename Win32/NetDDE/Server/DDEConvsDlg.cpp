/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		DDECONVSDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CDDEConvsDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
#include "DDEConvsDlg.hpp"
#include "LinksDlg.hpp"

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

CDDEConvsDlg::CDDEConvsDlg()
	: CDialog(IDD_DDE_CONVS)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_GRID,	&m_lvGrid)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_LINKS, BN_CLICKED, OnViewLinks)
		NFY_CTRLMSG(IDC_GRID,  NM_DBLCLK,  OnDblClkConv)
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
	m_lvGrid.InsertColumn(SERVICE_NAME, "Service", 125, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(TOPIC_NAME,   "Topic",   125, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(LINK_COUNT,   "Links",    70, LVCFMT_RIGHT);

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
	for (int i = 0; i < aoConvs.Size(); ++i)
	{
		char szValue[50];

		CDDECltConv* pConv = aoConvs[i];

		m_lvGrid.InsertItem(i,             pConv->Service());
		m_lvGrid.ItemText  (i, TOPIC_NAME, pConv->Topic());
		m_lvGrid.ItemText  (i, LINK_COUNT, itoa(pConv->NumLinks(), szValue, 10));
		m_lvGrid.ItemData  (i, (LPARAM)pConv->Handle());
	}
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
		AlertMsg("The conversation has been terminated.");
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
