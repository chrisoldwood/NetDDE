NetDDE Bridge v2.5
==================

Introduction
------------

This DDE client and server can be used to allow a DDE conversation to span a
network. Unlike Microsoft's Network DDE protocol* (which they later removed in
Windows Vista) this bridge does not require any changes to existing links. It
can also cache data client side and tries to minimise the number of round trips
when setting up a large number of links.

This bridge was originally designed for passing simple financial (numerical)
data around using the CF_TEXT format so that's its sweet-spot.

*The protocol used here bears no relation to the one used by Microsoft so you
cannot mix-and-match the two.

Releases
--------

Stable releases are formally packaged and made available from my Win32 tools page:
http://www.chrisoldwood.com/win32.htm

The latest code is available from my GitHub repo:
https://github.com/chrisoldwood/NetDDE

Installation
------------

- Copy NetDDEClient.exe to a folder on the various DDE client's machines.
- Copy NetDDEServer.exe to a folder on the DDE server's machine.
- Configure the NetDDEClient to point to the NetDDEServer machine.

Uninstallation
--------------

Delete the files NetDDE*.*

Documentation
-------------

There is an HTML based manual - Manual.html.

Development
-----------

See DevNotes.txt

Contact Details
---------------

Email: gort@cix.co.uk
Web:   http://www.chrisoldwood.com

Chris Oldwood
23rd November 2023
