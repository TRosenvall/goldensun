/* OvlFunc_896_200c260 -- 0x0200c260  (asm/overlays/rom_78ef88/ovl_314_c_c_c_c_a.s)
 *
 * BLOCKER: register-role rotation across the whole function. 78 of 85, one line
 * short, and the instruction STREAM is very nearly 1:1 -- this is the
 * "same instructions, one register rotation" signature, recognisable from the
 * first twenty lines without diffing the rest.
 *
 *     rom   r6 = actor, r7 = party member, r8 = zero, r9 = item flag
 *     ours  r7 = actor, r8 = party member, r5 = zero, r9 = item flag
 *
 * The high-register prologue is identical -- both save r8, r9 and r10 -- so this
 * is not a pressure difference. It is which value got which slot.
 *
 * The one structural signal that looked reachable and is not: the ROM
 * materialises its zero BEFORE the first call (`mov r0,#0 / mov r8,r0`, held
 * across three calls) where we build it lazily just before its first use. The
 * zero is genuinely used later, at two `strb` sites, so the source reads the
 * same either way. Assigning it as the FIRST statement of the function is
 * byte-identical, and so is dropping the named local for the galloc size
 * alongside it. Statement order does not reach this.
 *
 * Everything the levers do reach is already applied and correct: the galloc
 * size named for its interleaved build, the -0x21 mask named as an `int` so it
 * is not narrowed to 0xdf on the byte operation, and the two adjacent byte
 * stores at 0x26/0x27 written as separate subscripts so the ROM's
 * add-then-increment addressing appears.
 *
 * NEXT: nothing in the inventory. Worth re-reading only if a lever for the
 * register-role swap is ever found; the body is otherwise a clean read.
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
extern void __Func_8078948(int a, int b);
extern void __GiveItemTo(int a, int b);
extern void __DeleteActor(void *a);
extern void __MapActor_SetAnim(int a, int b);

int OvlFunc_896_200c260(int item)
{
    unsigned char *act;
    unsigned char *o;
    unsigned char *buf;
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
    buf = __galloc_iwram(0x11, g1);
    __LoadItemIcon(item);
    __UploadSpriteGFX(o[0x1c], 0x80, buf + (0x80 << 3));
    __gfree(0x11);
    __PlaySound(0x53);
    __Func_808f140(act, 3);
    __Func_8078948(who, has);
    __GiveItemTo(who, item);
    __DeleteActor(act);
    __MapActor_SetAnim(0, 1);
    return who;
}
