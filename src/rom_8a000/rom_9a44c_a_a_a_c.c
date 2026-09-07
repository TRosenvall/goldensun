/* Func_809a484 -- MATCHES on the default flags.
 * ref: asm/rom_8a000/rom_9a44c_a_a_a_c.s
 * objcmp: OK -- 472 bytes, 223 encodings and 12 relocations identical.
 *
 * A projectile launcher: spawns an actor from a launch descriptor and applies
 * the optional fields the caller's flag word selects, one `if` per bit.
 * One of three byte-identical copies -- see src/overlays/rom_7f2f14/ovl_30_a_a_a_c_a_c_c.c
 * and src/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c_c_c.c.  They differ only in symbol names.
 *
 * THREE LEVERS, in the order they were found.
 *
 * 1. TWO `_CreateActor` CALL SITES, not one with a chosen id.  The ROM's
 *    `mov r2, r6` is DUPLICATED in both arms of the id choice, which a single
 *    call site cannot produce.  Cross-jumping merged the common suffix
 *    (`mov r1,r5 / mov r3,r8 / bl / mov r6,r0`) and stopped at `mov r2,r6`
 *    because the two arms set r0 and r2 in opposite orders.  Writing one call
 *    with `id = cond ? p->f18 : 0xde` is 59 differing; the two call sites are
 *    14.  It also gives x/y/z a second reference each, which is what lifts
 *    them past `flags` in global-alloc priority and puts z/part in r8 and
 *    flags in r10 -- the ROM's assignment.  The single-call form has the whole
 *    r8/r10 pair swapped and spills the table index instead of arg 4.
 *
 * 2. A NAMED BYTE TEMPORARY for the second `sel` write.  `s->sel = p->f00 & 3`
 *    computes the mask in the register holding the shared constant 3 and
 *    shifts into a third register; `t = ...; s->sel = t & 3;` with `t` a
 *    NARROW type (unsigned char or unsigned short -- int/short measure 32)
 *    gives the ROM's `and r2, r5 / lsl r2, #2`.  14 -> 8.
 *
 * 3. THE TEMPORARY IS READ THROUGH A `char *`.  Last difference was one insn:
 *    the ROM puts `mov r3, r8` before the `p->f00` load, we put it after.
 *    sched2's class test decides it (docs/elevation.md, "Two byte stores that
 *    may alias cost a scheduling slot"): the preceding `n->f23 &= ~1` STORE
 *    makes an alias-set-0 load class 2 (anti-dependence) and it loses the slot
 *    to the independent class-3 `mov`.  Read as `p->f00` the load carries the
 *    struct tag's own alias set, goes class 3 and wins.  `*(unsigned char *)p`
 *    is alias set 0 and demotes it.  8 -> exact.
 *
 * Also measured and INERT: ternary vs if/else for the id; `int i = flags & 0xf`
 * named; signed `flags`; local declaration order (all positions); `unsigned
 * short` f64.  WORSE: `short *hp = &n->f64` (45, and 68 if assigned late);
 * writing either `sel` insert as an explicit read-modify-write on a plain byte
 * (37-73); reaching the second `sel` through `n->part` (12); a union around
 * f09 or f23 (20/22); swapping the two statements of the 0x20000 block (26).
 *
 * STANDING HAZARD -- MEASURED, NOT ASSUMED.  `-fno-strict-aliasing` is 2
 * differing (`mov r1, r9` and `str r3, [r6, #0x4c]` swap), so STRICT ALIASING
 * IS LOAD-BEARING HERE and this TU must never fall under an ALIAS_CFLAGS rule,
 * including by directory wildcard.  Lever 3 wants ONE load in alias set 0, not
 * everything: the flat set 0 that the flag imposes costs the pair above.  Same
 * direction as "A `char` LVALUE PINS A BYTE STORE TOO EARLY".
 */

struct Part {
    unsigned char pad_00[9];
    unsigned char b0 : 2, sel : 2, b4 : 4;   /* 0x09 */
    unsigned char pad_0a[0x14];
    unsigned short f1e;                      /* 0x1e */
    unsigned char pad_20[6];
    unsigned char f26;                       /* 0x26 */
};

struct Actor {
    unsigned char pad_00[0x18];
    int f18;                    /* 0x18 */
    int f1c;                    /* 0x1c */
    unsigned char pad_20[3];
    unsigned char f23;          /* 0x23 */
    unsigned char pad_24[0xc];
    int f30;                    /* 0x30 */
    int f34;                    /* 0x34 */
    unsigned char pad_38[0xc];
    int f44;                    /* 0x44 */
    int f48;                    /* 0x48 */
    int f4c;                    /* 0x4c */
    struct Part *part;          /* 0x50 */
    unsigned char pad_54[1];
    unsigned char f55;          /* 0x55 */
    unsigned char pad_56[0xe];
    short f64;                  /* 0x64 */
    unsigned char pad_66[6];
    void (*f6c)(void);          /* 0x6c */
};

struct Launch {
    unsigned char f00;          /* 0x00 */
    unsigned char pad_01[3];
    int f04;                    /* 0x04 */
    int f08;                    /* 0x08 */
    int f0c;                    /* 0x0c */
    int f10;                    /* 0x10 */
    int f14;                    /* 0x14 */
    short f18;                  /* 0x18 */
    unsigned char pad_1a[2];
    unsigned char *f1c;         /* 0x1c */
    unsigned short f20;         /* 0x20 */
    unsigned short f22;         /* 0x22 */
    void (*f24)(void);          /* 0x24 */
};

struct Script {
    unsigned char pad_00[0xc];
    int f0c;                    /* 0x0c */
};

extern struct Actor *MapActor_GetActor(int slot);
extern struct Actor *_CreateActor(int id, int x, int y, int z);
extern void _Actor_SetAnim(struct Actor *a, int n);
extern void _Actor_SetScript(struct Actor *a, unsigned char *s);
extern void Func_80929d8(struct Actor *a, int n);
extern void Func_809a44c(void);
extern struct Script *La012c[] __asm__(".La012c");

void Func_809a484(int x, int y, int z, int a, int b, int c,
                 unsigned int flags, struct Launch *p)
{
    struct Actor *caster;
    struct Actor *n;
    struct Part *s;
    struct Script *e;
    unsigned char t;

    caster = MapActor_GetActor(0);
    if ((flags & 0x100000) && p != 0)
        n = _CreateActor(p->f18, x, y, z);
    else
        n = _CreateActor(0xde, x, y, z);
    if (n != 0) {
        s = n->part;
        _Actor_SetAnim(n, (flags + 1) & 0xf);
        _Actor_SetScript(n, (unsigned char *)La012c[flags & 0xf]);
        n->f55 = 0;
        s->f26 = 0;
        n->f6c = Func_809a44c;
        n->f44 = a;
        n->f48 = b;
        n->f4c = c;
        s->sel = caster->part->sel;
        n->f30 = 0;
        n->f34 = 0;
        n->f64 = 0;
        if ((flags & 0xffff0000) && p != 0) {
            if (flags & 0x10000)
                Func_80929d8(n, p->f04);
            if (flags & 0x20000) {
                n->f23 &= ~1;
                t = *(unsigned char *)p;
                s->sel = t & 3;
            }
            if (flags & 0x80000) {
                n->f18 = p->f08;
                n->f1c = p->f0c;
            }
            if (flags & 0x40000) {
                e = La012c[flags & 0xf];
                if (flags & 0x80000) {
                    n->f30 = (p->f10 - n->f18) / e->f0c;
                    n->f34 = (p->f14 - n->f1c) / e->f0c;
                } else {
                    n->f30 = (p->f10 - 0x10000) / e->f0c;
                    n->f34 = (p->f14 - 0x10000) / e->f0c;
                }
            }
            if (flags & 0x200000) {
                _Actor_SetAnim(n, 1);
                _Actor_SetScript(n, p->f1c);
            }
            if (flags & 0x400000)
                s->f1e = p->f20;
            if (flags & 0x800000)
                n->f64 = p->f22;
            if (flags & 0x1000000)
                n->f6c = p->f24;
        }
    }
}
