/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		OPTIONSDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	COptionsDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
#include "OptionsDlg.hpp"

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

COptionsDlg::COptionsDlg()
	: CDialog(IDD_OPTIONS)
	, m_ebNetTimeOut(false, 6)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_TRAY_ICON,		&m_ckTrayIcon )
		CTRL(IDC_MIN_TO_TRAY,	&m_ckMinToTray)
		CTRL(IDC_NET_TIMEOUT,	&m_ebNetTimeOut)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_MIN_TO_TRAY, BN_CLICKED, OnMinToTrayClicked)
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

void COptionsDlg::OnInitDialog()
{
	// Initialise controls.
	m_ckTrayIcon.Check(App.m_bTrayIcon);
	m_ckMinToTray.Check(App.m_bMinToTray);
	m_ebNetTimeOut.IntValue(App.m_nNetTimeOut);

	// Handle control dependencies.
	OnMinToTrayClicked();
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

bool COptionsDlg::OnOk()
{
	// Get control values.
	App.m_bTrayIcon   = m_ckTrayIcon.IsChecked();
	App.m_bMinToTray  = m_ckMinToTray.IsChecked();
	App.m_nNetTimeOut = m_ebNetTimeOut.IntValue();

	// Update the service timeouts.
	for (int i = 0; i < App.m_aoServices.Size(); ++i)
		App.m_aoServices[i]->m_oConnection.SetTimeOut(App.m_nNetTimeOut);

	return true;
}

/******************************************************************************
** Method:		OnMinToTrayClicked()
**
** Description:	"Minimise To Tray" clicked - Check "Tray Icon" state.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void COptionsDlg::OnMinToTrayClicked()
{
	// If minimising to tray, need icon.
	if (m_ckMinToTray.IsChecked())
		m_ckTrayIcon.Check();

	m_ckTrayIcon.Enable(!m_ckMinToTray.IsChecked());
}
