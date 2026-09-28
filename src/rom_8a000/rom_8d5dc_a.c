/* Func_808d5dc (InteractWithTarget) -- 0x0808d5dc.  EXACT.
 * ref: asm/rom_8a000/rom_8d5dc_a.s  (ONE function, no data section --
 *      grep -ci func_start = 1; converts the WHOLE FILE, no split needed)
 * objcmp: OK whole file -- 508 bytes, 222 encodings and 29 relocations identical.
 *
 * batch 293, brief A, target 3.  NO SHIMS: no register pins, no "+r" barriers,
 * no volatile, no do{}while(0), no .equ, no DMA macros, default flags.
 * (register ... __asm__ declarations in the code below: ZERO.)
 *
 * Structural sibling of Func_808d7d8 in src/rom_8a000/rom_8d5dc_b.c, which has
 * the same "record at ev[2]: below 0x10000 is a message id, at or above it is a
 * function pointer" dispatch.
 *
 * THE LOAD-BEARING CONSTRUCTS.  Every bracketed figure is one single drop off
 * the exact candidate, measured with objcmp --whole; the candidate is 222
 * encodings / 508 bytes, and each drop's own count is given when it moved.
 *
 * 1. THE TWO gState OFFSETS ARRIVE THROUGH LOCALS, `o` AND `o2`.  The ROM
 *    reaches gState+0x24a as `ldr r3,=gState / ldr r1,=0x24a / add r3,r1` and
 *    gState+0x1f4 as `ldr r5,=gState / mov r2,#0xfa / lsl r2,#1 / add r5,r2`.
 *    Written `gState + 0x24a` the sum is a tree constant, the address becomes a
 *    single CONST, and gcc pools `gState+586` as ONE relocated word -- which
 *    also rotates the pool and moves every later relocation.  A local forces the
 *    two-part address at every site, and gcc rematerialises the constant per
 *    site exactly as the ROM does (two `ldr r1,=0x24a`, two `mov #0xfa/lsl #1`),
 *    so one local per offset covers all its uses.
 *      drop `o`  -> [200 of 222, ours 218, relocations differ]
 *      drop `o2` -> [150 of 222, ours 215, relocations differ]
 *    This is the whole reason the first draft sat at 214 differing.
 *
 * 2. `signed char f = pl[0x16]; if ((unsigned char)f <= 1 || f == 3)`.
 *    The ROM tests the facing byte as
 *        ldrb r3,[r1,#0x16] / mov r2,#0x80 / lsl r3,#24 / lsl r2,#17
 *        cmp r3,r2 / bls / mov r1,#0xc0 / lsl r1,#18 / cmp r3,r1 / bne
 *    i.e. it compares (f << 24) against 1<<24 and 3<<24 instead of comparing the
 *    byte.  combine's simplify_comparison undoes an explicit shift whenever the
 *    high bits of the shifted value are known zero, so writing the shifts by
 *    hand is inert -- the shape only survives when the value is NOT known narrow.
 *    A `signed char` holding an `ldrb` result is exactly that: the sign
 *    extension is not a no-op for 0x80..0xff, the `(unsigned char)` cast back
 *    zero-extends, and the `lsl #24` has nowhere to fold.  Eight spellings were
 *    probed (see the ladder note below); `unsigned char f`, `int f`, `int f &
 *    0xff`, an inline `(unsigned char)` cast and a `switch` all give the plain
 *    `cmp r3,#1 / bls` form.
 *      drop to `if (f <= 1 || f == 3)` -> [135 of 222, ours 218]
 *    The SECOND read of the same byte, at the end, is `*(signed char *)` and
 *    compares directly (`ldrsb` + `cmp r0,#3`): two reads of one byte, two
 *    spellings, and the ROM says which is which.
 *
 * 3. AN int CARRIER FOR THE 0xffff STORE, BECAUSE THE POOL IS ORDERED BY MODE.
 *    `*(unsigned short *)(gState + o) = 0xffff` gives a HImode pool entry, which
 *    gcc emits with `ldrh r3, .Lpool` -- the same encoding as `ldr` (thumb-1 has
 *    no PC-relative ldrh, batch 292 settled that) but placed FIRST in the pool,
 *    ahead of every SImode word, which rotates the pool and shifts all three
 *    ABS32 relocations by 4.  Assigning through an `int` makes it an SImode
 *    constant, which pools in first-use order, i.e. last.
 *      drop the carrier -> [15 of 222, relocations differ]
 *    This is a pool-ORDER defect that tryc cannot see at all: --align reports it
 *    as zero differing instructions.
 *
 * 4. `unsigned char bit = 1; e[0x5a] = bit | e[0x5a];` FOR THE BIT SET.
 *    thumb's iorsi3 ties operand 0 to operand 1, so whichever side of the IOR is
 *    first becomes the destination.  `e[0x5a] |= 1` puts the loaded byte first
 *    (`ldrb r3 / mov r2,#1 / orr r3,r2`); the ROM has the constant first
 *    (`ldrb r2 / mov r3,#1 / orr r3,r2`).  A QImode local holding the 1 is what
 *    orders them that way -- an `int` local set to 1 is NOT enough (gcc folds it
 *    back to a constant operand and re-canonicalises), and neither is writing
 *    `1 | e[0x5a]`, a bitfield store, or a named pointer.  All six alternatives
 *    measured identical to the plain `|=`.
 *      drop to `e[0x5a] |= 1;` -> [2 of 222]
 *
 * 5. `ret = 0;` SITS AFTER THE `if (repeat == 0)` BLOCK, NOT INSIDE IT.  The
 *    ROM's `.L8d790` (`mov r2,#0 / mov r9,r2`) is the target of the
 *    `cmp r2,#0 / bne` that skips the block, so the success return is shared.
 *    Inside the block, gcc threads the branch straight to the epilogue test and
 *    the `bne` displacement is wrong by five instructions.
 *      drop (move it inside) -> [1 of 222]
 *
 * 6. DECLARATION ORDER `ang` BEFORE `flag`.  Both are spilled -- `sub sp, #8` --
 *    and the ROM puts the interaction flag at sp+0 and the saved facing angle at
 *    sp+4.  Swapping the two declarations swaps the slots.
 *      drop (swap them) -> [5 of 222]
 *    `ang` is an `int` assigned from a halfword and later stored as a halfword;
 *    the ROM's `add r1, sp, #4 / ldrh r1, [r1]` is combine narrowing the load of
 *    the spill slot, and the address computation is there only because thumb has
 *    no sp-relative `ldrh`.
 *
 * MEASURED AND INERT (all still OK whole file):
 *  - `extern void Func_8092848(...)` instead of `int`.  It mattered at an earlier
 *    rung -- the ROM fills r2, r1, r0 in that order, which the r0-last rule reads
 *    as non-void -- and stopped mattering once construct 7 below was in place.
 *    Recorded because the two are COUPLED, not because either is free.
 *  - literal `0` instead of `repeat` for the three velocity stores at +0x24/28/2c.
 *    The ROM's `mov r3, r10` is cse's own substitution: record_jump_equiv learns
 *    r10 == 0 from the `cmp/bne` above, so the literal reaches the ROM's copy
 *    without being written as the variable.
 *  - `((void (*)(int))ev[2])(id)` instead of the `fp` local.  THIS CONTRADICTS
 *    docs/elevation.md ("Indirect calls: _call_via_r3 is a solved shape"), which
 *    states "The local matters.  Calling through the expression directly is a
 *    different shape".  On this function the two are byte-identical, including
 *    the `ldr r3,[r6,#8]` reload after the intervening `bl Func_8091660`.  The
 *    doc's claim is not general; it may still hold where the pointer is live
 *    across something.
 *
 * LADDER (objcmp --whole, then tryc --align in brackets):
 *   v1 plain draft, folded offsets       214 differ of 232 lines
 *   v2 + named `o`                       158 differ        [66 of 232]
 *   v3 + named `o2`, decl order, int
 *      carrier candidates, `repeat`      154 differ        [27 of 232]
 *   v4 + signed-char facing test          18 of 222        [27 of 232]
 *   v5 + int carrier for 0xffff            3 of 222
 *   v6 + `bit` and the hoisted `ret = 0`   EXACT
 */
extern unsigned char gState[];
extern unsigned char *Func_808d394(int id);
extern unsigned char *MapActor_GetActor(int id);
extern int *FindMapActorEvent(int kind, int id);
extern int MapActor_GetName(int id);
extern unsigned int Random(void);
extern void CutsceneStart(void);
extern void MessageID(int id);
extern void ActorMessage(int a, int b);
extern void CutsceneEnd(void);
extern void _Actor_SetAnimSpeed(unsigned char *a, int s);
extern int Func_8092848(int a, int b, int c);
extern void _Func_8017620(int a);
extern void Func_8091660(void);
extern void Actor_SetBehavior(unsigned char *a, void *b);
extern void _Actor_SetScript(unsigned char *a, void *s);
extern void Func_809ade8(int id);
extern unsigned int Data_9ff40[];
extern unsigned char Data_9fc1c[];

int Func_808d5dc(int id)
{
    unsigned char *pl;
    unsigned char *e;
    unsigned char *p;
    int *ev;
    int ret;
    int repeat;
    int ang;
    int flag;
    signed char f;
    int g;
    int o;
    int o2;
    int w;
    unsigned char bit;
    int m;
    void (*fp)(int);

    o = 0x24a;
    o2 = 0xfa << 1;
    pl = Func_808d394(id);
    ret = -1;
    e = MapActor_GetActor(id);
    flag = 0;
    repeat = 0;
    if (*(short *)(gState + o) == id) {
        repeat = 1;
        ev = FindMapActorEvent(7, id);
        if (ev == 0) {
            ev = FindMapActorEvent(0, id);
            flag = 1;
            if (ev == 0)
                return ret;
            if (ev[2] >= 0x10000) {
                m = MapActor_GetName(id) * 2 + ((Random() * 2) >> 16) + 0xe0b;
                CutsceneStart();
                MessageID(m);
                ActorMessage(id, 0);
                CutsceneEnd();
                goto done;
            }
        }
    } else {
        ev = FindMapActorEvent(0, id);
    }
    if (ev == 0)
        goto done;
    if (ev[2] == 0)
        goto done;
    if (repeat == 0) {
        *(unsigned char *)(e + 0x5b) = 1;
        _Actor_SetAnimSpeed(e, 0);
        ang = *(unsigned short *)(e + 6);
        f = pl[0x16];
        if ((unsigned char)f <= 1 || f == 3) {
            p = MapActor_GetActor(*(int *)(gState + o2));
            *(int *)(p + 0x38) = *(int *)(p + 8);
            *(int *)(p + 0x3c) = *(int *)(p + 0xc);
            *(int *)(p + 0x40) = *(int *)(p + 0x10);
            *(int *)(p + 0x24) = repeat;
            *(int *)(p + 0x28) = repeat;
            *(int *)(p + 0x2c) = repeat;
            Func_8092848(id, *(int *)(gState + o2), 0);
        }
    }
    if (ev[2] < 0x10000) {
        _Func_8017620(flag);
        CutsceneStart();
        MessageID(ev[2]);
        ActorMessage(id, 0);
        CutsceneEnd();
    } else {
        Func_8091660();
        fp = (void (*)(int))ev[2];
        fp(id);
    }
    if (repeat == 0) {
        if (((int *)*(int *)e)[*(short *)(e + 4)] == 0x10) {
            g = *(signed char *)(pl + 0x16);
            if (g == 3) {
                *(int *)(e + 0x68) = (int)MapActor_GetActor(*(int *)(gState + o2));
                bit = 1;
                e[0x5a] = bit | e[0x5a];
                Actor_SetBehavior(e, Data_9ff40);
            } else if (g == 1) {
                *(short *)(e + 0x64) = ang;
                _Actor_SetScript(e, Data_9fc1c);
            }
        }
        *(unsigned char *)(e + 0x5b) = 0;
        _Actor_SetAnimSpeed(e, 0x10);
    }
    ret = 0;
done:
    if (repeat != 0) {
        Func_809ade8(*(short *)(gState + o));
        w = 0xffff;
        *(unsigned short *)(gState + o) = w;
    }
    return ret;
}
