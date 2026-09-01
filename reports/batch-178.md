# Batch 178

Ten functions in one round, **zero new parks, and seven old ones retired.** The
whole batch is a single file — `asm/rom_b5000/rom_bbb0c_a_c_c_a_a.s` no longer
exists — and a single lever, which was found by looking at what the parks said
rather than at what the assembly said.

## Function breakdown

All ten live in one translation unit, [rom_bbb0c_a_c_c_a_a.c](src/rom_b5000/rom_bbb0c_a_c_c_a_a.c).

| # | function | address | offset ticked | screens | previously |
|---|---|---|---|---|---|
| 1 | `Func_80bf250` | `0x080bf250` | `0x132` + companion `0x133` | 1 | **parked** |
| 2 | `Func_80bf2b4` | `0x080bf2b4` | `0x134` + companion `0x135` | 1 | **parked** |
| 3 | `Func_80bf318` | `0x080bf318` | `0x136` + companion `0x137` | 2 | — |
| 4 | `Func_80bf37c` | `0x080bf37c` | `0x138` | 1 | **parked** |
| 5 | `Func_80bf3bc` | `0x080bf3bc` | `0x139` | 1 | **parked** |
| 6 | `Func_80bf400` | `0x080bf400` | `0x13a` | 1 | **parked** |
| 7 | `Func_80bf440` | `0x080bf440` | `0x13b` | 1 | **parked** |
| 8 | `Func_80bf484` | `0x080bf484` | `0x13c` | 3 | — |
| 9 | `Func_80bf4c4` | `0x080bf4c4` | `0x13d`, 3-bit field | 1 | **parked** |
| 10 | `Func_80bf524` | `0x080bf524` | `0x13e`, no call | 1 | — |

Nine of the ten took one screen. The three that were never parked are the ones
that did the work; the seven parked ones fell out of the answer for free.

All ten verified at their exact ROM addresses in the linked ELF after a clean
`make clean && make -j8 && make compare`.

## A "REDUNDANT" REGISTER COPY IS USUALLY A SECOND READ GCC HAS CSEd

Every one of the ten opens the same way:

```
    ldrb r2, [r5]
    mov  r3, r2
    cmp  r3, #0
```

r2 is dead immediately after — the next read of that byte is a fresh
`ldrb r1, [r5]`. So the `mov` does nothing, and it was filed under the
copy-into-a-register class, which this notebook has recorded as a wall three
times. Six of the ten were parked on it. The park went further and recorded a
negative result: splitting the value across two named locals does not produce
the copy, because gcc coalesces them.

Every word of that is true and none of it is the point.

**The copy is a second read of `*p`.** With a local there is one read:

```c
n = *p;
if (n == 0) goto fail;
n += 0xff;
*p = n;                       /* 31 instructions vs the ROM's 32, 26 differing */
```

Written entirely through the pointer there are two, the second is CSEd into the
`mov`, and it matches on the first screen:

```c
if (*p == 0) goto fail;
*p = *p + 0xff;               /* 32 vs 32, MATCH */
```

> **A body that is ONE INSTRUCTION SHORT, with a `mov rA, rB` occupying the
> missing line in the ROM, is a CSEd reload — not an allocation artifact.**
> Count the reads in your C against the reads the ROM makes before its store. If
> the ROM loads once and copies, the source read twice.

### The general form: naming a value is not free

This notebook is full of levers that say *give it a name*. Every one of them
buys a statement boundary — and pays for it by collapsing repeated reads into
one. That price has never been written down, because until now the diffs always
ran the other way.

> When the ROM shows **more** traffic than your version rather than less, try
> **removing** a name before adding another.

### Third reject in four batches

Branch-over-pool (501 functions), r8–r11 (109), and now this. All three are the
same shape: a correct observation — *the compiler doesn't emit mid-body pools*,
*high registers mean an allocation fight*, *that copy does nothing useful* —
promoted into a conclusion about what the source **could not have been**.

None of the three needed a new technique. All three needed one retest.

## THE PARKS-NEAR DISCIPLINE PAID OFF IMMEDIATELY

Batch 177 closed by adding a `parks near` column to `family_siblings.py`, on the
finding that a park which fails on residue B is often the only record of working
lever A.

This batch is the first use of it. `Func_80bf484` was the *unparked* member of a
twin set. Its two parked twins carried the measurement — *31 against 32, one
short* — that made the reading obvious, along with two levers that were still
needed and would have cost several screens to rediscover:

- three `return 0` paths share one block via `goto fail` (three plain returns
  make gcc hoist `mov r0, #0` to the top and emit the exits reversed)
- `*p = *p + 0xff` rather than `(*p)--`, which on an `unsigned char` wraps the
  store in `lsl #24 / lsr #24`

The park was worth more than either elevated sibling would have been.

## One new lever, and one correction to the verification rule

**The named zero goes between the store and the test.** Three of the ten also
clear a companion signed byte, and the ROM materialises their zero with
`mov r7, #0` sitting *after* `lsl r3, #0x18` — i.e. between the counter's store
and the test of its result. Assigning it any earlier in the C moves the
instruction and costs six differing lines. This is the live-range rule from
batch 176 applied at single-statement resolution.

**`.gcc2_compiled.` is a per-TU symbol.** The standing verification — every
elevated address must carry a `.gcc2_compiled.` local symbol at the same address
— quietly assumed one function per file, which has been true until now. A
ten-function TU emits the marker once:

```
080bf250 t .gcc2_compiled.
080bf250 T Func_80bf250
080bf2b4 T Func_80bf2b4      <- no marker, and this is correct
```

For a multi-function TU the marker at the first address proves the unit was
compiled from C, and the remaining symbols at their exact ROM addresses prove
the layout. Do not read its absence on functions 2..N as a failure.

## Why this round is ten and not five

The working rule is 2–5 functions per round. This file is atomic: ten functions,
all interleaved, all one routine. Splitting it ten ways to pace the count would
have added ten linker-script entries and ten files to describe one lever. The
file went in whole.

## Next

Seventeen parks in `src/non_matching/` describe themselves as **one short** or
**one line short**. That phrase is now a specific diagnosis rather than a
symptom, and re-reading those seventeen against it is the cheapest work
currently visible in the tree. Five of them name the copy-into-a-register class
by name.

## State

1,913 functions remain in `asm/`. Seven parks retired, none added — the first
batch in this project's log where the non-matching directory got smaller.
