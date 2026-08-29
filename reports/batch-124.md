# Batch 124 — the address idiom pays out, and the size-check gap is closed

Verified on a clean `make clean && make compare` (with the host-side agbcc
recovery) — `goldensun.gba: OK` — and every address below read out of the linked
ELF.

## Elevated (2258 → 2254)

| function | address | file |
|---|---|---|
| `OvlFunc_935_2008944` | 0x02008944 | `src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_b.c` |
| `OvlFunc_897_200aba0` | 0x0200aba0 | `src/overlays/rom_791794/ovl_30_c_c_a_c_c_c_a_b.c` |
| `Func_801e260` | 0x0801e260 | `src/rom_15000/rom_1de5c_a_b.c` |
| `OvlFunc_964_20091e0` | 0x020091e0 | `src/overlays/rom_7ed0a0/ovl_30_a_a_c_c_a_c_b.c` |
| `OvlFunc_881_20084a0` | 0x020084a0 | `src/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_a_c.c` |
| `OvlFunc_881_20084f0` | 0x020084f0 | same file |
| `OvlFunc_923_200a370` | 0x0200a370 | `src/overlays/rom_7aa430/ovl_1a3c_a_c_c_b.c` |
| `Func_8078500` | 0x08078500 | `src/rom_77000/rom_78414_c_c_a_a_b.c` |

## The batch-123 idiom, used deliberately for the first time

Batch 123 recovered the integer-local address idiom from a bad park and sized
the class at 356 functions. This batch picked four candidates *because* they
carried the shape. **Three matched on the first or second screen**, and the
fourth needed one new rule (below). That is the class converting, not a
one-off.

`OvlFunc_881_20084a0`/`20084f0` are twins — the same function against struct
field 0x8 versus 0x10. The second cost one `sed` and one screen, and since they
were the only two functions in their `.s`, the file became a single `.c` with no
split and no linker-script change at all.

`OvlFunc_923_200a370` also confirmed the batch-123 refinement on derived
constants: it derives one symbol address from another with `sub r3, #0x20`, and
that reproduced from a plain mutated variable, because the base address is
forced into a register by the `ldr` that dereferences it.

## New rule: one integer local per address chain

Writing the idiom, the natural thing is to reuse one scratch variable the way
the ROM reuses r3. On `OvlFunc_923_200a370` that was **10 of 32 differing**, a
pure r1/r3 swap that no statement reordering fixed — I matched the ROM's
statement order exactly and the count did not move. Giving the second chain its
own variable was exact on the next screen. Swapping the declarations instead:
still 10.

The ROM's register reuse is the **allocator** reusing r3, not the source reusing
a variable. Reusing one C variable for two unrelated chains makes one pseudo
with a long live range, and gcc allocates it differently from two short ones.

## `tools/oneref.py` — the size check is now one command

`tryc.py` only runs its `.text` size check when the reference holds a single
function; against a multi-function `.s` it prints `[size check skipped]` and
compares instructions only. Most targets live in multi-function files, so that
check was silently off for most screens — which is how batch 123 shipped a
candidate whose `.text` was 0x9c against 0x98.

    python3 tools/oneref.py <function>
    docker run ... tools/tryc.py cand.c --ref scratch/<function>.s

Every function in this batch was screened that way. The tool refuses ambiguous
names and deliberately does **not** copy a trailing `.section .data`, which
belongs to the file rather than the function and would make the size check
compare the wrong thing.

It is still only necessary, not sufficient — see the batch-123 fakematch that
passed the size check with identical objdump sections and still differed in 58
linked bytes.

## Still owed

- `src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c` carries an `-O1` wildcard that
  is wrong for `OvlFunc_968_2009780`'s TU (26 differing at `-O1`, 2 at `-O2`).
- 12 not-yet-elevated `.s` TUs sit inside Makefile wildcards and will silently
  inherit a flag when elevated.
- The Makefile still holds ~3,300 lines of exact-duplicate rule blocks; the
  build prints "overriding recipe" warnings for them on every run. Flags agree,
  so behaviour is unchanged, but the de-dup is overdue and the warnings are
  noise that would hide a real one.
