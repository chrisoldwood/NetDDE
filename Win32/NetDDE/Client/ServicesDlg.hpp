/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		SERVICESDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CServicesDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef SERVICESDLG_HPP
#define SERVICESDLG_HPP

// Template shorthands.
typedef TPtrArray<CNetDDESvcCfg> CSvcConfigs;

/******************************************************************************
** 
** The dialog used to configure the services.
**
*******************************************************************************
*/

class CServicesDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CServicesDlg();
	~CServicesDlg();
	
	//
	// Members.
	//
	CSvcConfigs	m_aoServices;
	bool		m_bModified;
	
protected:
	//
	// Controls.
	//
	CListView	m_lvServices;
	
	//
	// Message handlers.
	//
	virtual void OnInitDialog();
	virtual bool OnOk();

	void OnAdd();
	void OnEdit();
	void OnRemove();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // SERVICESDLG_HPP
