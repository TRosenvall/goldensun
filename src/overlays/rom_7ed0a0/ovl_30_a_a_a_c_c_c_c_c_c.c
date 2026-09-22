/* Cluster OvlFunc_964_2008ae8..OvlFunc_964_2008ae8 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_a_a_c_c_c_c_c_c.s.
 *
 * Total .text for this TU = 472 bytes (= 0x1d8). Never attempted before batch 279.
 * No pins, no volatile, no flags -- 220 instructions of ordinary C. This file needs no split.
 *
 * ONE OF A THREE-MEMBER DUPLICATE GROUP with OvlFunc_927_2008ae8, OvlFunc_946_2008ae8 and
 * OvlFunc_964_2008ae8, all three verified IDENTICAL (normalising .L<hex> to serial tokens and the
 * OvlFunc_NNN_ prefix) and each objcmp'd against its OWN reference. The only differences across the
 * three are six symbol NAMES -- the function, one callee and a data label -- with NO constant and
 * NO bl target differing. Every host .s holds exactly one function, so all three landed with no
 * split. If you edit one, edit all three.
 *
 * ===== LEVER 1: THE if/else MUST CONTAIN THE CALL, NOT JUST THE ARGUMENT =====
 *
 * Writing the selector as a value and then calling once -- a ternary into a `sel` local, then one
 * __CreateActor -- is 56 differing: the wrong frame size (sub sp,#4 against the ROM's #8), two
 * values swapped between r8 and r10, and three long-lived values in the wrong one of
 * {r11, [sp,#0], [sp,#4]}.
 *
 * DUPLICATING THE CALL INTO BOTH ARMS dropped it to 8 IN ONE STEP. gcc's cross-jumping then merges
 * only the tail (the three argument moves and the bl), stopping where arm 1 has its `mov r2,r6` and
 * arm 2 has `mov r0,#0xde` -- which is exactly the ROM's duplicated `mov r2,r6`.
 *
 * THAT IS A METHOD LESSON, not just a lever: what looked like FIVE allocation defects was ONE
 * cross-jump defect upstream of allocation. The batch-277 priority formula had already been computed
 * on this function and predicted greg's processing order exactly -- 0.0444, 0.0441, 0.0435, 0.0306,
 * 0.0208 -- so THE FORMULA WAS RIGHT AND THE AXIS WAS WRONG. Before spending a round moving live
 * ranges, check whether the BLOCK STRUCTURE is what the ROM has.
 *
 * ===== LEVER 2: A CHAR-POINTER READ TO CREATE THE MEMORY DEPENDENCE sched2 NEEDED =====
 *
 * The last 2 of 223 were one adjacent swap, the ROM loading the byte after moving the high register
 * and ours before. -fsched-verbose=6 shows a genuine tie -- block 10, t=8, both candidates priority
 * 7 -- and store_bit_field ALWAYS masks and shifts the value before reading the destination, so the
 * value load's LUID is structurally lower and the tie is structurally lost. LUID is not the handle.
 *
 * The handle is the DAG. Read as a struct field of a DIFFERENT struct type, strict aliasing proves
 * the load cannot alias the preceding byte store, so it is ready early and independent. Read through
 * a CHAR POINTER it lands in alias set 0, the memory dependence appears, and the ROM's order falls
 * out.
 *
 * AND -fno-strict-aliasing DOES NOT SUBSTITUTE FOR IT -- still 2 differing, and it breaks an
 * unrelated store pair. THE ALIAS SET HAS TO CHANGE AT THE ACCESS, NOT GLOBALLY. That refines the
 * batch-278 note which introduced that flag as the pointer-reload lever: the flag and the cast are
 * not interchangeable.
 *
 * Measured, of 223: the bare struct field 8; either mask operand order 8; an `int` temp 6; a 2-bit
 * BITFIELD in the struct 4 at 225 insns (a bitfield SOURCE extracts with two shifts, not an `and`);
 * a hand-written read-modify-write through a cast struct 11-30 (right order, but it narrows the mask
 * and shares one address copy where the ROM has two); a union 36; an `unsigned char` temp with a
 * compound and 2; THE CHAR-POINTER READ 0. Nineteen flags measured on the 2-differing form, all 2
 * or worse.
 *
 * Also confirmed side by side in one function: the two masked writes to the sprite's +9 are both
 * BITFIELDS (a 32-bit mask built with mov/neg and CSE-shared), while the actor's +0x23 write is
 * HAND-WRITTEN BYTE MASKING -- the same split the sibling documents, here in one place. And the
 * +0x64 field as a plain struct member reproduces the ROM's shared address pseudo, where a named
 * halfword pointer made it WORSE (66).
 */
struct Sprite {
    /* 0x00 */ unsigned char pad0[9];
    /* 0x09 */ unsigned char lo : 2;
               unsigned char sel : 2;
               unsigned char hi : 4;
    /* 0x0a */ unsigned char pad0a[0x14];
    /* 0x1e */ unsigned short ang;
    /* 0x20 */ unsigned char pad20[6];
    /* 0x26 */ unsigned char f26;
};

struct Actor {
    /* 0x00 */ int pad0[6];
    /* 0x18 */ int a, b;
    /* 0x20 */ unsigned char pad20[3];
    /* 0x23 */ unsigned char f23;
    /* 0x24 */ unsigned char pad24[0x0c];
    /* 0x30 */ int da, db;
    /* 0x38 */ int pad38[3];
    /* 0x44 */ int dx, dy, dz;
    /* 0x50 */ struct Sprite *spr;
    /* 0x54 */ unsigned char pad54;
    /* 0x55 */ unsigned char f55;
    /* 0x56 */ unsigned char pad56[0x0e];
    /* 0x64 */ unsigned short spin;
    /* 0x66 */ unsigned char pad66[6];
    /* 0x6c */ void *fn;
};

struct P {
    /* 0x00 */ unsigned char f0;
    /* 0x01 */ unsigned char pad1[3];
    /* 0x04 */ int x4;
    /* 0x08 */ int x8, xc;
    /* 0x10 */ int x10, x14;
    /* 0x18 */ short f18;
    /* 0x1a */ unsigned char pad1a[2];
    /* 0x1c */ void *x1c;
    /* 0x20 */ unsigned short h20, h22;
    /* 0x24 */ void *x24;
};

struct Def { unsigned char pad[0xc]; int step; };

extern struct Actor *__MapActor_GetActor(int slot);
extern struct Actor *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_SetScript(struct Actor *a, void *s);
extern void __Func_80929d8(struct Actor *a, int n);
extern void OvlFunc_964_2008ab0();
extern struct Def *L336c[] __asm__(".L336c");

void OvlFunc_964_2008ae8(int a, int b, int c, int d, int e, int f, int flags, struct P *p)
{
    struct Actor *a0;
    struct Actor *act;
    struct Sprite *spr;
    struct Def *def;

    a0 = __MapActor_GetActor(0);
    if ((flags & 0x100000) && p != 0)
        act = __CreateActor(p->f18, a, b, c);
    else
        act = __CreateActor(0xde, a, b, c);
    if (act == 0)
        return;
    spr = act->spr;
    __Actor_SetAnim(act, (flags + 1) & 0xf);
    __Actor_SetScript(act, L336c[flags & 0xf]);
    act->f55 = 0;
    spr->f26 = 0;
    act->fn = (void *)OvlFunc_964_2008ab0;
    act->dx = d;
    act->dy = e;
    act->dz = f;
    spr->sel = a0->spr->sel;
    act->da = 0;
    act->db = 0;
    act->spin = 0;
    if ((flags & 0xffff0000) == 0)
        return;
    if (p == 0)
        return;
    if (flags & 0x10000)
        __Func_80929d8(act, p->x4);
    if (flags & 0x20000) {
        act->f23 = act->f23 & 0xfe;
        {
            unsigned char v = *(unsigned char *)p;
            v &= 3;
            spr->sel = v;
        }
    }
    if (flags & 0x80000) {
        act->a = p->x8;
        act->b = p->xc;
    }
    if (flags & 0x40000) {
        def = L336c[flags & 0xf];
        if (flags & 0x80000) {
            act->da = (p->x10 - act->a) / def->step;
            act->db = (p->x14 - act->b) / def->step;
        } else {
            act->da = (p->x10 - 0x10000) / def->step;
            act->db = (p->x14 - 0x10000) / def->step;
        }
    }
    if (flags & 0x200000) {
        __Actor_SetAnim(act, 1);
        __Actor_SetScript(act, p->x1c);
    }
    if (flags & 0x400000)
        spr->ang = p->h20;
    if (flags & 0x800000)
        act->spin = p->h22;
    if (flags & 0x1000000)
        act->fn = p->x24;
}
