/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPDLG.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"

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

CAppDlg::CAppDlg()
	: CMainDlg(IDD_MAIN)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_TRACE,	&m_lbTrace)
	END_CTRL_TABLE

	DEFINE_GRAVITY_TABLE
		CTRLGRAV(IDC_TRACE, LEFT_EDGE, TOP_EDGE, RIGHT_EDGE, BOTTOM_EDGE)
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

void CAppDlg::OnInitDialog()
{
	m_lbTrace.Font(CFont(ANSI_FIXED_FONT));
}

/******************************************************************************
** Method:		Trace()
**
** Description:	Add a line of trace to the window.
**
** Parameters:	pszMsg		The trace message.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::Trace(const char* pszMsg)
{
	m_lbTrace.Redraw(false);

	uint i = m_lbTrace.Add(pszMsg);

	m_lbTrace.CurSel(i);

//	if (i >= App.m_nMaxTrace)
//		m_lbTrace.Delete(0);

//	m_lbTrace.Update();
	m_lbTrace.Redraw(true);
	m_lbTrace.Invalidate();
}
