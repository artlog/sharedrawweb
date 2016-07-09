#!/bin/bash

mvn install:install-file -Dfile=../dist/lib/sharedrawweb-0.0.1.jar -DgroupId=org.artisanlogiciel -DartifactId=sharedrawweb -Dversion=0.0.1 -Dpackaging=jar
