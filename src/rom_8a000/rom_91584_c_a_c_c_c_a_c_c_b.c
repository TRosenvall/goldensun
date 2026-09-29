/* Func_8091a58  --  0x08091a58.  BYTE-IDENTICAL, 452 bytes / 181 instructions,
 * and BLOCKED ON TWO message.sym ROWS THAT ARE NOT IN THE TREE YET:
 *
 *     _MSG_96a = 0x096a;
 *     _MSG_978 = 0x0978;
 *
 * objcmp, the authority (production -O2, no flag probe used anywhere here):
 *
 *   python3 tools/objcmp.py <this> asm/rom_8a000/rom_91584_c_a_c_c_c_a_c_c.s \
 *       --func Func_8091a58
 *     XX ENCODINGS differ in 2 place(s) (ref 188, ours 188)
 *        first at index 183: ref 0000096a  ours 00000000
 *     XX RELOCATIONS differ
 *        ours carries two extra R_ARM_ABS32: _MSG_96a at 0x1b0, _MSG_978 at 0x1b8
 *
 * SIZE IS SILENT (452 = 452) and the 34 R_ARM_THM_CALL records are at IDENTICAL
 * offsets in both objects.  The two differing "encodings" ARE THE TWO POOL WORDS
 * themselves: zero in our object with an R_ARM_ABS32 to an absolute symbol, the
 * literal in the reference.  That is the BENIGN case recorded in
 * docs/elevation.md -- a zero-addend R_ARM_ABS32 to an absolute .sym value links
 * byte-identically to the literal.  PROVEN, not asserted:
 *
 *     as the reference .s and this .c, objcopy -j .text -O binary both, patch our
 *     0x1b0 <- 0x96a and 0x1b8 <- 0x978, compare the 452 bytes
 *       -> EQUAL   (ref Func_8091a58 size 0x1c4, ours 0x1c4)
 *
 * SIZE 452 bytes and 181 instructions are stated SEPARATELY and both match; tryc
 * reads 192 lines against 192 with the only disagreeing lines the three
 * `ldr rX, =_MSG_*` sites, which tryc cannot see through because it normalises
 * pool loads to `=value`.
 *
 * SHIMS: 0 (tools/shimcount.py -- no `register ... __asm__` pin, no `.equ`).
 *
 * SPLIT SHAPE.  asm/rom_8a000/rom_91584_c_a_c_c_c_a_c_c.s holds TWO functions,
 * Func_80919d8 (line 8) and Func_8091a58 (line 81), counted with the anchored
 * `^[[:space:]]*\.?thumb_func_start(_noalign)?[[:space:]]+` pattern.  Landing
 * needs `python3 tools/split_s.py asm/rom_8a000/rom_91584_c_a_c_c_c_a_c_c.s
 * Func_8091a58`, which gives
 *     _c_c_a.s   Func_80919d8   (stays hand-written)
 *     _c_c_b.c   Func_8091a58   (THIS FILE)
 *     _c_c_c.s   empty, not written -- Func_8091a58 is the LAST function
 * and one new object line in the linker script where the old one was.
 * `python3 tools/datacheck.py asm/rom_8a000/rom_91584_c_a_c_c_c_a_c_c.s` prints
 * NOTHING: no data sections, and no `.global` data exports to preserve.
 *
 * WHAT THE FUNCTION IS.  The "your pack is full" hand-off.  Try to give the
 * player item `item`; if the bag is full (_GiveItem returns -1) it says lines
 * 0x96a and 0x977, then loops the sell/discard menu (base 0x978) until either
 * the player frees a slot -- in which case the freed slot's stack is consumed
 * _GetInventoryItem times through _Func_8078948, the item is handed over for
 * real and 0x96a/0x96b report who got it -- or the player refuses, which either
 * re-prompts (0x97c for an undroppable item, bit 3 of _GetItemInfo()+0x03) or
 * takes the confirmation prompt at 0x979/Func_8091d84 and drops the item
 * (_Func_8078ad0) with line 0x97a.  Every exit restores the scene word cached at
 * iwram_3001ebc[0xec<<1] and returns the receiving party member, or -1.
 *
 * THE FIVE LEVERS, EACH WITH ITS MEASURED DELTA (tryc "instructions in
 * disagreeing regions" while navigating, objcmp encodings at the end).
 *
 * 1. _MSG_978 AND _MSG_96a ARE SYMBOLS, NOT LITERALS -- the base-plus-arithmetic
 *    signature.  The ROM holds 0x978 in r7 across the sell loop and derives the
 *    other three ids with `add r0, r7, #4` / `#1` / `#2`; it holds 0x96a in r5 in
 *    the bag-had-room arm and derives 0x96b with `add r0, r5, #1`.  cse.c relates
 *    two CONSTs only through get_related_value, which needs a SYMBOL_REF, so no
 *    spelling of two literals can produce those `add`s.  MEASURED:
 *      both symbols (this file) ............................  0 real, 2 benign
 *      _MSG_978 only, 0x96a/0x96b literal .................  +3 encodings
 *      _MSG_96a only, 0x978 literal .......................  +2 encodings
 *      both literal .......................................  +5 encodings
 *    THE SYMBOL MUST GO AT ALL THREE 0x96a SITES, not just the one that feeds the
 *    `add`: with the two plain-literal sites left alone the pool carries BOTH
 *    `_MSG_96a` and `0x96a`, one word too many, and objcmp reports `SIZE ref 452,
 *    ours 456` with three encodings off.  0x96b STAYS A LITERAL -- the ROM
 *    materialises it fresh in its own region, and writing it as `_MSG_96a + 1`
 *    would be a second pool word there.
 *
 * 2. THE SELL LOOP IS BUILT FROM A LABEL AND `goto`, NOT `for (;;)` + `continue`.
 *    A front-end loop emits NOTE_INSN_LOOP_BEG, loop.c then hoists the invariant
 *    `ldr r7, =_MSG_978` into the preheader -- ABOVE the label -- and the ROM
 *    keeps it as the first insn INSIDE the loop head.  A goto-built loop carries
 *    no loop notes, so loop.c leaves it alone.  MEASURED: `for (;;)`/`continue`
 *    costs +7 encodings.  Same lever as
 *    src/overlays/rom_78ef88/ovl_314_c_c_c_c_b.c ("THE LOOP IS UN-ROTATED and
 *    needs `goto`") and as the loop-notes barrier in the sibling
 *    src/rom_8a000/rom_91584_c_a_c_c_c_a_c_b.c.
 *
 * 3. `short *q` IS THE MISSING PSEUDO, AND IT IS WHAT SPILLS `saved`.  The ROM's
 *    frame is `sub sp, #0xc` -- three words -- but only two are accounted for by
 *    the _UI_SellMenu out-params at sp+8 and sp+4.  sp+0 is `saved` SPILLED
 *    (`str r2, [sp]` at entry, `ldrh r2, [sp]` at each of the three write-backs),
 *    which happens only when eight values want seven callee-saved registers.  The
 *    eighth is the address `p + (0xec << 1)`, held in r7 from entry to the
 *    bag-had-room exit and reused there for the 0x978 base inside the loop (two
 *    disjoint live ranges in one register -- the recorded lever).  Naming it
 *    `q` and storing through it ONLY on the path that does not cross the loop,
 *    while the other two write-backs rebuild the offset off `p` inline, is what
 *    produces the ROM: frame 0xc, `saved` at sp+0, and the three store sequences
 *    UN-CROSS-JUMPED.  MEASURED: 108 -> 56 disagreeing in one edit, and it is
 *    also what stops gcc merging the two identical `p`-relative stores into one.
 *
 * 4. TWO SEPARATE SHORT-LIVED LOCALS WHERE ONE WOULD DO (docs/elevation.md's
 *    "give each region its OWN block-scoped local").
 *      * `{ int s; s = slot; n = _GetInventoryItem(unit, s); }` -- naming the
 *        SECOND argument makes precompute_register_parameters load it first, so
 *        the ROM's `ldr r1, [sp, #4] / ldr r0, [sp, #8]` order comes out.
 *        Written `_GetInventoryItem(unit, slot)` the loads come out r0-first:
 *        +2 encodings.  expand emits register args r0-then-r1 and sched2 will not
 *        swap them (checked in cand.c.19.flow2 and cand.c.23.sched2: insn 205
 *        precedes 207 in both, both depending only on call 202, so the tie breaks
 *        on INSN_LUID and the order is FIXED AT EXPAND -- a source lever, not a
 *        scheduling one).
 *      * `int k; k = n;` as the do-while counter, separate from the `n` that the
 *        `> 0` guard tests.  That is what leaves the guard comparing the call's
 *        own r0 (`cmp r0, #0 / ble`) with `mov r5, r0` sunk into the loop
 *        preheader.  One shared variable instead: +3 encodings.
 *      * conversely `r` IS shared between the _UI_SellMenu result and the
 *        Func_8091d84 result -- both want the callee-saved r5 and their ranges
 *        are disjoint.
 *
 * 5. `_Func_8078ad0` MUST BE DECLARED int-RETURNING.  The ROM fills its r0 LAST
 *    (`mov r1, #1 / mov r0, r6`); declared `void` it comes out r0-first, +2
 *    encodings.  Same lever as "declare the callee int-returning so gcc fills r0
 *    last".  Leaving it undeclared altogether is equally exact, but the explicit
 *    `int` is the cleaner spelling and measures identically, as does declaring
 *    `_GetInventoryItem`.
 *
 * MEASURED NEGATIVES, so nobody re-runs them:
 *   - `int saved[1];` to force the spill: gcc-2.96 scalar-replaces a
 *     one-element array whose address never escapes; frame stays 0x8.  Worse than
 *     the `q` reading by 52 disagreeing instructions.
 *   - `(unsigned char *)&gState + (0xfa << 1)` written as one expression folds to
 *     `ldr r3, =gState+500`.  The ROM wants `ldr r3, =gState / mov r2, #0xfa /
 *     lsl r2, #1 / add r3, r2`, so the base goes through a named `unsigned char *`
 *     local first (the idiom already in rom_91584_c_a_c_c_c_a_c_b.c) and it is
 *     re-materialised in EACH of the two arms that need it.
 *   - `_GetUnit` declared void / int / `void *()` / `short *` / not at all: all
 *     inert, it does not move the _GetInventoryItem argument order.
 *   - `_GetInventoryItem` declared int / long / unsigned int / `()`: all inert.
 *   - `slot | 0`, `(unsigned char)slot`, and naming the FIRST argument instead of
 *     the second: none of them reverse the argument loads.
 *   - `_Func_8078ad0` declared void: +2.
 *   - The `beq .L91a8c / b .L91baa` pair at entry is NOT a source artefact: the
 *     else arm is 292 bytes away and a Thumb conditional branch reaches +-256, so
 *     the backend has to invert.  It appears on its own once the size is right.
 *
 * THE ONE THING STILL OWED.  message.sym has no 0x96a or 0x978 row, and neither
 * does any other .sym.  The evidence for both is the in-function two-sided kind
 * the file's own notes ask for: each base is HELD in a callee-saved register and
 * arithmetic is done ON it (`add r0, r7, #1/#2/#4`, `add r0, r5, #1`) while OTHER
 * ids in the same function stay plain literals (0x977 before the loop, 0x96b in
 * its own region) -- the symbol earns exactly the sites that show the `add`, and
 * no more.  PROPOSAL, for the owner:
 *     _MSG_96a = 0x096a;    -- "no room in your pack" / "X received the item"
 *     _MSG_978 = 0x0978;    -- base of the 0x978..0x97c sell-loop prompt run
 */
typedef struct { unsigned char _b[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _MSG_978;
extern int _MSG_96a;

struct Item { unsigned char pad0[3]; unsigned char f3; };

extern int _GiveItem(int item);
extern int _UI_SellMenu(int *a, int *b);
extern struct Item *_GetItemInfo(int item);
extern void *_GetUnit(int a);
extern void _Func_8078948(int a, int b);
extern void _PlaySound(int sfx);
extern void _Func_8019908(int a, int b);
extern void _Func_801776c(int a, int b);
extern int Func_8091d84(int a);
extern void _Func_8019a54(void);
extern int _GetInventoryItem(int a, int b);
extern int _Func_8078ad0(int a, int b);

int Func_8091a58(int item)
{
    unsigned char *p;
    unsigned char *g;
    short *q;
    int unit;
    int slot;
    int saved;
    int r;
    int base;
    int result;

    p = iwram_3001ebc;
    q = (short *)(p + (0xec << 1));
    saved = *q;
    result = _GiveItem(item);
    if (result == -1) {
        _Func_8019908(item, 2);
        _Func_801776c((int)&_MSG_96a, 1);
        _Func_801776c(0x977, 1);
loop:
        base = (int)&_MSG_978;
        _Func_801776c(base, 1);
        r = _UI_SellMenu(&unit, &slot);
        if (r == -1) {
            if (_GetItemInfo(item)->f3 & 8) {
                _Func_8019908(item, 2);
                _Func_801776c(base + 4, 1);
                goto loop;
            }
            _Func_8019908(item, 2);
            _Func_801776c(base + 1, 5);
            r = Func_8091d84(1);
            _Func_8019a54();
            if (r != 0)
                goto loop;
            _Func_8078ad0(item, 1);
            _Func_8019908(item, 2);
            _Func_801776c(base + 2, 1);
            *(short *)(p + (0xec << 1)) = saved;
            return result;
        } else {
            int n;
            _GetUnit(unit);
            { int s; s = slot; n = _GetInventoryItem(unit, s); }
            if (n > 0) {
                int k;
                k = n;
                do {
                    _Func_8078948(unit, slot);
                } while (--k);
            }
            result = _GiveItem(item);
            _PlaySound(0x53);
            g = (unsigned char *)&gState;
            if (result == *(int *)(g + (0xfa << 1))) {
                _Func_8019908(item, 2);
                _Func_801776c((int)&_MSG_96a, 3);
            } else {
                _Func_8019908(item, 2);
                _Func_8019908(result, 1);
                _Func_801776c(0x96b, 3);
            }
            *(short *)(p + (0xec << 1)) = saved;
            return result;
        }
    } else {
        _PlaySound(0x53);
        _Func_8019908(item, 2);
        r = (int)&_MSG_96a;
        _Func_801776c(r, 3);
        g = (unsigned char *)&gState;
        if (result != *(int *)(g + (0xfa << 1))) {
            _Func_8019908(item, 2);
            _Func_8019908(result, 1);
            _Func_801776c(r + 1, 3);
        }
        *q = saved;
        return result;
    }
}
