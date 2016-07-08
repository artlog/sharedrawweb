WHAT THIS PROJECT INTENDED TO DO:
--------------------------------

ShareDrawWeb is released under the GPL license terms.

It provide a server for many client to share a same basic black and white vector based drawing.
server is a a java application, client is an applet to embed in a web browser or can be an application too.
Idea come from M.lasnier a student in art that wanted to see the dynamic of creativity when multipe people faces a same white board.
It can be use to quickly share drawing or to have a minimal graphical irc...

-------------------------------------------------------------------------------------------------------------

ENVIRONMENT:

openjdk 7

TO BUILT IT:
------------

PREREQUISITE
need a jdk to compile

INSTALL
./init.sh
./doit.sh

in this order :
Clean
Ant
Run

Will start server on a port given on console

[ if 'Run it' does not work, launch ./server 127.0.0.1 ]

Then
[Load]
[Add]
[Add]

then what is drawn in on one windows is displayed / synced on the other and conversely.

For javascript version you need ... an apache

assuming apache document root is /var/www/html/
and <user> cloned sharedrawweb within a parent directory artisanlogiciel
=>

/var/www/html/tests/
 images -> /home/<user>/artisanlogiciel/sharedrawweb/images
 javascript -> /home/<user>/artisanlogiciel/sharedrawweb/javascript
 test.html -> /home/<user>/artisanlogiciel/code/java/sharedrawweb/test.html

_________________________________________________________________________________________________________________

FROM HERE THIS README is obsolete, should be reviewed since forked from savannaa to github ( both projects are mine anyway ).
initialy this project was tested under GNU/Linux Debian 2.2.
with latest CVS of kaffe machine installed in default directory /usr/local/kaffe.
and with gcj (apt-get install gcj )

TO RUN IT :
-----------

PREREQUISITE:
kaffe latest CVS version installed into /usr/local/..

( see example of use to have clearer view )

- start server
./server <dns_name_of_server>
note the listening port you will need to give it for generating client html page

- do the example.html page with
./do_example.sh example.html <dns_name_of_server> <server_port>

- use embeded client script ./client to launch appletviewer
./client example.html

you can copy the ShareDraw.jar on another machine and test ./client on other machine.
it should be able to put it on a http server to generate automaticcaly html page and have
a real draw line server.

EXAMPLE OF USE:
---------------
(line starting with '<' means input from user )
(line starting with # means comment to explain this example : please read them they are valuable )
( other line are generated by code )

#all should be done under you normal user ( ie no need to be root, it is bad anyway for X access ).

#first set working directy to root directory where you untarred this file
<cd ShareDrawPoll

<make
rm -f ./classes/lasnier/sharedraw/*.class
rm -f ShareDraw.jar
use gcj
gcj -C -d classes ./lasnier/sharedraw/*.java
compilation done
/usr/local/kaffe/bin/jar -cvf ShareDraw.jar -C classes/ lasnier/
added manifest
adding: lasnier/sharedraw/BitFieldReader.class (in=1122) (out=693) (deflated 62%)
adding: lasnier/sharedraw/BitFieldWriter.class (in=1104) (out=678) (deflated 61%)
adding: lasnier/sharedraw/DrawException.class (in=219) (out=171) (deflated 78%)
adding: lasnier/sharedraw/DrawLineExpander.class (in=1936) (out=1189) (deflated 61%)
adding: lasnier/sharedraw/DrawLineKompressor.class (in=2284) (out=1451) (deflated 64%)
adding: lasnier/sharedraw/ImageChangeEvent.class (in=494) (out=300) (deflated 61%)
adding: lasnier/sharedraw/ImageChangeListener.class (in=213) (out=150) (deflated 70%)
adding: lasnier/sharedraw/ImageHeuristic.class (in=533) (out=285) (deflated 53%)adding: lasnier/sharedraw/ItemCreatedListener.class (in=208) (out=153) (deflated 74%)
adding: lasnier/sharedraw/NoMoreLineException.class (in=243) (out=179) (deflated 74%)
adding: lasnier/sharedraw/RemoteException.class (in=223) (out=176) (deflated 79%)
adding: lasnier/sharedraw/ResetException.class (in=205) (out=151) (deflated 74%)adding: lasnier/sharedraw/ShareDrawClient.class (in=3649) (out=1740) (deflated 48%)
adding: lasnier/sharedraw/ShareDrawClientMethods.class (in=357) (out=227) (deflated 64%)
adding: lasnier/sharedraw/ShareDrawPoll.class (in=2323) (out=1178) (deflated 51%)
adding: lasnier/sharedraw/ShareDrawServer.class (in=4913) (out=2332) (deflated 47%)
adding: lasnier/sharedraw/ShareDrawServerAccess.class (in=5494) (out=2722) (deflated 50%)
adding: lasnier/sharedraw/ShareDrawServerControl$4.class (in=747) (out=394) (deflated 53%)
adding: lasnier/sharedraw/ShareDrawServerControl$3.class (in=753) (out=414) (deflated 55%)
adding: lasnier/sharedraw/ShareDrawServerControl$2.class (in=703) (out=371) (deflated 53%)
adding: lasnier/sharedraw/ShareDrawServerControl$1.class (in=802) (out=439) (deflated 55%)
adding: lasnier/sharedraw/ShareDrawServerControl.class (in=1542) (out=774) (deflated 50%)
adding: lasnier/sharedraw/ShareDrawServerMethods.class (in=548) (out=281) (deflated 51%)
adding: lasnier/sharedraw/ShareDrawing.class (in=2130) (out=1075) (deflated 50%)adding: lasnier/sharedraw/ShareDrawingEditor.class (in=2458) (out=1266) (deflated 52%)
adding: lasnier/sharedraw/ShareDrawingLine.class (in=2241) (out=1151) (deflated 51%)
adding: lasnier/sharedraw/ShareDrawingLineEditor.class (in=2455) (out=1075) (deflated 44%)
adding: lasnier/sharedraw/ThresholdPolicy.class (in=1218) (out=579) (deflated 48%)

# how work server ? well just ask it !
<./server
usage ./server <serveripname> [jvmtype:(kaffe11,jdk)]

<./server localhost
Kaffe Virtual Machine

Copyright (c) 1996-2002 Kaffe.org project contributors (please see
  the source code for a full list of contributors).  All rights reservced.
Portions Copyright (c) 1996-2002 Transvirtual Technologies, Inc.

The Kaffe virtual machine is free software, licensed under the terms of
the GNU General Public License.  Kaffe.org is a an independent, free software
community project, not directly affiliated with Transvirtual Technologies,
Inc.  Kaffe is a Trademark of Transvirtual Technologies, Inc.  Kaffe comes
with ABSOLUTELY NO WARRANTY.

Engine: Just-in-time v3   Version: 1.1.x-cvs   Java Version: 1.1

ShareDrawWeb is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
any later version.

ShareDrawWeb is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ShareDrawWeb; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

listen on 1051
server control started
# please note the port here it as 1051

# with [add] button you can add a control editor for drawings from client
# with [save] button you will save (and overwrite without prompt) drawing into test.img (uncompressed) and compress.imc (compressed)
# with [load] button you will load previously compressed saved data compress.imc
# with [quit] you stops server

# open another terminal

# how works client ? Well just ask it !
< ./client
Kaffe Virtual Machine

Copyright (c) 1996-2002 Kaffe.org project contributors (please see
  the source code for a full list of contributors).  All rights reservced.
Portions Copyright (c) 1996-2002 Transvirtual Technologies, Inc.

The Kaffe virtual machine is free software, licensed under the terms of
the GNU General Public License.  Kaffe.org is a an independent, free software
community project, not directly affiliated with Transvirtual Technologies,
Inc.  Kaffe is a Trademark of Transvirtual Technologies, Inc.  Kaffe comes
with ABSOLUTELY NO WARRANTY.

Engine: Just-in-time v3   Version: 1.1.x-cvs   Java Version: 1.1

Usage: AppletViewer [-debug] <url|file>

# what is the url ? => it is a html file in our case that contains a applet tag 
# to generate it you case use do_example

# how do_example.sh works ? well just ask it !
<./do_example.sh 
aborted : server needed
./do_example.sh <htmlfile to generate> <ip address of server> <port of server>

# we want to generate example.html file for an applet that will connect to the localhost server on port 1051
< ./do_example.sh example.html localhost 1051 
example.html generated

# launch a client
./client example.html 
Kaffe Virtual Machine

Copyright (c) 1996-2002 Kaffe.org project contributors (please see
  the source code for a full list of contributors).  All rights reservced.
Portions Copyright (c) 1996-2002 Transvirtual Technologies, Inc.

The Kaffe virtual machine is free software, licensed under the terms of
the GNU General Public License.  Kaffe.org is a an independent, free software
community project, not directly affiliated with Transvirtual Technologies,
Inc.  Kaffe is a Trademark of Transvirtual Technologies, Inc.  Kaffe comes
with ABSOLUTELY NO WARRANTY.

Engine: Just-in-time v3   Version: 1.1.x-cvs   Java Version: 1.1

getLine(0)
exception received !
=>lasnier.sharedraw.NoMoreLineException
getLine(0)
exception received !
=>lasnier.sharedraw.NoMoreLineException
getLine(0)
exception received !
=>lasnier.sharedraw.NoMoreLineException
getLine(0)
exception received !
=>lasnier.sharedraw.NoMoreLineException
getLine(0)
exception received !
=>lasnier.sharedraw.NoMoreLineException
getLine(0)
exception received !
=>lasnier.sharedraw.NoMoreLineException

# a appletviewer windows should appears => draw with you mouse a line in
add line received lasnier.sharedraw.ShareDrawingLine@84025f8
getLine(1)
exception received !
=>lasnier.sharedraw.NoMoreLineException

# OK know you can open another console and launch another client and see !
./client example.html 
# you should see the line you draw in other window.
# when playing on both applets you should see same drawing to synchronize





