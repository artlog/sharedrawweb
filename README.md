------------------------------------------------------------------------------------------------------------

KEEP AWAY IF YOU EXPECT TO USE THIS ( USABILITY = 1/10, STABILITY 2/10, COMPATIBILITY 2/10 ).

-------------------------------------------------------------------------------------------------------------


WHAT THIS PROJECT INTENDED TO DO:
--------------------------------

ShareDrawWeb is released under the GPL license terms.

It provide a server for many client to share a same basic black and white vector based drawing.
server is a a java application, client is an applet to embed in a web browser or can be an application too.
Idea come from M.lasnier a student in art that wanted to see the dynamic of creativity when multipe people faces a same white board.
It can be use to quickly share drawing or to have a minimal graphical irc...

It was then changed to be a javascript client and a tomcat embedded service with a webapp that launches a GUI
( local to server ), this is still a work in progress.


ENVIRONMENT:

openjdk 7

TO BUILT IT:
------------

PREREQUISITE

Debian Linux => this has not been tested on any other system and not even on another Debian system that dev's one.

need a jdk to compile
for webapp you need maven

INSTALL
./init.sh
./doit.sh

in this order :
Clean
Ant

Then either 'Run' that is standalone GUI server or 'WebApp_Build_And_Run' that is both tomcat & GUI
WebApp_Build_And_Run will gather maven dependencies over web, this can be slow.

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

IMPORT EXPORT

1) Export to svg

code to export imc to svg is under eclipse
./eclipse_env.sh setup_dev_env
./eclipse_env.sh

java class is org.artisanlogiciel.compression.Main and require two arguments <imc file source> <svg file destination>

Launch GUI server : ./server

Launch webapp & http server : cd webapp; ./launch.sh

2) Create OpenGL or Xlib structures

this is C code 'expander' binary

make
build/expander <source.imc> <nameofdest>
nameofdest file will contain c struct definition to include.

3) Import .IMA files Export to .imc

IMA files are proprietary files for a very old laser show program.
to import , build and run java project ./doit.sh
Clean
Ant
Run

then in java GUI , enter full path of ima file then :
Add { first time only, to have an ouput window }
Import

then it is possible to enter another name with .imc and export it.

4) export svg parts ( svgpath bezier ) into .ima