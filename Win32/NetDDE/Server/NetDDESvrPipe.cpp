/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESVRPIPE.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CNetDDESvrPipe class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"

/******************************************************************************
** Method:		Constructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CNetDDESvrPipe::CNetDDESvrPipe()
	: CNetDDEPipe(this)
{
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

CNetDDESvrPipe::~CNetDDESvrPipe()
{
	Close();
}
