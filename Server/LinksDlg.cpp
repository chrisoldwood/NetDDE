/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		LINKSDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CLinksDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
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

CLinksDlg::CLinksDlg()
	: CDialog(IDD_LINKS)
	, m_pConv(NULL)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_GRID, &m_lvGrid)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
	END_CTRLMSG_TABLE

	DEFINE_GRAVITY_TABLE
		CTRLGRAV(IDC_GRID, LEFT_EDGE,  TOP_EDGE,    RIGHT_EDGE, BOTTOM_EDGE)
		CTRLGRAV(IDCANCEL, RIGHT_EDGE, BOTTOM_EDGE, RIGHT_EDGE, BOTTOM_EDGE)
	END_GRAVITY_TABLE
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

void CLinksDlg::OnInitDialog()
{
	ASSERT(m_pConv != NULL);

	// Set dialog title.
	Title(CString::Fmt("Links - %s|%s [%d]", m_pConv->Service(), m_pConv->Topic(), m_pConv->NumLinks()));

	// Set grid style.
	m_lvGrid.FullRowSelect(true);

	// Create grid columns.
	m_lvGrid.InsertColumn(ITEM_NAME,    "Item",        125, LVCFMT_LEFT);
	m_lvGrid.InsertColumn(UPDATE_TIME,  "Last Update", 125, LVCFMT_LEFT);
	m_lvGrid.InsertColumn(UPDATE_VALUE, "Last Value",  100, LVCFMT_LEFT);

	// Load links...
	for (int i = 0; i < m_pConv->NumLinks(); ++i)
	{
		CDDELink*   pLink  = m_pConv->GetLink(i);
		CLinkValue* pValue = App.m_oLinkCache.Find(m_pConv, pLink);

		m_lvGrid.InsertItem(i,               pLink->Item());
		m_lvGrid.ItemText  (i, UPDATE_TIME,  pValue->m_tLastUpdate.ToString());
		m_lvGrid.ItemText  (i, UPDATE_VALUE, pValue->m_oLastValue.ToString());
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

bool CLinksDlg::OnOk()
{
	return true;
}
