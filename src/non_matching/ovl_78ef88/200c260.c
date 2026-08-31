/* OvlFunc_896_200c260 -- 0x0200c260  (asm/overlays/rom_78ef88/ovl_314_c_c_c_c_a.s)
 *
 * BLOCKER: which scratch register a temporary constant gets. 4 of 85, exact
 * length. Was parked at 78 of 85 as "register-role rotation, nothing in the
 * inventory reaches it". That was WRONG, and the correction is the useful part.
 *
 * THE FIX WAS A VARIABLE COUNT, NOT AN ALLOCATOR PROBLEM. The ROM uses r8 for a
 * zero -- stored to o[0x26] and o[0x27] -- and then reuses r8 for the
 * __galloc_iwram result. Two values with nothing in common sharing one
 * register. Written as two locals (`z` and `buf`) gcc spreads every value one
 * slot and the whole function rotates. Written as ONE local holding the zero
 * and then the pointer, 78 differing collapses to 6.
 *
 * That is the inverse of "a variable with disjoint live ranges should be two
 * variables", and it is the same lever that closed OvlFunc_954_2008490 in the
 * same round. ONE REGISTER FOR TWO UNRELATED VALUES MEANS ONE VARIABLE. It
 * reads badly -- the local holds a zero and then a buffer address -- but the
 * allocator will not be argued into the ROM's assignment any other way.
 *
 * Deleting __Func_8078948's prototype took 6 to 4: its two arguments filled r0
 * before r1 where the ROM does the reverse, which is the no-prototype lever's
 * exact signature and its fourth confirmation.
 *
 * WHAT REMAINS is four instructions, all one thing:
 *
 *     rom   mov r10, r0 / mov r0, #0 / mov r8, r0 / mov r0, #0x16
 *     ours  mov r2, #0 / mov r10, r0 / mov r0, #0x16 / mov r8, r2
 *
 * The ROM builds the zero in r0 -- the parameter register, free the moment the
 * parameter is saved to r10 -- and we build it in r2. Four placements screened,
 * all 4 differing: assignment first, assignment last, assignment immediately
 * before the first call, and declaration order.
 *
 * LESSON FOR THE OTHER ROTATION PARKS: before recording one as unreachable,
 * check whether the ROM REUSES a register for two unrelated values. If it does,
 * the park is a variable-count question rather than an allocator question, and
 * it may be four instructions away rather than eighty.
 */
extern unsigned char gScript_881__0200cbe4[];
extern unsigned char *__CreateActor(int a);
extern int __CheckPartyItem(int item);
extern int __CheckItem(int a, int item);
extern void __Actor_SetScript(void *a, void *s);
extern unsigned char *__galloc_iwram(int a, int b);
extern void __gfree(int a);
extern void __LoadItemIcon(int id);
extern void __UploadSpriteGFX(int a, int b, void *p);
extern void __PlaySound(int id);
extern void __Func_808f140(void *a, int b);
extern void __GiveItemTo(int a, int b);
extern void __DeleteActor(void *a);
extern void __MapActor_SetAnim(int a, int b);

int OvlFunc_896_200c260(int item)
{
    unsigned char *act;
    unsigned char *o;
    int who;
    int has;
    int z;
    int g1;
    int m;

    g1 = 0xc1 << 3;
    m = -0x21;
    z = 0;
    act = __CreateActor(0x16);
    who = __CheckPartyItem(0xe0);
    has = __CheckItem(who, 0xe0);
    if (act == 0)
        return who;
    __Actor_SetScript(act, gScript_881__0200cbe4);
    o = *(unsigned char **)(act + 0x50);
    o[0x26] = z;
    o[0x27] = z;
    o[5] &= m;
    o[9] &= 0xf;
    *(int *)(act + 0x28) = 0xa0 << 10;
    *(int *)(act + 0x48) = 0x80 << 7;
    z = (int)__galloc_iwram(0x11, g1);
    __LoadItemIcon(item);
    __UploadSpriteGFX(o[0x1c], 0x80, (unsigned char *)z + (0x80 << 3));
    __gfree(0x11);
    __PlaySound(0x53);
    __Func_808f140(act, 3);
    __Func_8078948(who, has);
    __GiveItemTo(who, item);
    __DeleteActor(act);
    __MapActor_SetAnim(0, 1);
    return who;
}
