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

void CDDEConvsDlg::OnInitDialog()
{
	// Set grid style.
	m_lvGrid.FullRowSelect(true);

	// Create grid columns.
	m_lvGrid.InsertColumn(SERVICE_NAME, "Service", 100, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(TOPIC_NAME,   "Topic",   100, LVCFMT_LEFT );
	m_lvGrid.InsertColumn(LINK_COUNT,   "Links",    70, LVCFMT_RIGHT);

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

bool CDDEConvsDlg::OnOk()
{
	return true;
}
