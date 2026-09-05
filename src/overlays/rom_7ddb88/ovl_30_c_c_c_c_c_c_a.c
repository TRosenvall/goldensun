/* OvlFunc_955_2009538  --  0x02009538
 *
 * 163 instructions of straight-line cutscene script behind a single `if`, in
 * the same family as the template neighbour OvlFunc_954_2009214
 * (src/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_b.c): identical skeleton -- gState
 * halfword guard, __CutsceneStart, OvlFunc_common1_4cc, a three-way branch on
 * its result, OvlFunc_common1_5e4, __CutsceneEnd.
 *
 * EXACT. objcmp against both the scratch reference and the original
 * asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_a.s:
 *   OK OvlFunc_955_2009538 -- 412 bytes, 166 encodings and 38 relocations identical
 *
 * LEVER 1: THE CONSTANT-CSE PIN, x13. The prologue is `push {r5, r6, r7, lr} /
 * mov r7, r8 / push {r7}` -- a WIDE push, so the function keeps four values
 * live across calls and this is a pin function, not a rematerialise-at-the-use
 * function. Thirteen shifted constants are built in the dominating block, one
 * statement per value, whole value per statement, uniform ascending fill. That
 * is the default shape and it was not adjusted afterwards: transcribing the
 * ROM's emitted order bought nothing once the pins existed.
 *
 * All thirteen were re-measured INDIVIDUALLY in the final shape of this file
 * (drop the local, inline the literal at every use, nothing else changed).
 * Every one is load-bearing; the fixpoint is immediate because no single drop
 * is free, so no greedy descent reaches a smaller set:
 *
 *   drop  differing  first divergence
 *   ----  ---------  ----------------------------------------
 *   z       162      +4 bytes, +2 encodings; prologue at index 1
 *   m       162      +4 bytes, +2 encodings; prologue at index 1
 *   s       162      +8 bytes, +4 encodings; prologue at index 1
 *   w1        3      index 29:  ref 20bc  ours 22c0
 *   w2        3      index 45:  ref 20bc  ours 22e0
 *   w3        3      index 96:  ref 20bc  ours 22d8
 *   t1        4      index 40:  ref 2080  ours 2180
 *   t2        5      index 109: ref 2180  ours 2280
 *   q1        2      index 69:  ref 2000  ours 0249
 *   q2        5      index 67:  ref 21c0  ours 22c0
 *   u         2      index 124: ref 2000  ours 01c9
 *   x         2      index 137: ref 2021  ours 0489
 *   y         2      index 135: ref 21d2  ours 22e8
 *   g       155      -4 bytes, -2 encodings; index 3: ref 4b62 ours 4b61
 *
 * Note the split in MECHANISM, readable in the size column. Dropping z, m or s
 * GROWS the function: each is used at two or three sites, local CSE hoists it
 * itself into a high register, and the prologue gains a push -- the same
 * polarity the neighbour records, where a pin exists to LOWER the reference
 * count so local-alloc rematerialises instead of keeping the value live. The
 * ten single-use pins (w1..y) leave the length EXACTLY unchanged and move only
 * the argument fill order; that is the constant-CSE length tell cancelling, so
 * the diff TEXT is the only evidence and a length check alone would have
 * called them inert.
 *
 * LEVER 2: `g` IS A FOURTEENTH LOCAL, NOT A CONVENIENCE. `unsigned char *g =
 * gState;` is what makes the guard build its offset rather than fold it: the
 * ROM has `ldr r3, =gState / mov r2, #0xe1 / lsl r2, #1 / add r3, r2 / mov r2,
 * #0 / ldrsh r3, [r3, r2]`. Subscripting gState directly folds the offset into
 * the pool load and the register-offset ldrsh collapses to an immediate one --
 * four bytes SHORTER and 155 differing. This is the gState-offset-must-be-BUILT
 * rule; it costs a named pointer.
 *
 * LEVER 3: THE RETURN TYPE, NOT A MISSING PROTOTYPE. Three callees want `mov
 * r0` emitted LAST in their argument fill. The prior candidate here, the
 * template neighbour's header, and the "r0 in the middle vs r0 at the end"
 * entry in docs/elevation.md all spell that as DROPPING the declaration. That
 * is the batch-92..94 mistake the batch-99 correction already withdrew: the
 * parameter list is irrelevant and the RETURN TYPE is the lever. Measured here:
 *
 *   spelling                                          differing
 *   ------------------------------------------------  ---------
 *   no declaration for _15b8 and _5e4 (implicit int)      0
 *   extern int  _15b8(...) / extern int _5e4(...)         0   <-- shipped
 *   extern void _15b8(int,int,int) added alone            5
 *   extern void _5e4(int,int,int) added alone             3
 *   both void prototypes added                            8   (additive)
 *
 * So this function lands with EVERY callee declared. Nothing is withheld.
 *
 * NEW -- THE WITHHELD SET IS PER-SITE AND DOES NOT TRANSFER BETWEEN SIBLINGS.
 * The neighbour OvlFunc_954_2009214 states in its own header that the three
 * callees wanting r0 last are OvlFunc_common1_1078, _15b8 and _5e4, and
 * elevation.md's `OvlFunc_954_20095e0` entry names the same three. In this
 * sibling -- same overlay family, same skeleton, same fourteen-local shape --
 * only _15b8 and _5e4 respond. `OvlFunc_common1_1078` is INDIFFERENT: `extern
 * void`, `extern int`, and no declaration at all are byte-identical, all three
 * exact. Its site simply has no rotation to expose (`mov r1 / mov r2 / mov r0`
 * is already what uniform fill produces there). A family's withheld set is a
 * per-call-site reading, not an inherited constant, and the template's list
 * must be re-derived rather than copied.
 *
 * NEW -- A DISCARDED RESULT STILL SELECTS THE ORDER, SO THE RETURN TYPE IS NOT
 * INFERABLE FROM USE. `OvlFunc_955_2008310`'s result is never read, yet it must
 * be declared `int`: `extern void` costs 2 differing, first at index 93, where
 * ref has `mov r2, #0` (2200) and we emit `mov r0, #0x21` (2021) -- the r0-last
 * rotation again. elevation.md records this for `__CloseUIBox`'s discarded site
 * via a void-returning alias; the corollary worth stating is the reverse
 * direction -- you cannot type a callee `void` just because nobody uses what it
 * returns, and doing so is a silent two-encoding regression.
 *
 * MEASURED WORSE, ALL SPELLINGS TRIED
 *
 *   spelling                                          differing
 *   ------------------------------------------------  ---------
 *   any one of the 14 locals dropped                   2..162 (table above)
 *   extern void OvlFunc_common1_15b8(...)                  5
 *   extern void OvlFunc_common1_5e4(...)                   3
 *   both of the above                                      8
 *   extern void OvlFunc_955_2008310(...)                   2
 *   gState subscripted directly (no `g`)                 155
 *
 * MEASURED NEUTRAL (cosmetic, either spelling is exact)
 *
 *   the 13 constants on one declaration line vs two
 *   OvlFunc_common1_1078 declared void / int / not at all
 *
 * LANDING: NO SPLIT NEEDED. asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_a.s
 * holds exactly one function and no data -- `.thumb_func_start` at line 8,
 * `.func_end` at line 177, nothing else but the two `.include`s and a comment
 * block. The linker script already names the object it produces and does not
 * change: overlays/rom_7ddb88/overlay.ld line 53 for `.text`, line 95 for
 * `.data`, line 104 for `.data1`, all reading
 * `asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_a.o`. No Makefile rule mentions
 * rom_7ddb88, so the generic cross-dir rule `asm/%.o: src/%.c` (Makefile line
 * 146) picks the .c up with no new rule. objcmp agrees byte for byte against
 * the scratch reference and against the original asm path, so no pattern rule
 * is biting.
 */
extern unsigned char gState[];

extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __SetCameraTarget(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern int OvlFunc_955_2008310(int a, int b, int c);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_2c4(void);
extern void OvlFunc_common1_1254(int a);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_common1_1078(int a, int b, int c);
extern int OvlFunc_common1_15b8(int a, int b, int c);
extern int OvlFunc_common1_5e4(int a, int b, int c);

void OvlFunc_955_2009538(int a)
{
    unsigned char *g;
    int r;
    int z, m, s, w1, w2, w3, t1, t2, q1, q2, u, x, y;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 3);
    z = 0xbc << 18;
    m = -1;
    s = 0x80 << 9;
    w1 = 0xc0 << 16;
    w2 = 0xe0 << 16;
    w3 = 0xd8 << 16;
    t1 = 0x80 << 6;
    t2 = 0x80 << 8;
    q1 = 0xc0 << 9;
    q2 = 0xc0 << 8;
    u = 0x80 << 7;
    x = 0xd2 << 18;
    y = 0xe8 << 16;
    if (r == 0) {
        __MessageID(0x20a6);
        __Func_80933d4(0xc0 << 10, 0xc0 << 7);
        __Func_80933f8(z, m, w1, 1);
        __Func_8093530();
        __CutsceneWait(0x3c);
        __Func_80933d4(s, t1);
        __Func_80933f8(z, m, w2, 1);
        __Func_8093530();
        __ActorMessage(a, 0);
        OvlFunc_common1_1078(0, 0xd6 << 2, 0x84 << 1);
        __CutsceneWait(0xa);
        __MapActor_SetSpeed(0, q1, q2);
        OvlFunc_common1_15b8(0, 0xd6 << 2, 0x84 << 1);
        OvlFunc_common1_15b8(0, 0xd6 << 2, 0xe8);
        __ActorMessage(a, 0);
        OvlFunc_common1_15b8(0, 0xd2 << 2, 0xe8);
        __CutsceneWait(0xa);
        OvlFunc_955_2008310(0x21, -0x40, 0);
        __Func_80933f8(z, m, w3, 1);
        __MapActor_SetAnim(0, 1);
        __CutsceneWait(0xa);
        __MapActor_SetSpeed(0, s, t2);
        __Func_80921c4(0, 0xbe << 2, 0xe8);
        __CutsceneWait(0xa);
        __Func_8092adc(0, u, 0x1e);
        __ActorMessage(a, 0);
        OvlFunc_common1_1254(0);
        __SetCameraTarget(0, 0);
        __MapActor_SetPos(0x21, x, y);
        OvlFunc_common1_588(a, 3);
    } else if (r == 1) {
        __MessageID(0x20a5);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, 3);
    __CutsceneEnd();
}
