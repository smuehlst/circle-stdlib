FROM data.forgejo.org/oci/node:20-bookworm

RUN apt-get update && \
    apt-get install -y \
        build-essential wget ninja-build libglib2.0-dev libslirp-dev \
        python3 python3-pip python3-venv python3-tomli git texinfo netcat-traditional \
        ninja-build && \
    apt-get clean

ENV QEMU_BASEDIR=/opt/qemu
WORKDIR $QEMU_BASEDIR

RUN git clone -b circle-hcd-dwc2-9.1.1 https://github.com/smuehlst/qemu.git && \
    mkdir qemu/build && \
    cd qemu/build && \
    ../configure --without-default-features --enable-slirp --prefix=$QEMU_BASEDIR/qemu-9.1.1 --target-list=arm-softmmu,aarch64-softmmu && \
    make -j$(nproc) && \
    make install && \
    cd $BASEDIR && \
    rm -rf qemu

ENV PATH="$QEMU_BASEDIR/qemu-9.1.1/bin:$PATH"

WORKDIR /opt

RUN wget https://github.com/Kitware/CMake/releases/download/v3.31.11/cmake-3.31.11-linux-x86_64.tar.gz && \
    tar xf cmake-3.31.11-linux-x86_64.tar.gz && \
    rm cmake-3.31.11-linux-x86_64.tar.gz

ENV PATH="/opt/cmake-3.31.11-linux-x86_64/bin:$PATH"
