#!/bin/bash

DEBIAN_FRONTEND=noninteractive

# add repositories
add-apt-repository ppa:boost-latest/ppa

# update after repositories adding
apt-get update

# common libs
apt install -y \
    build-essential \
    libssl-dev \
    libev-dev \
    libssh2-1-dev \
    libpsl-dev

# conan
wget https://github.com/conan-io/conan/releases/download/2.0.17/conan-ubuntu-64.deb && \
dpkg -i conan-ubuntu-64.deb && \
rm -rf conan-ubuntu-64.deb
