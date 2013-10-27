NetDDE v1.1
===========

Introduction
------------

This DDE client and server can be used to allow a DDE conversation to span a
network.

Installation
------------

Run Setup.exe or
Copy the file NetDDEClient.exe to a folder on the DDE client machine.
Copy the file NetDDEServer.exe to a folder on the DDE server machine.

Configure the NetDDEClient to point to the NetDDEServer machine.

Uninstallation
--------------

Delete the files NetDDE*.*

Documentation
-------------

These utilities allow you to establish a DDE conversation between a DDE Client
and Server where they reside on separate machines on a network. The NetDDE
Client acts as the DDE server and the NetDDE Server makes the requests on its
behalf to the real DDE server. Although Windows has this kind of support built
in I have found it to be difficult to use and unreliable particularly with
DDE servers that provide real-time feeds.

Once you have installed the NetDDE components on the relative machines you need
to configure the NetDDE Client to behave as if it was the real DDE server. In
the "Options | Services..." dialog you can add the DDE servers you want to
remote. Click "Add..." and in the dialog you enter the name of the remote DDE
service (e.g. REUTER, EXCEL etc) and the name of the machine where the NetDDE
Server resides. Click OK on both this dialog and the "Services" dialog and the
connection is ready to use.

The NetDDE client will only establish a connection with the server when a
conversation has been requested. I have written a tool call DDE Query which you
can use to test the connection, or you can use something like EXCEL. When the
connection is active, and if you have the setting on, you will see a system tray
icon which shows activity. The icon will be red if there is no connection at all.

Both the client and server support multiple services and topics from multiple
machines simultaniously.

The other "Service Configuration" dialog options are for enhancing performance
and avoiding local DDE server conflicts:-

If you need to share a DDE server which has the same name as one which resides
on the client, you can set a "Local Name" for the DDE server which is different
from the "Remote Name", e.g. You are sharing an EXCEL spreadsheet and have one
running locally; set the local name to "REMOTEXL".

If you know that the DDE server only responds to requests in the CF_TEXT format
you can check the "Only Supports CF_TEXT" option and all requests for other
formats will be rejected locally thereby reducing network round-trips for failed
requests. EXCEL in particular likes to request many other formats before
falling back to CF_TEXT,

If the DDE server does not send back an inital value when creating an advise
loop (EXCEL does this) you can force this by checking "Request Initial Value".
If this is not checked the NetDDE Client will immediately advise the client with
the "Default Value" to show that no inital value has been sent. This should not
be enabled for some DDE servers (TWINDDE in particular) as it can cause the real
DDE server to lock up if a request is made straight after an advise start.

To avoid lengthy round trips for every advise start (which requires a response
from the NetDDE server) you can check the "Async Advises" option. This forces
the NetDDE Client to accept EVERY request for an advise loop immediately. The
real links are established in the background and any failed links return an
advise with the value in the "Failed Value" box. If you are creating many links
to the DDE server this will vastly speed up the process, however your real DDE
client will not get the usual DDE error for a failed advise.

The NetDDE client caches the last value for every link so that any request for
a linked item will be serviced locally rather than contacting the server. EXCEL
always assumes that a DDE server will not return an inital value and so makes a
request for the current value after establishing a link, unless the DDE server
advises very quickly.

The NetDDE server can ignore duplicate advises which some servers (e.g. BLP,
REUTER, TWINDDE) send. This reduces uneccesary network traffic.

Under "Options | Trace..." you can configure how much diagnostic output appears
in the window and/or gets logged to a file. This is useful for tracking down
problems, but for a real-time feed this can cause additional load on the machine.
You can inspect the active conversations and links in the "View" menu and from
there see the last cached value for an advise. You can also forcibly drop a
conversation which can be useful when testing a DDE Client.

Please be aware that sharing a DDE server across the network may be in violation
of the EULA. Some DDE servers were designed to only allow a single conversation
to restrict their use, remoting them with this utility circumvents that, if in
doubt check the EULA.

Development
-----------

See DevNotes.txt

Contact Details
---------------

Email: gort@cix.co.uk
Web:   http://www.chrisoldwood.com

Chris Oldwood
22nd November 2004
