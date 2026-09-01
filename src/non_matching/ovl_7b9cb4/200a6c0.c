/* OvlFunc_932_200a6c0 (0x0200a6c0) -- NON-MATCHING.
 * Blocker class: register allocation, in two shapes, at the ROM's exact length.
 *
 * 136 lines against 136, 28 differing, and the 28 are two patterns repeated:
 *
 * (a) THE `orr` REGISTER ROLES, three sites, six lines. The ROM loads the byte
 *     into r2 and materialises the constant into r3, then ORs into r3:
 *
 *         rom    ldrb r2, [r0] / mov r3, #0x2 / orr r3, r2 / strb r3, [r0]
 *         ours   ldrb r3, [r0] / mov r2, #0x2 / orr r3, r2 / strb r3, [r0]
 *
 *     Every spelling puts the OR's destination on whichever operand is written
 *     first, and gcc's choice of which register receives the LOAD is fixed:
 *
 *       `*a |= 2;`                        ldrb r3 / mov r2  -- 2 wrong
 *       `*a = 2 | *a;`                    identical (canonicalised)
 *       `b = *a; *a = b | 2;`             ldrb r2 / mov r3 right, orr and strb
 *                                         now wrong -- still 2
 *       `b = *a; *a = 2 | b;`             identical to the above
 *       `v = 2; v |= *a; *a = v;`         4 wrong (worse)
 *
 *     Two of the four instructions are right in every spelling and it is never
 *     the same two. That is the signature of an allocation decision rather than
 *     an expression-shape one.
 *
 * (b) THE TWO STACK ARGUMENTS, six sites, twenty-two lines. `__Func_8010704`
 *     and `__CopyMapTiles` take six arguments, the last two on the stack. When
 *     those two DIFFER the ROM materialises both before storing either:
 *
 *         rom    mov r3, #0x11 / mov r2, #0xa / str r3, [sp] / str r2, [sp, #4]
 *         ours   mov r3, #0x11 / str r3, [sp] / mov r3, #0xa / str r3, [sp, #4]
 *
 *     Same instruction count; gcc reuses one register and interleaves the
 *     stores. NOTE the site where both stack arguments are the SAME value
 *     (0x1a, 0x1a) MATCHES -- the ROM uses one register there too, and so do we.
 *     So the residue appears only when two distinct values compete.
 *
 * MEASURED (rom 136 lines, all at exact length):
 *   baseline                                            136, 28  <- best
 *   the four `orr` spellings above                      136, 28 / 28 / 28 / 34
 *   the eight stack constants assigned in the entry
 *     block (the batch-176 basic-block lever)           144, 142 (gcc spills
 *                                     all eight; the lever works for ONE
 *                                     constant at ONE call, not for eight)
 *   one pair only assigned in the entry block           136, 28
 *   both six-argument prototypes WITHHELD               136, 28
 *
 * THE LEVER-OVERLOAD RESULT IS WORTH KEEPING. Batch 176 broke the
 * arg-interleave wall by assigning a constant in a dominating basic block, and
 * that worked with four constants in a function with no other pressure. Eight
 * constants here cost eight extra lines and 142 differing: the locals have to
 * live somewhere, and a `push {lr}` function has nowhere to put them. **The
 * basic-block lever is bounded by the register file, and its cost is a spill.**
 *
 * WHAT IS RIGHT: all 24 calls, both six-argument call shapes including the
 * stack setup at the site where the two values coincide, the three
 * `__GetFlag`-guarded blocks, `0x80 << 2` / `0x81 << 2` / `0xc8 << 4`, the
 * nested `__Actor_SetSpriteFlags(__MapActor_GetActor(9), 0)`, and the actor
 * pointer ADVANCED IN PLACE (`a = __MapActor_GetActor(9) + 0x23;`) which is
 * what gives the ROM's `add r0, #0x23`.
 *
 * NEXT: nothing source-level in seven probes.
 */
extern char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(char *a, int f);
extern void OvlFunc_932_200840c(void);
extern void OvlFunc_932_200b460(int slot);
extern int __GetFlag(int);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern int __StartTask(void *fn, int pri);
extern void OvlFunc_932_200b428(void);

void OvlFunc_932_200a6c0(void)
{
    char *a;

    __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
    OvlFunc_932_200840c();
    OvlFunc_932_200b460(9);
    if (__GetFlag(0x80 << 2) != 0) {
        __MapActor_SetAnim(9, 5);
        __Func_8010704(0, 0, 1, 1, 0x1a, 0x1a);
        a = __MapActor_GetActor(9) + 0x23;
        *a |= 2;
    }
    OvlFunc_932_200b460(0xb);
    if (__GetFlag(0x201) != 0) {
        __MapActor_SetAnim(0xb, 5);
        __Func_8010704(1, 0, 1, 1, 0x11, 0xa);
        a = __MapActor_GetActor(0xb) + 0x23;
        *a |= 2;
    }
    OvlFunc_932_200b460(0xc);
    if (__GetFlag(0x81 << 2) != 0) {
        __MapActor_SetAnim(0xc, 5);
        __Func_8010704(1, 0, 1, 1, 0x1a, 0xf);
        a = __MapActor_GetActor(0xc) + 0x23;
        *a |= 2;
    }
    __StartTask(OvlFunc_932_200b428, 0xc8 << 4);
    if (__GetFlag(0x327) != 0) {
        __Func_8010704(0x1e, 0x52, 1, 1, 0x1d, 0x51);
        __CopyMapTiles(0x2e, 0x1c, 0x1d, 0x11, 1, 2);
    } else {
        __Func_8010704(0x1c, 0x52, 1, 1, 0x1d, 0x51);
        __CopyMapTiles(0x2f, 0x1c, 0x1d, 0x11, 1, 2);
    }
}
