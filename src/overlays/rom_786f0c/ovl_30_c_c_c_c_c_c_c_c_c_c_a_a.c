/* OvlFunc_886_2008368 -- 0x02008368 -- MATCHES, whole file, batch 335 brief H.
 *
 * Converts WHOLE: the reference holds this one function, datacheck is silent,
 * there is no global and shimcount charges nothing.
 *
 * WHAT CLOSED IT, and it is one edit to the park body.
 *
 * The halfword store of one through the second global is carried by a
 * FUNCTION-SCOPE `int one = 1;`.  Stored as a bare literal through a halfword
 * lvalue the constant is HImode, `*thumb_movhi_insn` matches its `mn`
 * alternative before its immediate one, and an HImode pool entry has a reach
 * of sixty-four bytes against a word's thousand-and-twenty.  That one entry
 * therefore forces an EARLY pool dump in the middle of the function, a branch
 * over the dump, and a SECOND copy of the gState pool word for the reads that
 * follow the dump.  Two pool words and one instruction -- eight bytes -- and
 * all three are ONE defect, not three.
 *
 * AND THE PLACEMENT OF THE CARRIER MATTERS AS MUCH AS ITS TYPE.  The same
 * `int` written in the INNERMOST block kills the pool entry just as well, but
 * it is a live pseudo inside that block and it pushes the FIRST store's offset
 * pseudo off the register it wants.  That breaks reload_cse_move2add, which
 * rewrites a set of a hard register to a constant as an add when that same
 * hard register already holds a known constant -- here the offset value, with
 * the stored word derived from it by adding a small immediate.  Lose the
 * shared register and the stored word pools instead, and the function is four
 * bytes long again.  At function scope the carrier costs no register anywhere:
 * cse sinks it to its single use as a SImode constant, where movsi's
 * alternatives are ordered the other way round and a small constant can never
 * pool.
 *
 * RULE: a halfword-store constant needs an `int` carrier; put it in the
 * innermost block when the site wants its own short-lived register, and at
 * FUNCTION SCOPE when an inner carrier would disturb an allocation elsewhere.
 * This corrects the unqualified "innermost block, never function scope" rule
 * recorded in the Func_809c138 park.
 *
 * The park's stated next step -- fix the block structure of the first
 * conditional so a stray branch to the following label goes away -- was wrong.
 * That branch was the pool branch, a consequence of the HImode pool entry.
 *
 * WHAT THE PARK HAD ALREADY GOT RIGHT, carried forward unchanged: the return
 * type is int returning zero; the second global is derived off the first by a
 * word subscript so one pool entry serves both; the stored word at the first
 * site is derived from the offset register; the actor pointer is NOT named so
 * the byte offset may clobber the argument register; the fifteen position
 * calls are separate statements, not a loop; and gState is declared with a
 * HALFWORD element type, because a byte-pointer-plus-offset spelling lets gcc
 * fold the whole address into a single pool word where the ROM materialises
 * the bare symbol and adds the offset at run time.
 */
struct Actor {
    unsigned char pad00[0x59];
    unsigned char f59;
};

extern char *iwram_3001ebc;
extern short gState[];

extern int __GetFlag(int id);
extern void __StartThunder(void);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Func_8095240(void);
extern void __Func_8095268(void);
extern void __Func_80118a8(int a);
extern void OvlFunc_886_2008658(void);

int OvlFunc_886_2008368(void)
{
    int one = 1;
    *(int *)(iwram_3001ebc + 0x1c0) = 0x209;
    if (__GetFlag(0x834)) {
    __MapActor_SetPos(0x8, 0, 0);
    __MapActor_SetPos(0x9, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_SetPos(0xb, 0, 0);
    __MapActor_SetPos(0xc, 0, 0);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xe, 0, 0);
    __MapActor_SetPos(0xf, 0, 0);
    __MapActor_SetPos(0x10, 0, 0);
    __MapActor_SetPos(0x11, 0, 0);
    __MapActor_SetPos(0x12, 0, 0);
    __MapActor_SetPos(0x13, 0, 0);
    __MapActor_SetPos(0x14, 0, 0);
    __MapActor_SetPos(0x15, 0, 0);
    __MapActor_SetPos(0x16, 0, 0);
        __StartThunder();
        *(short *)((&iwram_3001ebc)[3] + 0x1f84) = one;
        __Func_8095240();
        __WaitFrames(0x1e);
        __MapTransitionIn();
        __WaitMapTransition();
        __Func_8095268();
    }
    if (__GetFlag(0x87a)) {
        if (gState[0xe1] == 6 && !__GetFlag(0x81d))
            OvlFunc_886_2008658();
        __MapActor_GetActor(0xa)->f59 |= 0x80;
    }
    if (gState[0xe1] == 2 && __GetFlag(0x815)) {
        __MapActor_SetPos(0xd, 0xe3 << 17, 0x96 << 16);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xd), 0);
        __MapActor_SetAnim(0xd, 5);
        __Func_80118a8(4);
    }
    return 0;
}
