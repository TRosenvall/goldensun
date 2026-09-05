/* OvlFunc_971_2008d68 -- 0x02008d68
 *
 * A dialogue gate: read the leader's facing, start the cutscene, then pick one
 * of two message pairs depending on which way the leader is facing, and within
 * each pair either play the follow-up line or fall back to a refusal.
 *
 * THE SHARED BLOCK MUST BE SHARED IN SOURCE -- this is the COUNTER-case to the
 * "let gcc cross-jump a shared tail" entry. Writing both arms out in full lets
 * gcc constant-propagate the message id and it never produces the ROM's
 * `mov r0, r8 / add r0, #1` phi at all (62 differing). That entry is about a
 * shared TAIL; a shared MIDDLE carrying a DIFFERING CONSTANT cannot be produced
 * by jump.c, because the two copies are not identical. Write it once.
 *
 * AND ONE `goto` PLACES THE LAST ARM'S EARLY RETURN AFTER THE SHARED BLOCK.
 * With both early returns inline, gcc cross-jumps the two EARLY RETURNS
 * together and leaves the shared block its own tail copy (54 differing); the
 * ROM does the opposite -- the first arm's early return jumps INTO the shared
 * block's tail and the second sits last. Which pair jump.c merges is decided
 * entirely by SOURCE BLOCK ORDER, and the ROM's order is only expressible with
 * the goto. Note the goto is on the UNSHARED arm, not on the share, which is
 * what keeps it consistent with the entry above.
 *
 * STATEMENT ORDER GOT THE HIGH REGISTER FOR FREE. Assigning the message base
 * BEFORE the pointer overlaps their live ranges and gcc reaches for r8 by
 * itself, prologue and all. With the pointer first, the base falls into the
 * register the other value has just vacated and the r8 save disappears (63
 * differing). No pin was needed -- and a pin would likely have been dropped
 * anyway, since it spans two calls.
 *
 * `pop {r1} / bx r1` RATHER THAN `pop {r0} / bx r0` MEANS THE RETURN VALUE IS
 * LIVE. The last two differing lines were purely that: gcc picks r0 for the
 * epilogue scratch when r0 is dead (a void function) and r1 when r0 carries a
 * return value. Making the function `int` and returning the last call's result
 * on all three paths closed it. That is a cheap, unambiguous diagnostic for a
 * wrong return type, worth the same standing as the frame-size one.
 *
 * Two more on file: the folded range bounds were computed arithmetically rather
 * than read off the digits (the span constant confirms them), a NAMED POINTER
 * blocks the reassociation that would fold the base and index into one
 * register-offset load, and the callee's RETURN TYPE decides r0 fill order for
 * this direct call.
 *
 * Verified with tools/objcmp.py: 168 bytes, 67 encodings and 10 relocations
 * identical, pool order included.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern int __Func_809280c(int a, int b, int c);
extern void __Func_80198dc(void);
extern void __Func_8019908(int a, int b);
extern void __MessageID(int id);
extern int __ActorMessage(int actor, int b);

int OvlFunc_971_2008d68(unsigned int actor)
{
    unsigned int v;
    unsigned int gs;
    unsigned int off;
    unsigned int msgBase;
    unsigned int *q;
    unsigned short *p;

    v = *(unsigned short *)(__MapActor_GetActor(0) + 6);
    __CutsceneStart();
    gs = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    q = (unsigned int *)(gs + off);
    __Func_809280c(actor, *q, 0);
    if (v > 0xa000 && v < 0xe000) {
        msgBase = 0x297b;
        p = (unsigned short *)(gs + 0x2ac);
        if (*p == 0) {
            __MessageID(0x2988);
            return __ActorMessage(actor, 0);
        }
    } else {
        msgBase = 0x297d;
        p = (unsigned short *)(gs + 0x2b2);
        if (*p == 0)
            goto other;
    }
    __Func_80198dc();
    __Func_8019908(*p, 5);
    __MessageID(msgBase + 1);
    return __ActorMessage(actor, 0);
other:
    __MessageID(0x2989);
    return __ActorMessage(actor, 0);
}
