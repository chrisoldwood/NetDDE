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
		CTRL(IDC_REMOTE_NAME,    &m_ebRemName   )
		CTRL(IDC_LOCAL_NAME,     &m_ebLocName   )
		CTRL(IDC_TEXT_ONLY,      &m_ckTextOnly  )
		CTRL(IDC_SERVER,         &m_ebServer    )
		CTRL(IDC_PIPE,           &m_ebPipe      )
		CTRL(IDC_DEF_ADVISE_VAL, &m_ebDefaultVal)
		CTRL(IDC_REQ_VALUE,      &m_ckReqVal    )
		CTRL(IDC_ASYNC,          &m_ckAsync     )
		CTRL(IDC_BAD_ADVISE_VAL, &m_ebFailedVal )
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_REQ_VALUE, BN_CLICKED, OnClickedReqVal)
		CMD_CTRLMSG(IDC_ASYNC,     BN_CLICKED, OnClickedAsync )
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
	// Replace local name, if same as remote name.
	if (m_strLocName == m_strRemName)
		m_strLocName = "";

	// Initialise controls.
	m_ebRemName.Text(m_strRemName);
	m_ebLocName.Text(m_strLocName);
	m_ckTextOnly.Check(m_bTextOnly);
	m_ebServer.Text(m_strServer);
	m_ebPipe.Text(m_strPipe);
	m_ebDefaultVal.Text(m_strDefaultVal);
	m_ckReqVal.Check(m_bReqInitVal);
	m_ckAsync.Check(m_bAsyncAdvises);
	m_ebFailedVal.Text(m_strFailedVal);

	// Initialise dependent controls.
	OnClickedReqVal();
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
	if (m_ebRemName.TextLength() == 0)
	{
		AlertMsg("Please provide the remote DDE Service name.");
		m_ebRemName.Focus();
		return false;
	}

	if (m_ebServer.TextLength() == 0)
	{
		AlertMsg("Please provide the NetDDE Server Hostname.");
		m_ebServer.Focus();
		return false;
	}

	if (m_ebPipe.TextLength() == 0)
	{
		AlertMsg("Please provide the NetDDE Server Pipe name.");
		m_ebPipe.Focus();
		return false;
	}

	// Get control values.
	m_strRemName    = m_ebRemName.Text();
	m_strLocName    = m_ebLocName.Text();
	m_bTextOnly     = m_ckTextOnly.IsChecked();
	m_strServer     = m_ebServer.Text();
	m_strPipe       = m_ebPipe.Text();
	m_strDefaultVal = m_ebDefaultVal.Text();
	m_bReqInitVal   = m_ckReqVal.IsChecked();
	m_bAsyncAdvises = m_ckAsync.IsChecked();
	m_strFailedVal  = m_ebFailedVal.Text();

	// Set local name, if not already.
	if (m_strLocName == "")
		m_strLocName = m_strRemName;

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

/******************************************************************************
** Method:		OnClickedReqVal()
**
** Description:	Request Value checkbox toggled. Update dependent controls.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CServiceDlg::OnClickedReqVal()
{
	bool bReqVal = m_ckReqVal.IsChecked();

	m_ebDefaultVal.Enable(!bReqVal);
}
