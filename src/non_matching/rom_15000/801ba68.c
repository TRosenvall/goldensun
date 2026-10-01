/* Func_801ba68 / AnimatePanelTransition (0x0801ba68) -- NON-MATCHING: 172 encodings of 218 differ (objcmp).
 * Reference 218 instructions, ours 203; reference 460 bytes, ours 428. A count is NOT
 * a distance while those disagree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801ba68.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_c.s --func Func_801ba68
 *
 * tools/datacheck.py reports no data section for rom_1aeec_a_a_c_c.s, but the file
 * still holds five functions, so landing this one needs a text-only split.
 *
 * WHAT THE STRUCT LEVER BOUGHT, and it is the reusable part: 211 -> 185 differing
 * and the ENTIRE opening now exact. Typing the first parameter as `struct W *` with a
 * member array at 0x374 and 0x354 instead of `unsigned char *` plus casts is what
 * puts the base in a LOW callee-saved register (r7) and produces the ROM's
 * `add r3, r2, r1 / ldrh r3, [r7, r3]` -- base in the register, whole displacement in
 * the index. With `unsigned char *` gcc folds the base into the index sum, lands the
 * pointer in r8, and every access costs a different shape. This is the batch-290
 * "member array keeps base and offset in separate registers" note, and here it also
 * decides the REGISTER CLASS of the parameter.
 *
 * BLOCKER: reload_cse_move2add CHAINING OF THE ADDRESS CONSTANTS, which is downstream
 * of which hard register each one gets. The ROM and gcc chain DIFFERENT ones:
 *   rom   mov r4,#0xd5 / lsl r4,#2      (0x354 built fresh)
 *   ours  sub r0,#0x48                  (0x354 derived from 0x39c, already in r0)
 *   rom   sub r4,#0x50                  (0x348 derived from 0x398, already in r4)
 *   ours  mov r0,#0xd2 / lsl r0,#2      (0x348 built fresh)
 * The ROM keeps a dedicated scratch (r4, call-used here via -fcall-used-r4) for the
 * 0x3xx family and chains inside it; gcc spreads them over r0/r1 and chains the other
 * pair. Each mismatched chain is worth one instruction, and they account for most of
 * the 15-instruction shortfall.
 *
 * ALSO OPEN: `q->f18 = q->f10 - 0x10` on an `unsigned short` field is folded to
 * `+ 0xfff0`, which is not a Thumb immediate, so gcc pools it and adds; the ROM has
 * `sub r3, #0x10`. An int carrier for the loaded field improves the encoding count
 * (185 -> 173) but does not restore the instruction.
 *
 * MEASURED: plain `while` walk loops 185 differing at 206 instructions; explicit
 * `goto` walk loops 197 at 214 (closer in count, further in encodings, because the
 * defeated LICM re-loads the pooled constant inside the loop); int carrier for the
 * f10 read 172 at 203 (this file).
 *
 * WHAT IS RIGHT: both symmetric branches, the two guarded do-while list walks (first
 * node peeled in branch A, LAST node fixed up in branch B), the `WaitFrames` polls on
 * the signed f22 field against 0 and 0x100, the tail list walk, and the ROM's own
 * quirk `p->f18 = y + 0x10000` -- a store that strh truncates back to y, which must be
 * written as the ROM computes it.
 */
struct P {
    struct P *f0;
    struct P *f4;
    unsigned char pad08[2];
    unsigned short fa;
    unsigned short fc;
    unsigned char pad0e[2];
    unsigned short f10;
    unsigned short f12;
    unsigned short f14;
    unsigned char pad16[2];
    unsigned short f18;
    unsigned short f1a;
    unsigned char pad1c[6];
    short f22;
    unsigned short f24;
    unsigned short f26;
};

struct W {
    unsigned char pad000[0x348];
    struct P *head;
    unsigned char pad34c[0x354 - 0x34c];
    unsigned short a354[16];
    unsigned short a374[17];
    unsigned short f396;
    unsigned short f398;
    unsigned short f39a;
    unsigned short f39c;
};

extern struct P *Func_801a910(int a);
extern int Func_801bd98(int a, int b, struct P *p, int d);
extern int WaitFrames(int n);
extern int Func_8003f3c(int id);

void Func_801ba68(struct W *w, struct P *arg)
{
    struct P *p;
    struct P *q;
    struct P *n;
    int i;
    int a, b;
    int y, z;
    int u;

    if (arg != 0) {
        i = w->f39c + 4;
        a = w->a374[i];
        b = w->a354[i];
        p = Func_801a910(0);
        if (p == 0)
            return;
        Func_801bd98(b, a, p, 0);
        y = w->f396;
        p->f10 = y + 0x50;
        z = w->f398;
        p->f18 = y + 0x40;
        p->f12 = z;
        p->f1a = z;
        p->f24 = 0x20;
        p->f22 = 0x20;
        p->f26 = 0x100;
        p->f14 = 0xfffe;
        q = w->head;
        q->f24 = 0xffe0;
        u = q->f10;
        q->f18 = u - 0x10;
        n = q->f4;
        q->f26 = 0;
        q->f14 = 0xfffe;
        while (n != 0) {
            q = n;
            u = q->f10;
        q->f18 = u - 0x10;
            n = q->f4;
            q->f14 = 0xfffe;
        }
        q->f4 = p;
        p->f4 = 0;
        p->f0 = q;
        q = w->head;
        do {
            WaitFrames(1);
        } while (q->f22 != 0);
        w->head = q->f4;
        Func_8003f3c(q->fc);
        q->fa = 0;
        q = q->f4;
        q->f0 = 0;
    } else {
        i = w->f39c;
        a = w->a374[i];
        b = w->a354[i];
        p = Func_801a910(0);
        if (p == 0)
            return;
        Func_801bd98(b, a, p, 0);
        y = w->f396;
        p->f10 = y + 0xfff0;
        z = w->f398;
        p->f12 = z;
        p->f1a = z;
        p->f14 = 2;
        p->f22 = 0x20;
        p->f24 = 0x20;
        p->f18 = y + 0x10000;
        p->f26 = 0x100;
        q = w->head;
        q->f0 = p;
        p->f4 = q;
        p->f0 = arg;
        w->head = p;
        q = p;
        u = q->f10;
        q->f18 = u + 0x10;
        n = q->f4;
        q->f14 = 2;
        while (n != 0) {
            q = n;
            u = q->f10;
        q->f18 = u + 0x10;
            n = q->f4;
            q->f14 = 2;
        }
        q->f26 = 0;
        q->f24 = 0xffe0;
        q = w->head;
        do {
            WaitFrames(1);
        } while (q->f22 != 0x100);
        n = q->f4;
        while (n != 0) {
            q = n;
            n = q->f4;
        }
        Func_8003f3c(q->fc);
        p = q->f0;
        q->fa = 0;
        p->f4 = 0;
    }
}
