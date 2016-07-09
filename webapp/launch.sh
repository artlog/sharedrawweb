#!/bin/bash

# maven project build
mvn package

# run tomcat embed app
bash target/bin/webapp
