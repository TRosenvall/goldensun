/* OvlFunc_952_200c0b4 -- NON-MATCHING, 37 encodings of 270.  Size equal (652 bytes),
 * 270 = 270 encodings, 261 instructions against 260.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7d768c/200c0b4.c \
 *     asm/overlays/rom_7d768c/ovl_30_c_c.s
 * ONE function.  Path: 271 -> 235 -> 157 -> 124 -> 37.
 *
 * NEEDS A TEXT/DATA REHOME to land: datacheck.py reports .data, and the blob is
 * **5608 bytes across 15 .incbin blocks** -- the largest of batch 283's four, and far
 * past the emit-from-C allowance.  All 16 labels are already .global.
 *
 * `_AREA_8b` is used and already in area.sym.  AND THE NEGATIVE MATTERS AS MUCH: the
 * other gState compares in this function -- 0x5a, 0xb, 0x21, 0x63, 0x62 -- are PLAIN
 * LITERALS and read gState+0x1C2, A DIFFERENT HALFWORD, so they are NOT area ids.  The
 * halfword offset is what separates the two spaces; do not promote a constant to
 * _AREA_* on the compare shape alone.
 *
 * ================================================================
 * THREE RESIDUE WINDOWS, all measured
 * ================================================================
 *
 * 1. BLOCKER CLASS: SIGN/ZERO-EXTEND LOAD MERGE IN `combine`.  The ROM reads
 *    gState+0x1C2 as `ldrsh` (the == 0x5a test) AND `ldrh` (the == 0x5b test) -- three
 *    loads, two of them ldrh.  gcc emits the ldrsh and DERIVES the unsigned value from
 *    it, because the 0x5b compare is HImode (`lsl #16` against `0xb6 << 15`) and only 16
 *    bits matter.  Costs one instruction.  TRIED AND REFUTED: reading `h` first, reading
 *    `h` last, an `unsigned short *hp` alias pointer, and a distinct
 *    `struct HU { unsigned short f0; }` overlay -- the distinct alias set had NO EFFECT,
 *    because the merge is combine's VALUE SUBSTITUTION, not CSE's exp_equiv_p.  That
 *    distinction is the useful part: an alias-set change cannot reach a combine merge.
 * 2. The second loop's buffer address.  `b2` sits at sp+0, so `mov r0, sp` and
 *    `mov r7, sp` are both one instruction and gcc commons them (`mov r7,sp / mov r0,r7`,
 *    +1).  The first loop (sp+0x10) already matches after switching to the ROM's
 *    byte-offset form `*(short *)(bp + o)` with `bp = (char *)b1` assigned INSIDE the
 *    guard.
 * 3. `ldrsh r0, [r6, r7]` against `[r7, r6]` -- base and offset register roles swapped in
 *    the register-offset load, 4 encodings.  `o + bp`, `bp` as `unsigned int`,
 *    declaration reordering and `((short *)bp)[o >> 1]` were all tested; gcc canonicalises
 *    the `plus` the same way every time.
 *
 * LEVERS THAT LANDED HERE AND ARE WORTH REUSING:
 *   the DROP-THE-`off = 0`-SCAFFOLD rule (235 -> 157, because the shared zero was ALSO
 *   serving as the `n > 0` compare operand, giving `cmp r5, r0 / bge` instead of the
 *   ROM's `cmp r0,#0 / ble`) -- see src/overlays/rom_7d0e88/ovl_2580_c_c.c;
 *   an `int` CARRIER for the `*(short *)p = 8` store, since a bare 8 into a short lvalue
 *   made gcc load the constant with `ldrh rN, .Lxx` -- the HImode pool lever in its
 *   use-an-int-carrier direction;
 *   an `int mask = -0x21` CARRIER for `spr->f5 &= mask`, because a bare `~0x21` narrows to
 *   `mov r3,#0xdf` where the ROM wants `mov r3,#0x21 / neg r3,r3`;
 *   and PIN1 on `__GetFlag(0x96f)` / `__SetFlag(0x96f)` to stop the pooled constant being
 *   carried in r5 across the call.
 */
__asm__(".equ _AREA_8b, 0x8b");   /* park self-verification shim: _AREA_8b is
                                     already in area.sym, and objcmp assembles
                                     the candidate standalone so it cannot see a
                                     linker-script definition.  Without this the
                                     pool word reads as a relocation placeholder
                                     and the count is 38 rather than 37. */

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _AREA_8b;

struct Spr {
    unsigned char pad00[5];
    unsigned char f5;
    unsigned char pad06[9 - 6];
    unsigned char f9;
    unsigned char pad0a[0x1c - 0xa];
    unsigned char f1c;
    unsigned char pad1d[0x27 - 0x1d];
    unsigned char f27;
};

struct Actor {
    unsigned char pad00[0xc];
    int fc;
    unsigned char pad10[0x50 - 0x10];
    struct Spr *f50;
    unsigned char pad54[0x55 - 0x54];
    unsigned char f55;
    unsigned char pad56[0x59 - 0x56];
    unsigned char f59;
    unsigned char pad5a[0x5c - 0x5a];
    unsigned char f5c;
};

struct Unit {
    unsigned char pad00[0x34];
    unsigned short f34;
    unsigned short f36;
    unsigned short f38;
    unsigned short f3a;
};

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_80118c0(int ch);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern unsigned char *__galloc_iwram(int id, int size);
extern void __gfree(int id);
extern void __LoadItemIcon(int item);
extern void __UploadSpriteGFX(int a, int b, unsigned char *c);
extern void __Func_8092adc(int a, int b, int c);
extern int __Func_80796c4(short *buf);
extern struct Unit *__GetUnit(int id);
extern void __UpdateStatBarPercent(int id);
extern void __AddPartyMember(int id);
extern void __Func_807a7a0(void);
extern void OvlFunc_952_200a014(void);
extern void OvlFunc_952_20097e8(void);
extern void OvlFunc_952_2008674(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

int OvlFunc_952_200c0b4(void)
{
    short b1[8];
    short b2[8];
    struct Actor *a;
    struct Spr *spr;
    struct Unit *u;
    unsigned char *v;
    unsigned int g;
    unsigned int g2;
    unsigned int p;
    unsigned int p2;
    unsigned int p3;
    unsigned int q;
    unsigned int off;
    unsigned int off2;
    unsigned int off3;
    unsigned int off4;
    unsigned int p4;
    int eight;
    int mask;
    unsigned short h;
    unsigned short *hp;
    int ev;
    int t;
    int t2;
    int n;
    int i;
    int k;
    int o;
    char *bp;

    __Func_80118c0(1);
    __Func_80118c0(2);
    __Func_80118c0(4);
    g = (unsigned int)&gState;
    off = 0xe1;
    off <<= 1;
    p = g + off;
    h = *(unsigned short *)p;
    ev = *(short *)p;
    if (ev == 0x5a) {
        __SetFlag(0x962);
        h = *hp;
    }
    if (h == 0x5b) {
        __SetFlag(0x962);
        __SetFlag(0x95 << 4);
    }
    off = 0xe0;
    off <<= 1;
    q = g + off;
    if (*(short *)q != (int)(&_AREA_8b)) {
        if (*(short *)p == 0xb)
            __ClearFlag(0x12f);
        if (__GetFlag(0x95 << 4)) {
            t = __GetFlag(0xf31);
            if (t) {
                __MapActor_SetPos(0x10, 0, 0);
            } else {
                a = __MapActor_GetActor(0x10);
                a->f5c = 1;
                a->f55 = t;
                spr = a->f50;
                a->fc = 0x80 << 11;
                spr->f27 = t;
                mask = -0x21;
                spr->f5 &= mask;
                spr->f9 &= 0xf;
                v = __galloc_iwram(0x11, 0xc1 << 3);
                __LoadItemIcon(0xcd);
                v += 0x80 << 3;
                __UploadSpriteGFX(spr->f1c, 0x80, v);
                __gfree(0x11);
            }
            p2 = (unsigned int)&gState;
            off2 = 0xe1;
            off2 <<= 1;
            p2 += off2;
            if (*(short *)p2 == 0x21) {
              { PIN1; q0 = 0x96f; t2 = __GetFlag(q0); }
              if (!t2) {
                { PIN1; q0 = 0x96f; __SetFlag(q0); }
                { PIN3; q0 = 0xe; q1 = 0xd0 << 16; q2 = 0xb0 << 18;
                  __MapActor_SetPos(q0, q1, q2); }
                OvlFunc_952_200a014();
              }
            }
            __MapActor_SetAnim(0xe, 5);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
        } else if (__GetFlag(0x962) && !__GetFlag(0x966)) {
            { PIN3; q0 = 0xa; q1 = 0xf0 << 15; q2 = 0x90 << 15;
              __MapActor_SetPos(q0, q1, q2); }
        }
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
        __MapActor_GetActor(9)->f59 |= 4;
        g2 = (unsigned int)&gState;
        off3 = 0xe1;
        off3 <<= 1;
        p3 = g2 + off3;
        if (*(short *)p3 == 0x63) {
            n = __Func_80796c4(b1);
            if (n > 0) {
                bp = (char *)b1;
                o = 0;
                k = n;
                do {
                    u = __GetUnit(*(short *)(bp + o));
                    u->f38 = u->f34;
                    u->f3a = u->f36;
                    k--;
                    __UpdateStatBarPercent(*(short *)(bp + o));
                    o += 2;
                } while (k != 0);
            }
            __AddPartyMember(1);
            __AddPartyMember(2);
            __AddPartyMember(3);
            __Func_807a7a0();
            OvlFunc_952_20097e8();
            g2 = (unsigned int)&gState;
            off4 = 0xe1;
            off4 <<= 1;
            p4 = g2 + off4;
            eight = 8;
            *(short *)p4 = eight;
        }
        off3 = 0xe1;
        off3 <<= 1;
        p3 = g2 + off3;
        if (*(short *)p3 == 0x62) {
            n = __Func_80796c4(b2);
            if (n > 0) {
                bp = (char *)b2;
                o = 0;
                k = n;
                do {
                    u = __GetUnit(*(short *)(bp + o));
                    u->f38 = u->f34;
                    u->f3a = u->f36;
                    k--;
                    __UpdateStatBarPercent(*(short *)(bp + o));
                    o += 2;
                } while (k != 0);
            }
            __AddPartyMember(1);
            __AddPartyMember(2);
            __AddPartyMember(3);
            __Func_807a7a0();
            __SetFlag(0x966);
            __SetFlag(0x967);
            { PIN3; q0 = 0xa; q1 = 0xe0 << 14; q2 = 0xf0 << 15;
              __MapActor_SetPos(q0, q1, q2); }
            __Func_8092adc(0xa, 0xf0 << 8, 0);
            OvlFunc_952_2008674();
            g2 = (unsigned int)&gState;
            off4 = 0xe1;
            off4 <<= 1;
            p4 = g2 + off4;
            eight = 8;
            *(short *)p4 = eight;
        }
    }
    return 0;
}
