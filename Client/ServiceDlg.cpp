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
	, m_bAsyncAdvises(false)
	, m_bTextOnly(false)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_SERVICE,        &m_ebService   )
		CTRL(IDC_TEXT_ONLY,      &m_ckTextOnly  )
		CTRL(IDC_SERVER,         &m_ebServer    )
		CTRL(IDC_PIPE,           &m_ebPipe      )
		CTRL(IDC_DEF_ADVISE_VAL, &m_ebDefaultVal)
		CTRL(IDC_REQ_VALUE,      &m_ckReqVal    )
		CTRL(IDC_ASYNC,          &m_ckAsync     )
		CTRL(IDC_BAD_ADVISE_VAL, &m_ebFailedVal )
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_ASYNC, BN_CLICKED, OnClickedAsync)
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
	m_ckTextOnly.Check(m_bTextOnly);
	m_ebServer.Text(m_strServer);
	m_ebPipe.Text(m_strPipe);
	m_ebDefaultVal.Text(m_strDefaultVal);
	m_ckReqVal.Check(m_bReqInitVal);
	m_ckAsync.Check(m_bAsyncAdvises);
	m_ebFailedVal.Text(m_strFailedVal);

	// Initialise dependent controls.
	OnClickedAsync();
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
	m_strService    = m_ebService.Text();
	m_bTextOnly     = m_ckTextOnly.IsChecked();
	m_strServer     = m_ebServer.Text();
	m_strPipe       = m_ebPipe.Text();
	m_strDefaultVal = m_ebDefaultVal.Text();
	m_bReqInitVal   = m_ckReqVal.IsChecked();
	m_bAsyncAdvises = m_ckAsync.IsChecked();
	m_strFailedVal  = m_ebFailedVal.Text();

	return true;
}

/******************************************************************************
** Method:		OnClickedAsync()
**
** Description:	Async checkbox toggled. Update dependent controls.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CServiceDlg::OnClickedAsync()
{
	bool bAsync = m_ckAsync.IsChecked();

	m_ebFailedVal.Enable(bAsync);
}
