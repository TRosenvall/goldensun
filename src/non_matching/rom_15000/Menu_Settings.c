/* Menu_Settings (0x0801d4cc) -- NON-MATCHING, 444 of 500 encodings differ.
 * 465 ROM instructions.  INSTRUCTION COUNT MATCHES EXACTLY: 500 against 500.
 * Size 1148 against the ROM's 1152 (-4), and that -4 is ONE POOL WORD, accounted
 * for below.  aligncmp: 253 of 500 aligned-equal (50.6%) in 69 hunks.
 * Frontier target: UNATTEMPTED before this batch.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_15000/Menu_Settings.c \
 *       asm/rom_15000/rom_1ca1c_c_c_a_a_a.s --func Menu_Settings
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/aligncmp.py \
 *       src/non_matching/rom_15000/Menu_Settings.c \
 *       asm/rom_15000/rom_1ca1c_c_c_a_a_a.s Menu_Settings -v
 *
 * 444 IS NOT A DISTANCE.  Size and count both effectively match, so the ranking
 * figure here is aligncmp's aligned-equal, and the navigation figure is the
 * RELOCATION SEQUENCE: all 52 relocations appear in the ROM's order with the
 * ROM's symbols, and relocations 0-6 are BYTE-EXACT -- Func_801d014 (0x18),
 * CreateUIBox (0x2c), Func_801d108 (0x32), Func_8021620 (0x42), WaitFrames
 * (0x4a) and both __modsi3 (0x6c, 0x84).  The whole entry sequence lands.
 *
 * ================================================================
 * THE REFERENCE PROSE IS WRONG THREE TIMES -- do not trust it
 * ================================================================
 * The `@ RunDialScreen` comment claims the function "reads iwram_1c94 and
 * iwram_1b04 for input".  It does not: it reads gKeyPress and gKeyRepeat, and
 * neither iwram symbol appears in the function.  It names "Func_1d014 /
 * .gcc2_compiled." as two things re-initialising the screen; `.gcc2_compiled.`
 * is an assembler artefact, not a function.  And "497 lines" is a .s LINE count
 * offered where an instruction count belongs -- the function is 465 instructions.
 * That is the seventh, eighth and ninth wrong prose claim in the running tally.
 * What the prose gets right: SetUIColor, Func_164d4, CloseUIBox, iwram_1ca0 and
 * iwram_1d08 are all really there.
 *
 * ================================================================
 * WHAT CLOSED, BY PASS
 * ================================================================
 * PASS 1 (settings_v1): +4 instructions (504 against 500), +4 bytes, 221 of 500
 *   aligned-equal in 75 hunks.  Call sequence already in the ROM's order.
 * PASS 2 (v2): replacing `*(signed char *)(p + N)` with `(signed char)p[N]` in
 *   the three AddOption loops was INERT -- identical size, count and encodings.
 *   Recorded because an inert spelling is untested, not disproved; it is now
 *   tested.  gcc canonicalises the two to the same RTL here.
 * PASS 3 (v3): the REGISTER-OFFSET OPERAND ORDER.  The ROM loads the three
 *   option-handle arrays as `ldr r0, [r6, r7]` -- INDEX FIRST -- and we emitted
 *   `ldr r0, [r7, r6]`.  Writing `*(void **)(off + (int)p)` instead of
 *   `*(void **)(p + off)` flips it, exactly as docs/elevation.md's measured rule
 *   says ("off + (int)file gives index-first").  Together with a `p + 1` base for
 *   the 0x599 read this took 504 -> 500 instructions -- COUNT EXACT -- and
 *   221 -> 242 aligned-equal, 75 -> 63 hunks.  It also fixed the prologue: the
 *   first differing encoding moved from index 7 to index 13.
 * PASS 4 (v4, this body): naming the two subscript offsets `o1 = 0x594 + cur`
 *   and `o2 = 0x598 + cur` so gcc cannot reassociate `p + (0x594 + cur)` into
 *   `(p + cur) + 0x594`.  242 -> 253 aligned-equal.
 *
 * ================================================================
 * WHAT IS LEFT, AND WHY -- three residues, none a spelling
 * ================================================================
 * 1. POOL DUMP POSITION, and it is most of the remaining hunk count.  The ROM
 *    dumps a literal pool INSIDE the function at ~0xc8 (the `.L1d594 / .pool`
 *    block, only ~200 bytes in); we carry ours to 0x2dc.  Every `ldr rN,[pc,#K]`
 *    after that point therefore differs in its displacement while being the same
 *    instruction, which is exactly the case objcmp exists to expose and exactly
 *    why 444 overstates the distance.  Nothing in the source reaches this.
 * 2. THE POOLED ZERO IS THE ENTIRE -4 BYTES.  At the `if (iwram_3001ca0)` guard
 *    the ROM loads its stored 0 from the pool (`ldr r2, .L1d594  @ 0`) where
 *    gcc-2.96 emits `mov r2, #0`.  One 4-byte pool word, one instruction each
 *    way, and the size delta closes to zero the moment it is explained.  Do not
 *    hunt for a missing instruction: there is none.
 * 3. A HIGH-REGISTER ROTATION.  The ROM holds `running` in r11 and `cur` in r9;
 *    we hold `cur` in r11.  In PASS 1 `running` was SPILLED TO THE STACK and the
 *    fix for that came free with PASS 3, which is the useful part: the spill was
 *    a consequence of the addressing form, not of the variable.  The remaining
 *    r9/r11 swap is an allocation-input difference, not REG_ALLOC_ORDER.
 *    SPILL SLOTS ARE ALREADY CORRECT and follow declaration order: sp+0x10 box,
 *    sp+0xc t, sp+8 pa, sp+4 pb, sp+0 the CreateUIBox fifth argument.
 *
 * ================================================================
 * TWO LEVERS THAT APPLY HERE AND ARE WORTH MORE THAN THE COUNT
 * ================================================================
 * THE gState EXIT CHAIN IS A MUTATED OFFSET VARIABLE, NOT A NAMED POINTER.  The
 * ROM writes the four settings back with `ldr r0,=0x205 / add r3,r2,r0 / strb`,
 * then `add r0,#1`, `add r0,#6`, `sub r0,#2` -- it DERIVES each address from one
 * held offset and re-adds gState every time.  A named `u8 *g = gState + 0x205`
 * would give immediate offsets off one base and cannot produce this; an offset
 * variable mutated between the stores does, which is the documented form ("that
 * pattern needs both, an offset variable to mutate and a pointer to store
 * through").  The body below writes it that way.
 *
 * THE u16 COMPARE IS SHIFTED, NOT DIRECT.  The wrap test on the palette index at
 * p+0x57e is `lsl r3,#16 / cmp r3, 0xa0<<11`, i.e. `(v << 16) > (5 << 16)`.
 * 0xa0 << 11 is 0x50000 -- the shifted-constant build of 5<<16, not a separate
 * magic number.  Written as `*d > 5` on a u16.
 *
 * SIGNEDNESS IS READ OFF THE ADDRESSING MODE.  Thumb has no `ldrsb` with an
 * immediate, so every signed byte read here is `mov rN,#0 / ldrsb rD,[rB,rN]`.
 * The `mov #0` is NOT a residue and not a missing quantity -- it is the ISA.
 *
 * SPLIT: anchored grep gives THREE functions in asm/rom_15000/rom_1ca1c_c_c_a_a_a.s
 * -- Func_801d108, Menu_Settings, Func_801d94c -- so a three-way text split.
 * datacheck.py is SILENT: no data section and no exports to add.  The five `.L`
 * tables the function reads (.L367c9, .L367cc, .L367ce, .L367d0, .L367d6) live
 * OUTSIDE this file and are reached with `__asm__(".L367c9")` declarations.
 * ALL FIVE ARE ALREADY `.global`, in asm/rom_15000/rom_1ca1c_c_c_c.s lines
 * 567-571, so the split needs NO export work -- verified, not assumed.
 */
#include "gba/types.h"

extern unsigned char iwram_3001ea0[];
extern u8 iwram_3001ca0;
extern u8 iwram_3001d08;
extern unsigned char gState[];
extern u32 gKeyPress;
extern u32 gKeyRepeat;

extern signed char L367c9[] __asm__(".L367c9");
extern signed char L367cc[] __asm__(".L367cc");
extern signed char L367ce[] __asm__(".L367ce");
extern unsigned char L367d0[] __asm__(".L367d0");
extern unsigned char L367d6[] __asm__(".L367d6");

extern void Func_801d014(void);
extern void Func_801d0f0(void);
extern void *Func_801d108(void);
extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void CloseUIBox(void *box, int b);
extern void *Func_8021620(int a, void *h, int c, int d);
extern void Func_80216b4(void *t);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void _Func_80a17c4(void *o);
extern void StartMenu_AddOption(int a, int b, int c);
extern void _Func_80b09fc(void *a, int b, int c, int d);
extern void Func_80164d4(void *h, int b, int c, int d, int e);
extern void Func_80164ac(void *box);
extern void Func_801e7c0(int id, void *h, int c, int d);
extern void SetUIColor(int a, int b);
extern void DrawSmallText(int id, void *box, int c, int d);

struct Sel { u16 f0c; u16 f0e; };

int Menu_Settings(void)
{
    unsigned char *p;
    int running;
    int cur;
    void *box;
    struct Sel *h;
    void *t;
    signed char *pa;
    signed char *pb;
    int ret;
    int i;
    s32 off;
    s32 o1;
    s32 o2;
    s32 v;
    void *o;
    signed char *sel3;

    running = 1;
    cur = 0;
    Func_801d014();
    p = *(unsigned char **)iwram_3001ea0;
    box = CreateUIBox(1, 2, 0x1c, 3, 2);
    h = (struct Sel *)Func_801d108();
    t = Func_8021620(7, h, 0x40, -0x30);
    WaitFrames(1);
    pa = (signed char *)(p + 0x594);
    pb = (signed char *)(p + 0x595);
    for (;;) {
        if (running) {
            running = 0;
            cur = (cur + 5) % 5;
            o2 = 0x598 + cur;
            o1 = 0x594 + cur;
            v = *(signed char *)((p + 1) + o2);
            *(signed char *)(p + o1) = (*(signed char *)(p + o1) + v) % v;
            *(u16 *)(p + 0x574) = cur;
            if (iwram_3001ca0)
                p[0x598] = 0;

            off = 0x5ec;
            for (i = 0; i <= 2; i++) {
                o = *(void **)(off + (int)p);
                *((u8 *)o + 0xf) = 0xfb;
                _Func_80a17c4(o);
                o = *(void **)(off + (int)p);
                StartMenu_AddOption(L367c9[i], *((u8 *)o + 0xe),
                                    i != *(signed char *)(p + 0x596));
                off += 4;
            }
            off = 0x5f8;
            for (i = 0; i <= 1; i++) {
                o = *(void **)(off + (int)p);
                *((u8 *)o + 0xf) = 0xfb;
                _Func_80a17c4(o);
                o = *(void **)(off + (int)p);
                StartMenu_AddOption(L367cc[i], *((u8 *)o + 0xe),
                                    i != *(signed char *)(p + 0x597));
                off += 4;
            }
            off = 0x604;
            sel3 = (signed char *)(p + 0x598);
            for (i = 0; i <= 1; i++) {
                o = *(void **)(off + (int)p);
                *((u8 *)o + 0xf) = 0xfb;
                _Func_80a17c4(o);
                o = *(void **)(off + (int)p);
                StartMenu_AddOption(L367ce[i], *((u8 *)o + 0xe), i != *sel3);
                off += 4;
            }

            v = h->f0c * 8 + 0x8c
                + *pa * 60 / *(signed char *)(p + 0x599);
            _Func_80b09fc(p + 0x5b4, v, h->f0e * 8 + 4, 1);
            v = h->f0c * 8 + 0x8c
                + *pb * 60 / *(signed char *)(p + 0x59a);
            _Func_80b09fc(p + 0x5c4, v, h->f0e * 8 + 0x14, 1);

            v = 0xc0a + *(signed char *)(p + 0x596);
            Func_80164d4(h, 0xa0, 0x28, 0xc8, 0x30);
            Func_801e7c0(v, h, 0xa0, 0x28);
            v = 0xc10 + *(signed char *)(p + 0x597);
            Func_80164d4(h, 0xa0, 0x40, 0xb8, 0x48);
            Func_801e7c0(v, h, 0xa0, 0x40);
            v = 0xc13 + *sel3;
            Func_80164d4(h, 0xa0, 0x58, 0xb8, 0x60);
            Func_801e7c0(v, h, 0xa0, 0x58);
            SetUIColor(*pa, *pb);
            v = (cur * 3 + h->f0e) * 8 + 4;
            if (cur == 0)
                v += 8;
            _Func_80b09fc(p + 0x5a4, h->f0c * 8, v, 3);
            Func_80164ac(box);
            DrawSmallText(0xc15 + cur, box, 0, 0);
        }
        Func_80216b4(t);
        WaitFrames(1);
        if (gKeyPress & 4) {
            u16 *d;
            _PlaySound(0x70);
            d = (u16 *)(p + 0x57e);
            running = 1;
            *d = *d + 1;
            if (*d > 5)
                *d = 0;
            *pa = L367d0[*d];
            *pb = L367d6[*d];
            continue;
        }
        if (gKeyPress & 9) {
            ret = 0;
            _PlaySound(0x70);
            break;
        }
        if (gKeyPress & 2) {
            ret = -1;
            _PlaySound(0x71);
            break;
        }
        if (gKeyRepeat & 0x40) {
            _PlaySound(0x6f);
            cur--;
            running = 1;
            continue;
        }
        if (gKeyRepeat & 0x80) {
            _PlaySound(0x6f);
            cur++;
            running = 1;
            continue;
        }
        if (gKeyRepeat & 0x20) {
            _PlaySound(0x6f);
            p[0x594 + cur]--;
            running = 1;
        }
        if (gKeyRepeat & 0x10) {
            _PlaySound(0x6f);
            p[0x594 + cur]++;
            running = 1;
        }
    }
    CloseUIBox(box, 2);
    CloseUIBox(h, 2);
    if (ret == 0) {
        off = 0x205;
        gState[off] = p[0x594];
        off++;
        gState[off] = p[0x595];
        off += 6;
        gState[off] = p[0x596];
        off -= 2;
        gState[off] = p[0x597];
        v = p[0x598];
        gState[0x22a] = v;
        iwram_3001d08 = v;
    } else {
        SetUIColor(gState[0x205], gState[0x206]);
    }
    Func_801d0f0();
    WaitFrames(1);
    return ret;
}
