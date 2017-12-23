FROM ubuntu:xenial

RUN apt update && apt install -y software-properties-common

RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test

RUN apt-get update -qq
RUN apt-get install -y git curl make cmake lcov
RUN apt-get install -y ruby g++
RUN gem install coveralls-lcov

RUN apt-get install -y sudo

RUN git clone https://github.com/facebook/proxygen.git && cd proxygen/proxygen && ./deps.sh && ./reinstall.sh
