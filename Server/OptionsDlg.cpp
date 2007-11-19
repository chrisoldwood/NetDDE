/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		OPTIONSDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	COptionsDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "OptionsDlg.hpp"
#include "NetDDESvrApp.hpp"

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
	, m_ebDDETimeOut(false, 6)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_TRAY_ICON,		&m_ckTrayIcon)
		CTRL(IDC_MIN_TO_TRAY,	&m_ckMinToTray)
		CTRL(IDC_DDE_TIMEOUT,	&m_ebDDETimeOut)
		CTRL(IDC_DISCARD_DUPS,	&m_ckDiscard)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_MIN_TO_TRAY, BN_CLICKED, &COptionsDlg::OnMinToTrayClicked)
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
	m_ebDDETimeOut.IntValue(App.m_nDDETimeOut);
	m_ckDiscard.Check(App.m_bDiscardDups);

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
	App.m_bTrayIcon    = m_ckTrayIcon.IsChecked();
	App.m_bMinToTray   = m_ckMinToTray.IsChecked();
	App.m_nDDETimeOut  = m_ebDDETimeOut.IntValue();
	App.m_bDiscardDups = m_ckDiscard.IsChecked();

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
