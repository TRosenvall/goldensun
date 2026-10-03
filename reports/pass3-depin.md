# Pass 3 — depinning: scope, and the policy that stops it growing

Pass 3 in the owner's roadmap is depinning. This file is its scope and the
decisions that bound it.

## Scope: 617 fakematch rows, and shimcount UNDERCOUNTS them

`fakematch.txt` carries **617 rows**. Each is a landed, byte-identical file that
reaches its bytes with a construct the project classes as a shim rather than as C.

**`tools/shimcount.py` is the pin authority and it has a known blind spot.** It
counts three classes — `register ... __asm__("rN")` pins, `__asm__(".equ ...")`
symbol declarations, and empty `__asm__ volatile("")` barriers — and **misses a
fourth entirely**: a *value-producing* inline asm such as

    __asm__ __volatile__ ("mov %0, lr" : "=l" (t))

Measured: it reports **zero shims for `GetUnit`**, which ships exactly one of
those. So every pin count taken from that tool is a **lower bound**, and the real
pass-3 scope is at least 617. Fixing shimcount is the first pass-3 task, because
the scope cannot be measured until it is fixed.

## Policy, set by the owner in batch 319

> **Prefer a pin-free body. If a landing needs pins and no pin-free landing is
> readily available, do not land it — park it at its pin-free figure and leave it
> for pass 3.**

`tools/install_batch.py` enforces the mechanical half (a landing with `pins > 0`
and `fakematch: false` is a hard error) and **warns loudly on every pinned
landing**, so the decision is taken deliberately rather than by default.

This replaces the earlier working practice, which was to take pinned landings and
record the pin-free body alongside. That practice is why the seven below exist.

## Grandfathered: the seven pinned landings from batches 317–318

These predate the policy. None of them had a pin-free *landing* available — the
pin-free alternatives were parks at 2 and 7 encodings, not matches — so there was
nothing to prefer at the time.

| function | shims | pin-free alternative |
|---|---|---|
| `GetUnit` | **1, invisible to shimcount** — a transcribed `mov r3, lr` | none; the instruction is a *dead store to a pseudo*, deleted by flow1 before local-alloc, proven unreachable on six spellings including `__builtin_return_address(0)`. Arguably a **transcription**, not a pin. |
| `Func_8005ee0` | 1 register pin | none (pin-free 22, and 4 bytes short) |
| `Func_8015e8c` | 1 register pin | **yes, at 2 of 23** — preserved at `docs/repro-b317/8015e8c_pinfree.c`, residue fully diagnosed. The cheapest depin in the tree. |
| `Func_80a65e4` | 1 `"+r"` barrier | device-free body reads 7; the transposition it closes is settled on **priority**, not a tiebreak, so no source arrangement reaches it |
| `OvlFunc_933_2009874` | 2 register pins | `{q1→r1}` alone is *exactly inert*; only the pair reaches 0 |
| `OvlFunc_921_2009fa4` | 6 register pins | none recorded |
| `OvlFunc_922_200a094` | 6 register pins | none recorded (byte twin of the above — **depin one and port**) |

### Where to start

1. **Fix `shimcount.py`'s inline-asm blind spot.** Until then the scope is unknown.
2. **`Func_8015e8c`** — a measured pin-free body already exists at 2 of 23 with its
   residue named. Closest thing to free.
3. **`OvlFunc_921_2009fa4` / `OvlFunc_922_200a094`** — 12 pins between them and they
   are byte duplicates, so a depin on one ports to the other. Note from batch 318:
   the *unpinned* `int *vp` was load-bearing there (the r5 pin cost an earlier
   repro its figure), so the pins are not uniformly removable — read the ladder in
   both headers first.
4. **Reconsider `GetUnit`'s classification.** A transcribed instruction that no C
   can produce is the `r9` static-chain class, not a pin. If the project accepts
   that distinction, the row stays but the count drops.

## The law that makes depinning tractable

From `docs/elevation.md`, and it is the reason a single-pin greedy will report a
false fixpoint:

> **cse1 unifies two pseudos holding the same `CONST_INT`, and unification needs
> TWO unpinned peers — so a pin defeats it only in company.** Group pin sites by
> the value they materialise and remove each group as a unit. Measured: 20 pins
> inert singly, 65 load-bearing together; and `pinmin.py`'s `157` on
> `OvlFunc_889_2008074` is a **lower bound, not a minimum**.
