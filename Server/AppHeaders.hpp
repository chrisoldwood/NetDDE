/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPHEADERS.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	Wrapper to include all the main application headers.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef APPHEADERS_HPP
#define APPHEADERS_HPP

/******************************************************************************
**
** System headers.
**
*******************************************************************************
*/

#include <Core/Common.hpp>		// Core library common headers.
#include <WCL/wcl.hpp>			// Windows C++ library.
#include <NCL/ncl.hpp>			// Network & Comms library.

/******************************************************************************
**
** Application specific headers.
**
*******************************************************************************
*/

#include "Resource.h"

// Shared classes.
#include "NetDDEShared.hpp"

// App data classes.
#include "NetDDEConv.hpp"
#include "NetDDESvrSocket.hpp"

// App GUI classes.
#include "AppCmds.hpp"
#include "AppDlg.hpp"
#include "AppWnd.hpp"
#include "NetDDESvrApp.hpp"

#endif //APPHEADERS_HPP
