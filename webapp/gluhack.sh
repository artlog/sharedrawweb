#!/bin/bash

source ../project_params

mvn install:install-file -Dfile=../dist/lib/sharedrawweb-${project_version}.jar -DgroupId=org.artisanlogiciel -DartifactId=sharedrawweb -Dversion=${project_version} -Dpackaging=jar
