/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDEDEFS.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The NetDDE shared types and constants.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDEDEFS_HPP
#define NETDDEDEFS_HPP

/******************************************************************************
** 
** Constants.
**
*******************************************************************************
*/

// The NetDDE pipe name format.
#define NETDDE_PIPE_DEFAULT		"NET_DDE_SERVER"
#define NETDDE_PIPE_FORMAT		"\\\\%s\\pipe\\%s"

// The NetDDE protocol version.
const uint16 NETDDE_PROTOCOL = 2;

#endif // NETDDEDEFS_HPP
