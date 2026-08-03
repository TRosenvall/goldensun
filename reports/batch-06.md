# Batch 06 — 5 functions elevated to matching C

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–05 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a **clean** build. Addresses read back from the overlay ELFs.

## The functions

| Function | Address | New source |
|---|---|---|
| `OvlFunc_887_20093b4` | `0x020093b4` | `src/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_c_c_b.c` |
| `OvlFunc_934_2008dcc` | `0x02008dcc` | `src/overlays/rom_7bdeb0/ovl_d20_c_c_a_b.c` |
| `OvlFunc_934_2008de8` | `0x02008de8` | `src/overlays/rom_7bdeb0/ovl_d20_c_c_a_c_b.c` |
| `OvlFunc_942_2008b68` | `0x02008b68` | `src/overlays/rom_7c6bac/ovl_30_c_c_c_b.c` |
| `OvlFunc_968_2008fbc` | `0x02008fbc` | `src/overlays/rom_7f2f14/ovl_30_c_a_c_a_a_c_b.c` |

All five came from splits; the sibling `.s` files must travel with them.

## The distinction that decided three of these

**Two stack arguments with the same value match; two with different values do
not.**

When a call passes the same value for both stack slots, the ROM reuses one
register for both stores — and so does gcc:

    mov r3, #0xf / str r3, [sp] / str r3, [sp, #4]

When they differ, the ROM builds both into separate registers *before* storing
either, and gcc builds one, stores it, and reuses the register:

    rom    mov r3, #0xa / mov r2, #0x54 / str r3, [sp] / str r2, [sp, #4]
    ours   mov r3, #0xa / str r3, [sp] / mov r3, #0x54 / str r3, [sp, #4]

That second shape is the `stack-arg-pair` blocker and it gates **50
functions**. Naming the two values as locals does not separate them; it costs
an instruction instead.

The distinction is invisible in the C — both are just six-argument calls — so
it is noted in each file.

## Tooling: the ranker now screens for blockers

`tools/elevation_candidates.py` reads the assembly for known-unmatchable
shapes and drops them with `--clean`. Five classes, measured across 395 overlay
candidates:

| blocker | count | status |
|---|---|---|
| `pool-tell` | 103 | blocked on **naming** — see below |
| `stack-arg-pair` | 50 | open |
| `arg-interleave` | 28 | nine formulations failed |
| `narrow-mask` | 9 | width solved, ordering open |
| `arg-fill-order` | 8 | open |

`tools/find_families.py` groups functions by identical shape: **50 families
covering 190 functions**, the largest with 30 members. The overlays duplicate
stubs per map rather than sharing them, so one solved member usually gives the
rest — three families account for 14 of the functions elevated so far.

**The filter is a first pass, not a guarantee.** It looks two lines back from
an `lsl` for the `mov` that starts it, so a pair split by three instructions
slips through (`OvlFunc_933_2009874`, parked). Widening the window would flag
ordinary code; the gap is deliberate and recorded.

## The question that would unblock the most

Unchanged and now measured: **103 of 395 overlay candidates** are blocked
because the ROM pools a constant that would fit in an eight-bit `mov`
(`ldr r0, =1`, `ldr r2, =0xf`, `ldr r3, =0x1d`). gcc never pools what it can
`mov`, and always pools a symbol address — so those operands were **symbol
references** in the original.

`message.sym` covers message ids, `file_table.sym` covers file ids. Neither
covers a map id, a text-ink value, or whatever `0x1d` is in the 22-member
family at `ovl_314_a.s`.

**What are those id namespaces?** It is the single highest-value open question
and cannot be answered by experiment.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
