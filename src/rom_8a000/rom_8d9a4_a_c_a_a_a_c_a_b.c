/* Cluster Func_808e14c..Func_808e14c extracted from goldensun/asm/rom_8a000/rom_8d9a4_a_c_a_a_a_c.s.
 *
 * Total .text for this TU = 240 bytes (= 0xf0). Never attempted before batch 279.
 * No pins, no volatile, no flags. TWO FINDINGS THAT AMEND docs/elevation.md.
 *
 * 1. THE "ldrh/ldrsh CSE BLOCKED SUB-CLASS" IS NOT BLOCKED IN THIS SHAPE. The note on file says
 *    that when an unsigned value is only ever used at 16-bit width the two loads are provably the
 *    same and "no legal C can force a second load". This function is exactly that case -- masks of
 *    0x800 and 0xff -- and it reproduces. The spelling is ONE SIGNED READ MASKED THREE WAYS:
 *
 *        hi  = *(short *)(e + 4) & 0xf000;
 *        bit = *(short *)(e + 4) & (short)0x800;
 *        lo  = *(short *)(e + 4) & 0xff;
 *
 *    gcc emits the ROM's `ldrsh r5,[r6,r3]` AND `ldrh r2,[r6,#4]`. Reading one signed plus one
 *    unsigned variable instead CSEs to a single `ldrsh` with `lsl`/`lsr` -- 54 differing of 112.
 *    So the blocked sub-class needs narrowing: it is blocked when the SAME variable is read twice,
 *    not when one read is masked several ways.
 *
 * 2. `& (short)0x800` IS WHAT POOLS THE MASK. The cast makes the AND HImode, so the constant goes
 *    to the pool as `ldrh r3, .L / .word 2048` -- which is the ROM's `ldr r3, =0x800`. Without the
 *    cast gcc synthesises `mov r3, #0x80 / lsl r3, #4`. **A POOLED *SHIFTABLE* CONSTANT CAN BE A
 *    HImode TELL RATHER THAN A SYMBOL TELL**, which matters because the shiftability argument is
 *    the strongest one in message.sym and const.sym. Check the mode before reading a symbol into it.
 *    (Casting a separately-read unsigned value both ways pools the mask but drags the byte mask
 *    into HImode too: 61 differing.)
 *
 * THE LAST INSTRUCTION was the loop-bottom merge copy `mov r1, r3`. A `for(;;)` with the test
 * inside has everything else exact but gcc coalesces the copy away; an explicit do/while bottom
 * test produces the copy but then cross-jumps the two `return e` tails and loses a `mov r0, r6`.
 * The form that gets both is THE COPY BEFORE THE TEST:
 *
 *        for (;;) { n = *(int *)e; flags = n; if (n == -1) break; ...
 *
 * None of the five allocation entry points applied: the ladder was LICM, then CSE, then
 * `local_alloc` coalescing of a loop-exit-test duplicate, fixed by source placement.
 */
extern unsigned char gState[];
extern unsigned char iwram_3001ebc[];
extern unsigned char *GetFieldActor(int a);
extern int Func_808ddec(int a);
extern int Func_808bd24(void);
extern unsigned int Func_808d428(int x);

char *Func_808e14c(int arg)
{
    unsigned char *gs;
    char *e;
    int key;
    int r9;
    int r10;
    int r11;
    int flags;
    int n;
    int flags2;
    int hi;
    short bit;
    int lo;
    int b1;

    e = *(char **)(*(unsigned char **)iwram_3001ebc + 0x10);
    gs = gState;
    gs += 0xfa << 1;
    r11 = *(unsigned short *)(GetFieldActor(*(int *)gs) + 6);
    r9 = Func_808ddec(*(int *)gs);
    key = arg & 0x1ff;
    r10 = Func_808bd24();
    for (;;) {
        n = *(int *)e;
        flags = n;
        if (n == -1) {
            break;
        }
        hi = *(short *)(e + 4) & 0xf000;
        bit = *(short *)(e + 4) & (short)0x800;
        lo = *(short *)(e + 4) & 0xff;
        if ((flags & 0xf) == 4 && Func_808d428(*(short *)(e + 6)) != 0) {
            if (bit == 0 || (unsigned short)((hi - r11) + 0x17ff) <= 0x2ffe) {
                flags2 = *(int *)e;
                b1 = *(unsigned char *)(e + 1);
                if (key == 0 || b1 == key) {
                    if ((flags2 & 0x10) != 0) {
                        if (lo == r9) {
                            return e;
                        }
                    } else {
                        if (lo == r10) {
                            return e;
                        }
                    }
                }
            }
        }
        e += 0xc;
    }
    return 0;
}
