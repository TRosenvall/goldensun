# Linux build environment for the Golden Sun decomp.
#
# macOS cannot build or run gcc-2.96 (see docs/building-on-macos.md), so the
# whole build -- compiler and make both -- runs in here. The repo is mounted at
# /work; nothing from it is copied into the image.
#
# Build:  docker build -t goldensun-build -f tools/Dockerfile .
# Use:    docker run --rm -it -v "$PWD:/work" -w /work goldensun-build bash
FROM ubuntu:22.04

# noninteractive stops tzdata prompting and hanging the build
ENV DEBIAN_FRONTEND=noninteractive

# build-essential          host compiler for building gcc-2.96
# binutils-arm-none-eabi   the assembler and linker the ROM build uses
# bison flex texinfo       generator tools some compiler build paths want
# python3                  the repo's own tooling
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        binutils-arm-none-eabi \
        bison flex texinfo \
        python3 python3-pip \
        git make patch file ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Build the compiler from camelot-gcc, which vendors the patched gcc-2.96 that
# reproduces Camelot's codegen. Cloned rather than copied so the image records
# exactly which upstream revision it came from.
ARG CAMELOT_GCC_URL=https://github.com/Coaltergeist/camelot-gcc.git
RUN git clone --depth 1 "$CAMELOT_GCC_URL" /opt/camelot-gcc \
    && cd /opt/camelot-gcc \
    && ./build.sh gcc296

# The stock m4a ("Sappy") audio engine in rom_f9000 is NOT Camelot's code -- it
# is the prebuilt library every GBA licensee linked, compiled with old_agbcc.
# It does not match under gcc-2.96, so the image carries old_agbcc too.
#
# The copies vendored in the repo at tools/agbcc/ are macOS binaries and cannot
# run here; this builds a Linux one.
RUN cd /opt/camelot-gcc && ./build.sh agbcc

# install.sh expects a decomp checkout to drop binaries into. We keep them in
# the image instead, so the mounted repo stays clean and a rebuild of the image
# cannot leave stale binaries behind in your working tree.
RUN mkdir -p /opt/gcc296 /opt/agbcc/bin \
    && cp /opt/camelot-gcc/build-296/gcc/cc1 \
          /opt/camelot-gcc/build-296/gcc/xgcc \
          /opt/camelot-gcc/build-296/gcc/cpp \
          /opt/camelot-gcc/build-296/gcc/tradcpp \
          /opt/gcc296/ \
    && cp /opt/camelot-gcc/agbcc/gcc/old_agbcc /opt/agbcc/bin/ \
    && mkdir -p /opt/agbcc/include \
    && cp -R /opt/camelot-gcc/agbcc/libc/include/. /opt/agbcc/include/ \
    && cp -R /opt/camelot-gcc/agbcc/ginclude/.     /opt/agbcc/include/ \
    && rm -rf /opt/camelot-gcc/build-296

# The Makefile finds the compilers here. Override with `make GCC296_DIR=...` if
# you install them somewhere else.
ENV GCC296_DIR=/opt/gcc296

# AGBCC_DIR MUST be set, not just defaulted in the Makefile.
#
# The Makefile's `AGBCC_DIR ?= tools/agbcc` points at the macOS binaries
# vendored in the repo, which cannot execute in this container -- they fail
# with `Syntax error: "(" unexpected` because the shell tries to run a Mach-O
# file as a script. The failure only shows up on a CLEAN build: src/lib/m4a/
# and src/lib/agb_flash/ change rarely, so their objects survive and the build
# looks green for as long as nobody deletes them.
ENV AGBCC_DIR=/opt/agbcc

WORKDIR /work
CMD ["bash"]
