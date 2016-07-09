#!/bin/bash

# maven project build
mvn package

# run tomcat embed app
bash target/bin/webapp &

sleep 20

# launch firefox to access webapp
firefox http://localhost:8080/test.html &
