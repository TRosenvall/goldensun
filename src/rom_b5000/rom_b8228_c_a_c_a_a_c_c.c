/* Func_80b874c  --  0x080b874c
 *
 * The whole of goldensun/asm/rom_b5000/rom_b8228_c_a_c_a_a_c_c.s: one function,
 * no data, default flags.
 *
 * NAMING A CONSTANT TO REORDER GLOBAL ALLOCATION -- a new lever, and the
 * diagnostic for it is a line the compiler prints.
 *
 * Two cross-block pseudos were rotated: a pointer loaded early and a two-way
 * constant built late. global.c ranks allocnos by
 * floor_log2(refs) * refs / live_length * size, higher first, so the shorter
 * range normally wins. Assigning the SECOND store's literal to a named local
 * placed BEFORE both stores moves its set one slot earlier in the join block,
 * lengthens the competing pseudo's live range, and flips the order printed by
 * `;; N regs to allocate:`. That one flip fixed both remaining residues --
 * including a second, apparently unrelated one back at function entry, where an
 * ldrsh scratch moved register.
 *
 * The recipe needs no guessing: diff the `;; N regs to allocate:` line and the
 * `;; Register dispositions:` block between candidate and reference, and if the
 * ORDER is wrong, change a LIVE LENGTH rather than a spelling.
 *
 * POSITION IS LOAD-BEARING; NAMING ALONE IS NOT. The constant named before both
 * stores matches, and named as the function's first statement also matches --
 * update_equiv_regs sinks the equivalent constant to the same place. Named
 * BETWEEN the two stores, or before the guard, it is identical to not naming it
 * at all. This is the batch-185 rule again: every "name the value" lever has a
 * placement and the placement is part of the lever.
 *
 * A THUMB ldrsh OFFSET REGISTER IS A CLOBBER, NOT A USER PSEUDO. The
 * `mov rX, #0 / ldrsh rD, [rB, rX]` pair comes from the sign-extend pattern's
 * own clobber, chosen by reload against whatever is live. It is not reachable by
 * naming an offset variable, so the [offset] guidance does not apply to ldrsh at
 * all -- a third independent confirmation of that this batch.
 *
 * THE ldrsh/strh PAIR ORDER IS SET BY THE SOURCE, NOT BY sched2. Across every
 * candidate sched2 moved the pointer load and the pool constant freely across
 * the store, but never reordered the store against the sign-extending load. To
 * get the ROM's order the read has to happen into a named local BEFORE the
 * store statement -- and the call result has to be named too, so the store can
 * be delayed.
 *
 * SWITCH ARMS ARE LAID OUT IN SOURCE ORDER while the compare tree is built from
 * the case VALUES, so writing the arms in the ROM's memory order is worth 48
 * aligned down to 6. And `case 0:` written alongside `default:` on a shared body
 * is NOT redundant: it puts a compare-and-branch node in the tree that a bare
 * `default:` omits.
 *
 * Nine flag groups swept; every one is byte-inert or worse, so no Makefile rule.
 * The two pooled constants are genuine -- neither is buildable with mov+lsl --
 * so no symbol question arises.
 */
typedef struct Ent {
    short f0;
    short f2;
    short f4;
    short f6;
    short f8;
    short fa;
    short fc;
    short fe;
} Ent;

extern int *iwram_3001f00;

extern short *_GetUnit(int id);
extern int Func_80b8f08(Ent *p);
extern void _Func_80198dc(void);
extern void _Func_80175a0(int a);
extern int Func_80b8824(Ent *p);
extern void WaitFrames(int n);
extern void Func_80b8888(Ent *p);
extern void Func_80b8c1c(Ent *p);
extern void Func_80b88d0(Ent *p);
extern void _Func_8016758(void);

int Func_80b874c(Ent *p)
{
    int *q;
    int *r;
    int v;
    int w;
    int n;
    int c;

    if (_GetUnit(p->f0)[0x1c] == 0)
        return -1;
    w = Func_80b8f08(p);
    q = iwram_3001f00;
    n = p->f0;
    p->fa = w;
    v = -0x2000;
    if (n <= 4)
        v = 0x2000;
    c = 0x3c;
    q[0] = v;
    q[1] = c;
    _Func_80198dc();
    switch (p->f6) {
    case 0x63:
        _Func_80175a0(0x843);
        if (Func_80b8824(p) != 0)
            return 1;
        break;
    case 3:
        WaitFrames(0x2d);
        Func_80b8888(p);
        break;
    case 2:
        WaitFrames(0x2d);
        Func_80b8c1c(p);
        break;
    case 0:
    default:
        r = iwram_3001f00;
        r[5] = 0;
        Func_80b8c1c(p);
        r[5] = 0;
        break;
    case 1:
        Func_80b88d0(p);
        break;
    }
    _Func_8016758();
    return 0;
}
