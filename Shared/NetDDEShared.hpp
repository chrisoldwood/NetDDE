/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESHARED.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	Wrapper to include all the shared application headers.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDESHARED_HPP
#define NETDDESHARED_HPP

/******************************************************************************
**
** System headers.
**
*******************************************************************************
*/

#include "wcl.hpp"		// Windows C++ library and core headers.
#include "ncl.hpp"		// Network & Comms library headers.

/******************************************************************************
**
** Application specific headers.
**
*******************************************************************************
*/

#include "Resource.h"

// Data classes.
#include "LinkValue.hpp"
#include "LinkCache.hpp"
#include "NetDDEDefs.hpp"
#include "NetDDEPacket.hpp"
#include "NetDDESocket.hpp"

#endif //NETDDESHARED_HPP
