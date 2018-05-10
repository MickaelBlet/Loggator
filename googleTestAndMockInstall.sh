#!/bin/sh

if [ `id -u` -ne 0 ] ; then
    echo "Please run as root"
    exit 1
fi

# gmock
sudo apt-get install google-mock
cd /usr/src/gmock
sudo cmake CMakelists.txt
sudo make
sudo cp *.a /usr/lib

# gtest
sudo apt-get install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakelists.txt
sudo make
sudo cp *.a /usr/lib