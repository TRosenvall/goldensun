/* OvlFunc_916_2008c2c  --  0x02008c2c, cut from
 * goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_c_c.s.
 *
 * EXACT.  Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_c_c.c \
 *     asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_c_c.s --func OvlFunc_916_2008c2c
 * 568 bytes, 249 encodings, 29 relocations identical.  No shims, no pins.
 * No TEXT/DATA split: `datacheck` prints nothing, and the three tables the tail
 * indexes (`.L1164`, `.L1168`, `.L116c`) are global in another object, so they
 * are reached with the asm-label extern (four digits each, so gcc's own `.LN`
 * labels cannot capture them).
 *
 * The walk: place the actor's tile centre, ask OvlFunc_916_2008b8c for a slot
 * record, then step the record up to eleven times -- each step rotates the
 * record's (x,z) by the actor's facing and hands it to OvlFunc_916_2008be4,
 * which stops the walk when it refuses.  If any step was taken, replay the
 * actor's centre and run the push cutscene.
 *
 * THREE LEVERS DID ALL THE WORK, in this order.
 *
 * 1. ONE variable for the two actors.  r7 holds the map actor for the whole
 *    body and then `s->f8` for the whole cutscene tail.  Written as two locals
 *    the function is FOUR INSTRUCTIONS SHORT: with two short live ranges gcc
 *    gives the loop's vec3 pointer r7 and pushes the map actor into r8, where
 *    every dereference costs a `mov` and the target `tx` spills.  One local,
 *    with the long live range and the high reference count that comes with it,
 *    outranks the loop pointer in global-alloc's priority; the map actor then
 *    keeps r7 and the loop pointer falls to r4 -- which `-fcall-used-r4` makes
 *    call-clobbered, so it is CALLER-SAVED around the two in-loop calls
 *    (`str r4, [sp]` / `ldr r4, [sp]`), which is where the four instructions
 *    are.  235 -> 239, count exact.
 *
 * 2. TWO SPELLINGS of the same vec3 keep the two pointers apart.  The loop
 *    writes the vector through the array name `v` and reads it back through the
 *    pointer `q`; both are `sp+8` and CSE turns the loop's address computation
 *    into `mov r4, r6`, but the pseudos stay distinct because their live ranges
 *    overlap.  Writing `p = q` instead lets copy propagation delete `p` and the
 *    second pointer disappears.  The split is not cosmetic either: the `tx`/`tz`
 *    arms must read `v[0]`/`v[2]` and the `s->f2`/`s->f4` stores `q[0]`/`q[2]`,
 *    or gcse commons the two loads of the same word across the if/else merge and
 *    the whole arm pair comes out a register short (43 differing -> 2).
 *
 * 3. A DOMINATING-BLOCK LOCAL for 0x4ccc.  The last residue was one swapped
 *    pair: the ROM issues `mov r0, #0` before `ldr r1, =0x4ccc`, gcc the other
 *    way round, because `precompute_register_parameters` (calls.c:805) hoists
 *    any argument costing more than one insn ahead of every hard-register load.
 *    `-fno-schedule-insns2` is inert here, so it is expand, not sched2, and the
 *    recorded table says the shape is unreachable for a literal.  Naming the
 *    constant in the block that dominates the call -- and NOT immediately before
 *    it -- makes the pseudo cross a call, which sends it down local-alloc's
 *    REG_EQUIV path: the set is deleted and the constant is REMATERIALISED at
 *    the argument slot, after `mov r0, #0`.  Assigned one statement before the
 *    call it coalesces into r1 and the load stays early (still 2); assigned
 *    inside the cutscene tail it wins a callee-saved register instead and costs
 *    two instructions (241).  The return-type lever was tried first on both
 *    `__MapActor_SetSpeed` and `__Camera_SetTarget` and is wrong in both
 *    directions here (19 and 21 differing) -- `void` is right for both.
 *
 * Smaller readings worth keeping: `ang` is `(0x2000 + actor->f6) & 0xc000`, a
 * quadrant, and `dir = ang / 0x4000` is the 0..3 index into the three tables --
 * written as a DIVISION, which is what the ROM's `cmp #0 / bge / add 0x3fff /
 * asr #14` is.  The four `/ 0x100000` sites are the same idiom at shift 20.
 * `0x80 << 12` is written in three places and CSEd into one register, which is
 * why the second `__vec3_translate` gets 0x80000 rather than the first call's
 * 0x100000.  The `ldrsh [r5, rN]` loads need no help: Thumb-1 `ldrsh` has only
 * a register-offset form, so `mov rN, #2 / ldrsh` is forced.
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x1c];
    int f30;
    int f34;
};

struct Slot {
    unsigned char pad00[2];
    short f2;
    short f4;
    short f6;
    struct Actor *f8;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __vec3_translate(int dist, int ang, int *v);
extern struct Slot *OvlFunc_916_2008b8c(int a, int x, int z);
extern int OvlFunc_916_2008be4(int x, int z, int d);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int slot, int n);
extern void __PlaySound(int id);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct Actor *a);
extern unsigned char *__galloc_ewram(int tag, int size);
extern void __Camera_SetTarget(int cam, struct Actor *a);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_809228c(int slot, int a, int b);
extern unsigned char L1164[] __asm__(".L1164");
extern signed char L1168[] __asm__(".L1168");
extern signed char L116c[] __asm__(".L116c");

void OvlFunc_916_2008c2c(int a)
{
    int v[3];
    int *q;
    struct Actor *ac;
    struct Slot *s;
    int ang;
    int flag;
    int i;
    int tx, tz;
    int dir;
    int spd;

    flag = 0;
    ac = __MapActor_GetActor(0);
    ang = ((0x80 << 6) + ac->f6) & (0xc0 << 8);
    q = v;
    q[0] = (ac->f8 & 0xfff00000) + (0x80 << 12);
    q[1] = ac->fc;
    q[2] = (ac->f10 & 0xfff00000) + (0x80 << 12);
    __vec3_translate(0x80 << 13, ang, q);
    s = OvlFunc_916_2008b8c(a, q[0] / 0x100000, q[2] / 0x100000);
    if (s != 0) {
        i = 0;
        do {
            v[0] = s->f2 << 20;
            v[2] = s->f4 << 20;
            __vec3_translate(0x80 << 13, ang, v);
            if (OvlFunc_916_2008be4(v[0] / 0x100000, q[2] / 0x100000, s->f6) != 0)
                break;
            flag = 1;
            if (s->f6 == 0) {
                tx = v[0] + (0x80 << 14);
                tz = v[2] + (0x80 << 12);
            } else {
                tx = v[0] + (0x80 << 12);
                tz = v[2] + (0x80 << 14);
            }
            s->f2 = q[0] / 0x100000;
            s->f4 = q[2] / 0x100000;
            i++;
        } while (i <= 0xa);
        if (flag != 0) {
            spd = 0x4ccc;
            q[0] = (ac->f8 & 0xfff00000) + (0x80 << 12);
            q[1] = ac->fc;
            q[2] = (ac->f10 & 0xfff00000) + (0x80 << 12);
            __vec3_translate(0x80 << 12, ang, q);
            ac = s->f8;
            dir = ang / 0x4000;
            __CutsceneStart();
            __MapActor_SetAnim(0, 8);
            __CutsceneWait(6);
            ac->f30 = 0x80 << 8;
            ac->f34 = 0x3333;
            __PlaySound(0xef);
            __Actor_SetAnim(ac, L1164[dir]);
            __Actor_TravelTo(ac, tx, 0, tz);
            __CutsceneWait(6);
            __MapActor_SetAnim(0, 2);
            __Camera_SetTarget(*(int *)(__galloc_ewram(0x1b, 0xccc) + (0xf0 << 1)), ac);
            __MapActor_SetSpeed(0, spd, 0x3333);
            __Func_809228c(0, L1168[dir], L116c[dir]);
            __CutsceneWait(0x18);
            __MapActor_SetAnim(0, 1);
            __Actor_WaitMovement(ac);
            __Actor_SetAnim(ac, 1);
            __PlaySound(0x90 << 1);
            __PlaySound(0xd5);
            __CutsceneWait(0xf);
            __CutsceneEnd();
        }
    }
}
