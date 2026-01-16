FROM data.forgejo.org/oci/node:20-bullseye

RUN apt-get update && \
    apt-get install -y \
        build-essential wget ninja-build libglib2.0-dev libslirp-dev \
        python3 python3-pip python3-venv git texinfo netcat-traditional && \
    apt-get clean && \
    pip3 install tomli

ENV QEMU_BASEDIR=/opt/qemu
WORKDIR $QEMU_BASEDIR

RUN git clone -b circle-hcd-dwc2-9.1.1 https://codeberg.org/larchcone/qemu.git && \
    mkdir qemu/build && \
    cd qemu/build && \
    ../configure --without-default-features --enable-slirp --prefix=$QEMU_BASEDIR/qemu-9.1.1 --target-list=arm-softmmu,aarch64-softmmu && \
    make -j$(nproc) && \
    make install && \
    cd $BASEDIR && \
    rm -rf qemu

ENV PATH="$QEMU_BASEDIR/qemu-9.1.1/bin:$PATH"

WORKDIR /opt

RUN wget -nv https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz && \
    wget -nv https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-elf.tar.xz && \
    tar -xf arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz && \
    tar -xf arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-elf.tar.xz && \
    rm arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi.tar.xz arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-elf.tar.xz

ENV PATH="/opt/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi/bin:/opt/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-elf/bin:$PATH"
