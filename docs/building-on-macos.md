# Building on macOS

macOS cannot build or run this project's compiler. The build runs inside a
Linux container instead; your files stay on the Mac and you edit them normally.

## Why a container is needed

The game was compiled with a patched **gcc-2.96** (an arm-elf dev snapshot from
2000). Two separate problems rule out running it natively:

1. **It cannot be built on macOS.** Its `configure` has no Darwin host entry --
   zero mentions of `darwin` or `apple` anywhere in it. Forcing the build
   through anyway hits a new incompatibility every few minutes: the build script
   hard-codes `--host=x86_64-unknown-linux-gnu`, the link fails on
   `_fputs$UNIX2003` (a legacy symbol modern SDKs dropped), then `sys_nerr` is
   redeclared with a conflicting type. Each is fixable; the list does not end.

2. **Patching it would defeat the point.** This is a *matching* decompilation:
   success is defined as producing byte-identical output. A compiler
   hand-patched to survive macOS is no longer the reference compiler, and any
   mismatch it produces is ambiguous -- our C, or our patches? That ambiguity is
   expensive to debug and impossible to rule out.

So the compiler must run on Linux. And because it is a Linux ELF binary, the
`make` that invokes it must run on Linux too. Hence: everything in the
container.

## What colima is

Docker containers are Linux processes. On Linux, Docker uses the host kernel
directly. On macOS there is no Linux kernel, so Docker needs a small Linux VM to
provide one.

Docker Desktop bundles that VM. **colima** is a lighter, command-line
alternative that does the same job. Either works; colima is smaller and has no
GUI or licence terms.

You need both pieces:

    brew install docker    # the CLI -- talks to a daemon, cannot run containers alone
    brew install colima    # the Linux VM the daemon runs in

If `docker ps` fails with *"Cannot connect to the Docker daemon"*, you have the
CLI but no VM. That is the usual macOS confusion.

## Setup

```sh
brew install colima docker
colima start --cpu 4 --memory 8 --disk 60
```

`colima start` boots the VM; it persists across reboots until you `colima stop`.
Check it with `docker ps` -- an empty table means it is working.

Then build the image (from the repo root):

```sh
docker build -t goldensun-build -f tools/Dockerfile .
```

That installs the Linux toolchain and builds gcc-2.96 from
[camelot-gcc](https://github.com/Coaltergeist/camelot-gcc). It takes a while the
first time and is then cached.

## Daily use

Mount the repo and work inside:

```sh
docker run --rm -it -v "$PWD:/work" -w /work goldensun-build bash
```

You are now at a Linux shell with the repo at `/work`. Run `make`, `make
compare`, anything else, normally. Edits made on the Mac appear immediately
inside; build outputs appear on the Mac. Only the *execution* is Linux.

For one-off commands without an interactive shell:

```sh
docker run --rm -v "$PWD:/work" -w /work goldensun-build make compare
```

A shell alias is worth having:

```sh
alias gsmake='docker run --rm -v "$PWD:/work" -w /work goldensun-build make'
```

## Notes

- **File permissions.** Files the container creates are owned by root by
  default. Passing `--user "$(id -u):$(id -g)"` to `docker run` avoids that.
- **Speed.** File I/O across the VM boundary is slower than native. `colima
  start --mount-type virtiofs` helps noticeably on Apple Silicon.
- **Apple Silicon.** colima emulates x86-64 if asked, but gcc-2.96 builds for
  an `arm-elf` *target* regardless of host architecture, so a native arm64 VM is
  fine and much faster. Only add `--arch x86_64` if something specifically needs
  it.
- **Disk.** The VM image plus the compiler build wants ~10 GB. `--disk 60`
  leaves room.

## Status of this document

The reasoning above is verified: the three macOS build failures were reproduced
directly, and the licence and host-support facts were checked in the vendored
source.

**The container recipe itself is untested** -- colima was not installed on the
machine where this was written. It follows standard Docker practice and
camelot-gcc's own documented Linux requirements, but expect to adjust the
package list on first run. Corrections welcome.
