FROM ubuntu:bionic

RUN apt update &&\
    apt install -y software-properties-common &&\
    add-apt-repository ppa:ubuntu-toolchain-r/test &&\
    apt update &&\
    apt install -y build-essential qtcreator qt5-default qtdeclarative5-dev qtwebengine5-dev libusb-1.0-0-dev libhidapi-dev pkgconf wget git libopenal-dev &&\
    apt install -y gcc-9 g++-9 &&\
    apt clean &&\
    rm /usr/bin/gcc &&\
    rm /usr/bin/g++ &&\
    ln -s /usr/bin/gcc-9 /usr/bin/gcc &&\
    ln -s /usr/bin/g++-9 /usr/bin/g++ &&\
    export $(dpkg-architecture)

WORKDIR /build

CMD ./scripts/scripts_build-plugin.sh
