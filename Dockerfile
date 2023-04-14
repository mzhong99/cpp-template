FROM fedora:latest
LABEL maintainer="Matthew Zhong <matthewzhong@logmethods.com>"

RUN dnf update -y
RUN dnf install -y sudo git vim cmake verilator g++ gdb
RUN useradd --create-home --shell /bin/bash validator

USER validator
WORKDIR /home/validator

COPY --chown=validator . .
