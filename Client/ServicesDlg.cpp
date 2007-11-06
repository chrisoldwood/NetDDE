/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SERVICESDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CServicesDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "ServicesDlg.hpp"
#include "ServiceDlg.hpp"
#include "NetDDESvcCfg.hpp"
#include "NetDDEDefs.hpp"

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

CServicesDlg::CServicesDlg()
	: CDialog(IDD_SERVICES)
	, m_bModified(false)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_SERVICES,	&m_lvServices)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_ADD,      BN_CLICKED, OnAdd)
		CMD_CTRLMSG(IDC_EDIT,     BN_CLICKED, OnEdit)
		CMD_CTRLMSG(IDC_REMOVE,   BN_CLICKED, OnRemove)
		NFY_CTRLMSG(IDC_SERVICES, NM_DBLCLK,  OnDblClkServices)
	END_CTRLMSG_TABLE
}

/******************************************************************************
** Method:		Destructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CServicesDlg::~CServicesDlg()
{
	m_aoServices.DeleteAll();
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

void CServicesDlg::OnInitDialog()
{
	// Set grid style.
	m_lvServices.FullRowSelect(true);

	// Create grid columns.
	m_lvServices.InsertColumn(0, "Service", 100, LVCFMT_LEFT);
	m_lvServices.InsertColumn(1, "Server",  100, LVCFMT_LEFT);

	// Load current services.
	for (int i = 0; i < m_aoServices.Size(); ++i)
	{
		CNetDDESvcCfg* pService = m_aoServices[i];

		m_lvServices.InsertItem(i,    pService->m_strRemName);
		m_lvServices.ItemText  (i, 1, pService->m_strServer);
		m_lvServices.ItemPtr   (i,    pService);
	}

	// Select 1st by default.
	if (m_lvServices.ItemCount() > 0)
		m_lvServices.Select(0);

	UpdateButtons();
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

bool CServicesDlg::OnOk()
{
	return true;
}

/******************************************************************************
** Method:		OnAdd()
**
** Description:	Add a new service.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CServicesDlg::OnAdd()
{
	CServiceDlg Dlg;

	// Initialise with default values.
	Dlg.m_strRemName    = "";
	Dlg.m_strLocName    = "";
	Dlg.m_bTextOnly     = false;
	Dlg.m_strServer     = "";
	Dlg.m_nPort         = NETDDE_PORT_DEFAULT;
	Dlg.m_strDefaultVal = "#PNDG";
	Dlg.m_bReqInitVal   = false;
	Dlg.m_bAsyncAdvises = false;
	Dlg.m_strFailedVal  = "#LINK";

	// Show config dialog.
	if (Dlg.RunModal(*this) != IDOK)
		return;

	CNetDDESvcCfg* pService = new CNetDDESvcCfg;

	// Save service config and add to collection.
	pService->m_strRemName    = Dlg.m_strRemName;
	pService->m_strLocName    = Dlg.m_strLocName;
	pService->m_bTextOnly     = Dlg.m_bTextOnly;
	pService->m_strServer     = Dlg.m_strServer;
	pService->m_nServerPort   = Dlg.m_nPort;
	pService->m_strInitialVal = Dlg.m_strDefaultVal;
	pService->m_bReqInitalVal = Dlg.m_bReqInitVal;
	pService->m_bAsyncAdvises = Dlg.m_bAsyncAdvises;
	pService->m_strFailedVal  = Dlg.m_strFailedVal;

	m_aoServices.Add(pService);

	// Add service to view.
	int i = m_lvServices.ItemCount();

	m_lvServices.InsertItem(i,    pService->m_strRemName);
	m_lvServices.ItemText  (i, 1, pService->m_strServer);
	m_lvServices.ItemPtr   (i,    pService);

	// Make new service selection.
	m_lvServices.Select(i);

	UpdateButtons();

	m_bModified = true;
}

/******************************************************************************
** Method:		OnEdit()
**
** Description:	Edit the selected service.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CServicesDlg::OnEdit()
{
	// Ignore, if no selection.
	if (m_lvServices.Selection() == LB_ERR)
		return;

	// Get the selected service.
	int            nSel     = m_lvServices.Selection();
	CNetDDESvcCfg* pService = static_cast<CNetDDESvcCfg*>(m_lvServices.ItemPtr(nSel));

	CServiceDlg Dlg;

	// Initialise with current config.
	Dlg.m_strRemName    = pService->m_strRemName;
	Dlg.m_strLocName    = pService->m_strLocName;
	Dlg.m_bTextOnly     = pService->m_bTextOnly;
	Dlg.m_strServer     = pService->m_strServer;
	Dlg.m_nPort         = pService->m_nServerPort;
	Dlg.m_strDefaultVal = pService->m_strInitialVal;
	Dlg.m_bReqInitVal   = pService->m_bReqInitalVal;
	Dlg.m_bAsyncAdvises = pService->m_bAsyncAdvises;
	Dlg.m_strFailedVal  = pService->m_strFailedVal;

	// Show config dialog.
	if (Dlg.RunModal(*this) != IDOK)
		return;

	// Update service config.
	pService->m_strRemName    = Dlg.m_strRemName;
	pService->m_strLocName    = Dlg.m_strLocName;
	pService->m_bTextOnly     = Dlg.m_bTextOnly;
	pService->m_strServer     = Dlg.m_strServer;
	pService->m_nServerPort   = Dlg.m_nPort;
	pService->m_strInitialVal = Dlg.m_strDefaultVal;
	pService->m_bReqInitalVal = Dlg.m_bReqInitVal;
	pService->m_bAsyncAdvises = Dlg.m_bAsyncAdvises;
	pService->m_strFailedVal  = Dlg.m_strFailedVal;

	// Update UI.
	m_lvServices.ItemText(nSel, 0, pService->m_strRemName);
	m_lvServices.ItemText(nSel, 1, pService->m_strServer);

	UpdateButtons();

	m_bModified = true;
}

/******************************************************************************
** Method:		OnRemove()
**
** Description:	Remove the selected service.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CServicesDlg::OnRemove()
{
	// Ignore, if no selection.
	if (m_lvServices.Selection() == LB_ERR)
		return;

	// Get the selected service.
	int            nSel     = m_lvServices.Selection();
	CNetDDESvcCfg* pService = static_cast<CNetDDESvcCfg*>(m_lvServices.ItemPtr(nSel));

	// Delete from collection and view.
	m_aoServices.Delete(m_aoServices.Find(pService));
	m_lvServices.DeleteItem(nSel);

	// Update selection.
	if (nSel == m_lvServices.ItemCount())
		nSel--;

	m_lvServices.Select(nSel);

	UpdateButtons();

	m_bModified = true;
}

/******************************************************************************
** Method:		UpdateButtons()
**
** Description:	Update the state of the action buttons.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CServicesDlg::UpdateButtons()
{
	bool bAnyServices = (m_lvServices.ItemCount() > 0);

	Control(IDC_ADD).Enable(true);
	Control(IDC_EDIT).Enable(bAnyServices);
	Control(IDC_REMOVE).Enable(bAnyServices);
}

/******************************************************************************
** Method:		OnDblClkServices()
**
** Description:	Double-clicked a service. Edit it.
**
** Parameters:	rMsgHdr		The WM_NOTIFY msg header.
**
** Returns:		0.
**
*******************************************************************************
*/

LRESULT CServicesDlg::OnDblClkServices(NMHDR& /*oMsgHdr*/)
{
	// If a selection, edit it.
	if (m_lvServices.IsSelection())
		OnEdit();

	return 0;
}
