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

### 1. Install and start the daemon

**You do not need Docker Desktop, and you do not need to launch any UI.**
`colima start` *is* the daemon, run from the command line:

```sh
brew install colima docker
colima start --cpu 4 --memory 4 --disk 20
```

First run downloads a VM image and takes a few minutes; after that it is
seconds. The VM persists across reboots until you `colima stop`.

Verify with `docker ps` -- an empty table means it works.

### 2. If you still get "cannot connect to the Docker daemon"

Check which context is active:

```sh
docker context ls
```

If a stale `desktop-linux` entry is starred, the CLI is still pointed at a
Docker Desktop socket that no longer exists -- a common leftover on machines
where Desktop was uninstalled. `colima start` normally switches the context for
you; if it did not:

```sh
docker context use colima
```

### 3. Sizing the VM

The defaults are generous. On a 8 GB machine, giving the VM half is right --
macOS needs the rest. `--cpu 4 --memory 4` is comfortable for this build;
gcc-2.96 is not memory-hungry.

Disk is the one to watch. `--disk` sets a *ceiling*, not an allocation -- the
image is thin-provisioned and grows as used. Actual consumption here:

| | size |
|---|---|
| colima VM base | ~1.2 GB |
| Docker image (Ubuntu + toolchain) | ~1.5 GB |
| gcc-2.96 source and build | ~3 GB |
| **total** | **~6 GB** |

Have 8 GB free before starting, and more if you want headroom.

### 4. Build the image

From the repo root:

```sh
docker build -t goldensun-build -f tools/Dockerfile .
```

That installs the Linux toolchain and builds gcc-2.96 from
[camelot-gcc](https://github.com/Coaltergeist/camelot-gcc). Slow the first time,
cached afterwards.

## Daily use

Mount the repo and work inside:

```sh
docker run --rm -it --security-opt seccomp=unconfined \
    -v "$PWD:/work" -w /work goldensun-build bash
```

`--security-opt seccomp=unconfined` is what makes the build REPRODUCIBLE, and it
is worth understanding before you drop it. gcc-2.96's optimiser is sensitive to
the process's address layout, so without it roughly one compile in thirty of an
affected file comes out different -- see "Determinism" in the README. The
Makefile runs the compiler under `setarch -R` to pin the layout, and Docker's
default seccomp profile blocks the `personality()` call that needs. The Makefile
probes for this and silently does without, so the build still works unflagged;
it just is not reproducible.

It relaxes one syscall filter on a container that only compiles code from your
own checkout. If you would rather not, omit it and rely on the `git status`
check below instead.

You are now at a Linux shell with the repo at `/work`. Run `make`, `make
compare`, anything else, normally. Edits made on the Mac appear immediately
inside; build outputs appear on the Mac. Only the *execution* is Linux.

For one-off commands without an interactive shell:

```sh
docker run --rm --security-opt seccomp=unconfined \
    -v "$PWD:/work" -w /work goldensun-build make compare
```

A shell alias is worth having:

```sh
alias gsmake='docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build make'
```

**After any build, run `git status`.** Every generated `.s` is tracked, so a
compile that came out differently shows up immediately as a modified file naming
the exact object. It costs nothing and it is the only cheap way to catch the
nondeterminism if you are building without the flag above.

## Notes

- **File permissions.** Files the container creates are owned by root by
  default. Passing `--user "$(id -u):$(id -g)"` to `docker run` avoids that.
- **Speed.** File I/O across the VM boundary is slower than native. `colima
  start --mount-type virtiofs` helps noticeably on Apple Silicon.
- **Apple Silicon.** colima emulates x86-64 if asked, but gcc-2.96 builds for
  an `arm-elf` *target* regardless of host architecture, so a native arm64 VM is
  fine and much faster. Only add `--arch x86_64` if something specifically needs
  it.
- **Disk.** See the sizing table above -- about 6 GB in total, thin-provisioned.
  `colima delete` reclaims all of it if you want the space back.

## Status of this document

Verified end to end on macOS (Darwin 23, Intel, 8 GB RAM, colima 4 CPU / 4 GB):

- the three native build failures were reproduced directly
- licence and host-support facts checked in the vendored source
- colima install, start, context switch and container execution tested
- **`tools/Dockerfile` builds clean on the first attempt**, producing
  `gcc version 2.96 20000731 (experimental)`
- the compiler was probed for the three behaviours agbcc lacks, and has all
  three: Thumb register-offset addressing (`ldr r0, [r1, r0]`), `lr` allocated
  as a scratch register in leaf functions (`mov lr, r0`), and no use of r7
  without needing `-ffixed-r7`

One gotcha worth knowing: **colima mounts `$HOME`, not `/tmp`.** A file written
to macOS `/tmp` (really `/private/tmp`) is invisible inside the container. Keep
scratch files under your home directory.
