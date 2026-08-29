# agbcc-gs

A Golden-Sun-specific build of [agbcc](https://github.com/pret/agbcc) with one
change: Thumb register-offset addressing enabled.

**Stock agbcc is left completely alone.** It stays at `tools/agbcc/bin/agbcc`
and is untouched, so a checkout of this project cannot disturb an agbcc
installation another project depends on. The build uses `agbcc-gs`; nothing
else does.

## Why

agbcc never emits `ldr rD, [rB, rI]`. `GO_IF_LEGITIMATE_ADDRESS` in
`gcc/thumb.h` accepts REG+REG addresses but gates the clause on
`reload_completed`, so the form only becomes legal after register allocation has
finished and is therefore never chosen.

Golden Sun's original compiler uses that addressing mode in **768 functions**,
36% of the ROM, which made all of them unreachable. `regoffset.patch` removes
the gate.

## Evidence it is safe here

| check | result |
|---|---|
| builds with the gate lifted | clean, no ICEs |
| the 80 C files (161 functions) already matching | **byte-identical output** |
| full `make compare` | ROM SHA1 unchanged |

`Func_2f40` shows what changes for a previously-blocked function:

```
ROM              stock agbcc      agbcc-gs
ldr r3,=Data     ldr r1,.L3       ldr r1,.L3
lsl r0,#2        lsl r0,#2        lsl r0,#2
ldr r0,[r3,r0]   add r0,r0,r1     ldr r0,[r1,r0]
bx  lr           ldr r0,[r0]      bx  lr
                 bx  lr
```

Same instruction sequence; only the register differs, which is the ordinary kind
of gap a register pin closes.

## What it does NOT fix

Two other differences between agbcc and Golden Sun's compiler are untouched and
still open:

- **lr allocation.** The ROM uses `lr` as a general-purpose register in leaf
  functions (117 functions). agbcc never does. `Func_b074` is blocked on this.
- **Constant materialisation.** The ROM pools constants above 0xBF; agbcc
  inlines up to 0xFF. A cost-model difference, ~5 functions.

## Rebuilding it

```sh
git clone https://github.com/pret/agbcc
cd agbcc
patch -p0 < /path/to/goldensun/tools/agbcc-gs/regoffset.patch
./build.sh                     # -j1 is deliberate; parallel make races on genrtl.h
cp gcc/agbcc /path/to/goldensun/tools/agbcc-gs/bin/agbcc-gs
```

## Caveat

Safety was measured **for this project's inputs only**. The upstream `???`
comment above the gate invited exactly this experiment, and the surrounding text
explains the gate exists because REG+REG is not offsettable and reload could not
express that. Whether lifting it is correct in general is not established here.
Re-run your own regression check before reusing it.
