/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESVRPIPE.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CNetDDESvrPipe class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDESVRPIPE_HPP
#define NETDDESVRPIPE_HPP

// Template shorthands.
typedef TPtrArray<CDDECltConv> CConvs;
typedef TPtrArray<CDDELink> CLinks;

/******************************************************************************
** 
** The server end of a NetDDE pipe.
**
*******************************************************************************
*/

class CNetDDESvrPipe : public CServerPipe, public CNetDDEPipe
{
public:
	//
	// Constructors/Destructor.
	//
	CNetDDESvrPipe();
	~CNetDDESvrPipe();

	//
	// Properties.
	//
	const CString& Service() const;
	void Service(const CString& strService);

	const CString& Computer() const;
	void Computer(const CString& strComputer);

	const CString& User() const;
	void User(const CString& strUser);

	//
	// Conversation methods.
	//
	void AddConversation(CDDECltConv* pConv);
	void RemoveConversation(CDDECltConv* pConv);
	bool UsesConversation(CDDECltConv* pConv);
	void GetConversations(CConvs& aoConvs);
	int  NumConversations() const;

	//
	// Link methods.
	//
	void AddLink(CDDELink* pLink);
	void RemoveLink(CDDELink* pLink);
	void RemoveAllConvLinks(CDDECltConv* pConv);
	bool UsesLink(CDDELink* pLink);
	void GetLinks(CLinks& aoLinks);
	int  NumLinks() const;

protected:
	//
	// Members.
	//
	CString	m_strService;	// DDE Service name.
	CString	m_strComputer;	// Computer name.
	CString m_strUser;		// User name.
	CConvs	m_aoConvs;		// DDE Conversation list.
	CLinks	m_aoLinks;		// DDE links list.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline const CString& CNetDDESvrPipe::Service() const
{
	return m_strService;
}

inline void CNetDDESvrPipe::Service(const CString& strService)
{
	m_strService = strService;
}

inline const CString& CNetDDESvrPipe::Computer() const
{
	return m_strComputer;
}

inline void CNetDDESvrPipe::Computer(const CString& strComputer)
{
	m_strComputer = strComputer;
}

inline const CString& CNetDDESvrPipe::User() const
{
	return m_strUser;
}

inline void CNetDDESvrPipe::User(const CString& strUser)
{
	m_strUser = strUser;
}

inline int CNetDDESvrPipe::NumConversations() const
{
	return m_aoConvs.Size();
}

inline int CNetDDESvrPipe::NumLinks() const
{
	return m_aoLinks.Size();
}

#endif // NETDDESVRPIPE_HPP
