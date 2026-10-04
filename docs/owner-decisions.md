# Owner decisions

Standing rulings from the project owner, so agents and coordinators stop
re-litigating them. **A decision here is settled for pass 2.** Several are
explicitly marked for revisiting during pass 3 (depinning) and pass 4
(humanizing/naming) — revisiting those is expected; reopening them mid-batch is
not.

Each entry records what was asked, what was decided, and **the reasoning**, so a
future reader can tell whether their new evidence actually bears on it.

---

## Decided 2026-10-04 (batches 321–322)

### 1. `_MSG_b24` — DECLINED

**Asked:** add `_MSG_b24 = 0xb24;` to `message.sym`, which lands `Func_80a9a5c`
(1 away).

**Decided: NO.** The park stays at 9 pin-free.

**Why.** The standard for a `*.sym` entry is **structural impossibility**: gcc
never pools a constant it can build with an 8-bit `mov`, so a pooled small value
proves the source named a symbol. `_MSG_182`, admitted in the same batch, meets
that bar exactly — SImode, `0x182 = 0xc1 << 1`, so constraint `K` matches at
alternative 3 of `*thumb_movsi_insn` while the pool path `mi` is alternative 6,
and recog takes the first match. **An SImode `const_int` 0x182 can never reach the
pool.**

`0xb24` is neither 8-bit-movable nor shiftable, so **gcc pools it as a literal
anyway**, and the ROM's pool word carries **no relocation**. The bytes are equally
consistent with a plain literal. The supporting argument — that a `symbol_ref`
goes through `force_const_mem` and becomes a real MEM, which carries a sched2
dependence a `const_int` cannot — is sound, but it explains the **scheduling**,
not the **value**. Sufficient, not necessary.

> **The symbol table's whole value is that every entry means "the bytes force
> this".** `_MSG_b24` would be the first that does not, and the cost of admitting
> it is paid by every future entry's credibility.

**Revisit if:** someone finds a ROM pool word for this value that *does* carry a
relocation, or an independent reason the original named it.

### 2. `Field_Halt` under per-file `-ffixed-r11` — DECLINED

**Asked:** add `FIXEDR11_CFLAGS` for one object, making `Field_Halt`
byte-identical. The alternative is **1 of 191** flag-free.

**Decided: NO.** Park at 1 flag-free.

**Why.** Nine objects already carry per-file flag groups, so the mechanism is
precedented — but the counter-evidence is in the candidate's own header: the
landed twin **`Field_Whirlwind` is in the same bank and uses fp freely.** A
per-file flag asserts something about how the original object was built, and a
sibling object in the same bank using the register freely makes that assertion
hard to credit. One encoding is a cheap price for not making it.

**Revisit if:** a second object in that bank independently needs the same flag, or
the 1 turns out to be reachable another way.

### 3. `Func_80979a4` — DEFERRED TO PASS 3

**Asked:** it reads **0 of 47** with one `register int h __asm__("r4")`.

**Decided: park it, land it in pass 3.** The agent reported it as a park rather
than a landing on its own initiative, because it would be a **fourth pin on a
veneer that already carries three** — the prefer-pin-free policy working as
intended.

**Why.** `fakematch.txt` already has 617 rows and `shimcount.py` cannot yet count
them accurately (two known blind spots). Adding a row to a list nobody can measure
is the thing pass 3 exists to stop. The figure is recorded, the mechanism is named
(`global.c:find_reg`'s `used1`, pass 1 dropping `regs_someone_prefers`), and the
landing is one edit away whenever the pin budget is settled.

**Revisit:** in pass 3, by design. See `reports/pass3-depin.md`.

### 4. `Func_8078870`'s volatile cast — RULED A DEVICE

**Asked (implicitly):** ship a body reading 2 of 40 that carries
`*(volatile unsigned short *)p`, against 5 device-free.

**Decided: NO.** The existing body stays at 5; the 2 is recorded as a figure
*about the blocker*.

**Why.** The same lvalue `*(unsigned short *)p` is read **three times** in that
loop — twice plain, once volatile. The qualifier therefore says nothing true about
the data; it exists only to stop cse commoning the third read. And measured, the
brief's body is better *only with* the device: device-free it reads **7**, worse
than the 5 already in the tree.

> **A volatile cast applied to one of several reads of the same lvalue is a
> DEVICE. Applied consistently to an access whose width or ordering is a genuine
> property of the data, it is a LEVER** — which is what pokefirered's *original*
> source did (`u8 *` pointer, `*(vu16 *)p` at the one access whose width was real).

**Revisit if:** someone establishes that the location genuinely is volatile, in
which case all three reads should be.

---

## Still open, carried from earlier batches

- **Promote `DMA3_COPY_RW` to `include/dma.h`** — a naming/placement call.
- **`_CONST_1f` / `_CONST_200`** — symbol-table entries awaiting the same
  structural test as above.
- **`_FILE_e4` / `_FILE_e5`** — structural argument accepted and recorded in
  `file_table.sym`, **withheld on the completion test** (admitting them would
  leave 13 real differences and buy only a relocation line). This is the
  precedent the `_MSG_b24` decision follows.

## Standing standards these decisions rest on

1. **A `*.sym` entry needs structural impossibility, not plausibility** — and the
   argument is **mode-dependent**, because it turns on which `recog` alternative
   matches first, not on whether an immediate alternative exists.
2. **Evidence quality and completion are separate tests.** A good structural
   argument is necessary and not sufficient; the entry must also complete its
   function.
3. **Prefer a pin-free body.** If a landing needs pins and no pin-free landing is
   readily available, park it at its pin-free figure for pass 3.
4. **A device is permitted as an instrument and forbidden as a result.** Keep its
   number, label it as a figure about the blocker, and ship the device-free body.
