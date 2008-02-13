/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPDLG.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "AppDlg.hpp"
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
** Method:		Clear()
**
** Description:	Clear the window.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::Clear()
{
	m_lbTrace.Reset();
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

void CAppDlg::Trace(const tchar* pszMsg)
{
	m_lbTrace.Redraw(false);

	size_t i = m_lbTrace.Add(pszMsg);

	// Scroll trace, if at bottom.
	if (m_lbTrace.CurSel() == (i-1))
		m_lbTrace.CurSel(i);

	// Delete expired trace lines.
	while (m_lbTrace.Count() > App.m_nTraceLines)
		m_lbTrace.Delete(0);

	m_lbTrace.Redraw(true);
	m_lbTrace.Invalidate();
}
