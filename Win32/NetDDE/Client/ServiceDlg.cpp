/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SERVICEDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CServiceDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
#include "ServiceDlg.hpp"

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

CServiceDlg::CServiceDlg()
	: CDialog(IDD_SERVICE)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_SERVICE, &m_ebService)
		CTRL(IDC_SERVER,  &m_ebServer )
		CTRL(IDC_PIPE,    &m_ebPipe   )
		CTRL(IDC_ASYNC,   &m_ckAsync  )
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
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

void CServiceDlg::OnInitDialog()
{
	// Initialise controls.
	m_ebService.Text(m_strService);
	m_ebServer.Text(m_strServer);
	m_ebPipe.Text(m_strPipe);
	m_ckAsync.Check(m_bAsync);
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

bool CServiceDlg::OnOk()
{
	// Validate control values.
	if (m_ebService.TextLength() == 0)
	{
		AlertMsg("Please provide the DDE Service name.");
		m_ebService.Focus();
		return false;
	}

	if (m_ebServer.TextLength() == 0)
	{
		AlertMsg("Please provide the Host Server name.");
		m_ebServer.Focus();
		return false;
	}

	if (m_ebPipe.TextLength() == 0)
	{
		AlertMsg("Please provide the Named Pipe name.");
		m_ebPipe.Focus();
		return false;
	}

	// Get control values.
	m_strService = m_ebService.Text();
	m_strServer  = m_ebServer.Text();
	m_strPipe    = m_ebPipe.Text();
	m_bAsync     = m_ckAsync.IsChecked();

	return true;
}
