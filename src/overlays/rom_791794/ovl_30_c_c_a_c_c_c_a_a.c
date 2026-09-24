/* OvlFunc_897_200a9a4 -- asm/overlays/rom_791794/ovl_30_c_c_a_c_c_c_a_a.s.
 *
 * The set-piece opener: clear sixteen field actor slots, pick the area fade by
 * mode, then spawn ten actors off a position table, arm the per-actor timers one
 * every six frames, wait for the companion task to drain them, and fade out.
 *
 * FOUR LEVERS.
 *
 * 1. `mode` IS UNSIGNED, and the switch tree says so.  gcc's four-case tree for
 *    a SIGNED selector is `cmp #1 / beq / cmp #1 / bgt / cmp #0 / beq / b`; for
 *    an unsigned one it folds "< 1" into "== 0" and emits the ROM's
 *    `cmp #1 / beq / cmp #1 / bcc`.  Two extra instructions per tree, and the
 *    tree appears TWICE, so this is worth six encodings on its own.  A `bcc`
 *    or `bcs` against a case value that is not a range bound is the tell.
 *
 * 2. THE INVARIANT ZERO NEEDS A NAME, AND THE LOOP HAS TO BE A `while`.  The
 *    spawn loop stores 0 to gTimers[i] and to two actor bytes, so gcc hoists one
 *    zero to a callee-saved register either way -- but as a hoisted invariant its
 *    `mov r9, r6` lands AFTER `mov r10, r0`, and the ROM has it before.  The fix
 *    is all three parts together: `i = 0; zero = 0; t = gSpots;` as three
 *    statements ahead of `while (i <= 9)`.  With a `for (i = 0; ...)` header the
 *    named zero is 169 differing -- strictly worse than the 2 the unnamed
 *    hoisted zero gives -- because the for-init re-sorts the three defs.  The
 *    named zero is only right in combination with the explicit `i = 0` first.
 *
 * 3. `__MapActor_Emote`'s 0x100 IS BUILT TWICE, ONCE PER CALL.  The two calls
 *    sit in one basic block, so gcc commons `mov #0x80 / lsl #1` into a
 *    callee-saved register and then spends a `mov r1, r5` at each site.  A
 *    three-register pin per site with the mov/lsl pair SPLIT in the source
 *    (`q1 = 0x80; q0 = <slot>; q1 <<= 1; q2 = 0;`) reproduces the ROM's
 *    `mov r1,#128 / mov r0,#N / lsl r1,#1 / mov r2,#0` exactly.
 *
 * 4. `t->y` NEEDS A NAME.  Left in the __CreateActor argument list it is loaded
 *    after the switch; the ROM loads both table words together at the top of the
 *    iteration.
 *
 * The `neg` of 0xd is the batch-71 bitfield, read the way
 * rom_77a7c8/ovl_30_a_a_a_c_b.c records it: -13 clears bits 2 and 3 only, so it
 * is a two-bit field at offset 2 and the `orr #4` is the value 1.
 *
 * VERIFIES: 508 bytes, 217 encodings and 32 relocations identical.
 */
struct Part {
    unsigned char pad00[9];
    unsigned char b0 : 1,
                  b1 : 1,
                  b2 : 2,
                  b4 : 4;
    unsigned char pad0a[0x26 - 0xa];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[0x50];
    struct Part *part;
    unsigned char pad54;
    unsigned char f55;
};

struct Spot {
    int x;
    int y;
};

extern struct Spot gSpots[] __asm__(".L3684");
extern unsigned int gTimers[] __asm__(".L3b40");
extern struct Actor *gEnts[] __asm__(".L3b10");

extern void __DeleteFieldActor(int slot);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __PlaySound(int id);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __WaitFrames(int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern int __StartTask(void (*fn)(void), int prio);
extern void __StopTask(void (*fn)(void));
extern void OvlFunc_897_200aba0(void);

void OvlFunc_897_200a9a4(unsigned int mode)
{
    struct Actor *a;
    struct Part *p;
    struct Spot *t;
    unsigned int i;
    int x, y, z;
    int zero;

    for (i = 0; i <= 0xf; i++)
        __DeleteFieldActor(i + 0x10);
    switch (mode) {
    case 0:
        __Func_8091200(0x4039d2, 1);
        break;
    case 1:
        __Func_8091200(0x4049d2, 1);
        break;
    case 2:
        __Func_8091200(0x404a4e, 1);
        break;
    case 3:
        __Func_8091200(0x403a52, 1);
        break;
    }
    __Func_8091254(0x3c);
    __PlaySound(0xd6);
    i = 0;
    zero = 0;
    t = gSpots;
    while (i <= 9) {
        x = t->x;
        y = t->y;
        z = 0;
        switch (mode) {
        case 0:
            x += 0xe8 << 16;
            z = 0x90 << 16;
            break;
        case 1:
            x += 0xe8 << 16;
            z = 0xe8 << 17;
            break;
        case 2:
            x += 0x2c70000;
            z = 0x90 << 16;
            break;
        case 3:
            x += 0x2c70000;
            z = 0xe8 << 17;
            break;
        }
        gTimers[i] = zero;
        a = __CreateActor(0x8e << 1, x, y, z);
        gEnts[i] = a;
        a->f55 = zero;
        p = a->part;
        p->f26 = zero;
        p->b2 = 1;
        __Actor_SetAnim(a, 6);
        __WaitFrames(6);
        t++;
        i++;
    }
    if (mode == 0) {
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x80;
            q0 = 0;
            q1 <<= 1;
            q2 = 0;
            __MapActor_Emote(q0, q1, q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x80;
            q0 = 1;
            q1 <<= 1;
            q2 = 0;
            __MapActor_Emote(q0, q1, q2);
        }
    }
    __WaitFrames(0x14);
    __StartTask(OvlFunc_897_200aba0, 0xc8 << 4);
    __PlaySound(0xf6);
    gTimers[0] = 1;
    __WaitFrames(6);
    gTimers[1] = 1;
    __WaitFrames(6);
    gTimers[2] = 1;
    __WaitFrames(6);
    gTimers[3] = 1;
    __WaitFrames(6);
    gTimers[4] = 1;
    __WaitFrames(6);
    gTimers[5] = 1;
    __WaitFrames(6);
    gTimers[6] = 1;
    __WaitFrames(6);
    gTimers[7] = 1;
    __WaitFrames(6);
    gTimers[8] = 1;
    __WaitFrames(6);
    gTimers[9] = 1;
    __WaitFrames(6);
    for (;;) {
        for (i = 0; i <= 9; i++) {
            if (gTimers[i] != 0) {
                i = 0xde << 2;
                break;
            }
        }
        if (i != 0xde << 2)
            break;
        __WaitFrames(1);
    }
    __WaitFrames(0x28);
    __StopTask(OvlFunc_897_200aba0);
    __Func_8091200(0x80 << 9, 1);
    __Func_8091254(0x28);
}
