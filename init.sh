#!/bin/bash

if [[ ! -d  artlog_toolbox ]]
then
    git clone https://github.com/artlog/artlog_toolbox.git artlog_toolbox
    pushd artlog_toolbox
    git checkout
    popd
fi
artlog_toolbox/deploy.sh
