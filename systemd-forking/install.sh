#!/bin/bash

## build application without daemonize routine
make -C ../src DAEMON_TYPE=glibc clean mywebserver

## copy executable
sudo mkdir /opt/mywebserver
sudo cp ../src/mywebserver /opt/mywebserver

## link script
SCRIPT=$(pwd)/mywebserver.service
sudo rm -f /etc/systemd/system/mywebserver.service
(cd /etc/systemd/system; sudo ln -sf ${SCRIPT})
