/* Cluster ShowDamageNumbers..ShowDamageNumbers extracted from
 * goldensun/asm/rom_b5000/rom_b9b30_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_b5000/rom_b9b30_c_a_c_a.o and asm/rom_b5000/rom_b9b30_c_a_c_c.o in
 * goldensun/stage1.ld.
 */
/* Func_80babdc @ 0x080babdc -- ShowDamageNumbers.
 *
 * Puts the damage overlay up through rom_15000's _Func_801f200, counting the
 * actor's parts with Func_80ba918 and pricing with _Func_802281c.  Two
 * iterations of a do/while whose counter lives in r8 (`mov r3,#1 / neg r3,r3 /
 * add r8,r3` -- Thumb has no `sub` immediate for a high register).
 *
 * TWO LEVERS, and they pull against each other.
 *
 *   THE FRAME ADDRESS IS MATERIALISED BY A NAMED POINTER, NOT BY LICM.  The
 *   ROM's preheader is `mov r3,#1 / mov r6,sp / mov r8,r3`; writing the buffer
 *   as a plain array gives `mov r3,#1 / mov r8,r3 / mov r6,sp`.  Nothing is
 *   scheduled differently -- sched2 keeps whatever order flow2 hands it, and
 *   rank_for_schedule's last tie-break is INSN_LUID.  With the array spelling
 *   `mov r6,sp` is a loop movable, and move_movables emits movables immediately
 *   before NOTE_INSN_LOOP_BEG, i.e. AFTER `i = 1`, so it can only ever carry the
 *   higher LUID.  `short *p = buf;` written before `i = 1` is a real insn with
 *   the lower LUID, and then sched2 issues `mov r3,#1` first (it has a
 *   dependent, so a longer path), leaves `mov r6,sp` and `mov r8,r3` tied, and
 *   breaks the tie on LUID -- which is the ROM's order.
 *
 *   NAMING THE POINTER COSTS THE 0xff, AND AN int BUYS IT BACK.  `short buf[2]`
 *   is four bytes, so with no explicit address-taking gcc-2.96 expands it into a
 *   single SImode PSEUDO and reaches the halves with and/ior inserts; the
 *   addressof pass later spills it and purge_addressof rewrites the inserts as
 *   MEM stores, but the 255 stays in the SImode pseudo the ior built, so it
 *   emits `mov r3,#255`.  Assigning `p = buf` takes the address at expansion
 *   time, buf goes straight to the stack, and the store expands as
 *   `(set (mem:HI ...) (reg:HI))` with a HImode constant -- and *thumb_movhi_insn
 *   has no immediate alternative for one, so gcc emits `ldrh r3,.L7` plus a
 *   `.word 255` pool and a `b` around it: three instructions worse than the
 *   array spelling's two-instruction win.  Routing the constant through an int
 *   restores the SImode source operand and with it `mov r3,#255`.
 *
 * MEASURED, all against the reference's 58 encodings:
 *   plain `short buf[2]`, address taken implicitly     2 differ (the park)
 *   + __asm__("r6") pin on a named pointer            26 differ, 61 encodings
 *   + __asm__("r6") and __asm__("r8") pins            26 differ, 61 encodings
 *   pins declared in the reverse order                28 differ, 61 encodings
 *   named pointer, no pin                             26 differ, 61 encodings
 *   pin on the pointer but array-spelled stores       58 differ, 64 encodings
 *   named pointer + int-routed 0xff                   EXACT
 *
 * The three pinned rows are the informative ones: the pin is INERT here.  It
 * fixes the same two encodings the bare named pointer already fixes and nothing
 * else, because the lever is the LUID of a real assignment and `p = buf` is
 * already one.  Reversing the declaration order costs the two encodings back.
 */

extern unsigned char *iwram_3001e74;

extern void _GetUnit(int id);
extern void **GetBattleActor(int id);
extern void _Actor_SetAnim(void *actor, int anim);
extern void _Func_802281c(short *req);
extern void Func_80ba918(void *actor, int part);
extern void WaitFrames(int frames);
extern int Func_80b6cd0(int id);
extern void _Func_801f200(int n);

void Func_80babdc(int id)
{
    short buf[2];
    short *p;
    void **actor;
    int i;
    int flag;

    _GetUnit(id);
    _Actor_SetAnim(*GetBattleActor(id), 5);

    p = buf;
    i = 1;
    do {
        flag = 0xff;
        p[1] = flag;
        p[0] = id;
        _Func_802281c(p);
        Func_80ba918(*GetBattleActor(id), 7);
        WaitFrames(2);

        p[0] = id;
        _Func_802281c(p);
        actor = GetBattleActor(id);
        Func_80ba918(*actor, Func_80b6cd0(id));
        WaitFrames(2);

        i--;
    } while (i >= 0);

    _Func_801f200(iwram_3001e74[0x41]);
}
