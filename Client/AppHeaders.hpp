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

#include "wcl.hpp"		// Windows C++ library and core headers.
#include "ncl.hpp"		// Network & Comms library headers.

/******************************************************************************
**
** Application specific headers.
**
*******************************************************************************
*/

#include "Resource.h"

// App data classes.
#include "LinkValue.hpp"
#include "LinkCache.hpp"
#include "NetDDEDefs.hpp"
#include "NetDDEPacket.hpp"
#include "NetDDEPipe.hpp"
#include "NetDDECltPipe.hpp"
#include "NetDDESvcCfg.hpp"
#include "NetDDEService.hpp"

// App GUI classes.
#include "AppCmds.hpp"
#include "AppDlg.hpp"
#include "AppWnd.hpp"
#include "NetDDECltApp.hpp"

#endif //APPHEADERS_HPP
