#!/bin/sh
htmlfile=$1
serveraddress=$2
if test -z $serveraddress
then
echo "aborted : server needed"
echo "$0 <htmlfile to generate> <ip address of server> <port of server>"
exit 1
fi
serverport=$3
if test -z $serverport
then
echo "aborted : port needed"
echo "$0 <htmlfile to generate> <ip address of server> <port of server>"
exit 1
fi

cat >$htmlfile <<EOF
<html>
  <head>
      <title>Draw Test (1.1)</title>
  </head>
  <body>
      <h1>Draw Test (1.1)</h1>
      <hr>
      <applet archive="ShareDraw.jar" code="lasnier.sharedraw.ShareDrawingEditor.class" width=400
height=400>
      <param name=drawServer value=$serveraddress>
      <param name=drawPort value=$serverport>
 	alt="Your browser understands the &lt;APPLET&gt; tag but isn't
running the applet, for some reason." 	Your browser is completely ignoring
the &lt;APPLET&gt; tag!       </applet>
      <hr>
      <a href="DrawTest.java">The source</a>.
  </body>
</html>
EOF

echo $htmlfile generated
