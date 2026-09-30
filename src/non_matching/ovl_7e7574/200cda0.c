/* OvlFunc_959_200cda0 -- NON-MATCHING, 19 of 184 encodings differ.
 * Unattempted before batch 298.  Reference
 * asm/overlays/rom_7e7574/ovl_9dc_c_c_c_a_a_c_a.s (3 functions, this is the 1st).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7e7574/200cda0.c \
 *       asm/overlays/rom_7e7574/ovl_9dc_c_c_c_a_a_c_a.s --func OvlFunc_959_200cda0
 *
 * 19 IS A TRUE DISTANCE: size exact (448 bytes) and instruction count exact
 * (184 == 184).  SHIMS: 15 register pins (5x PIN3) plus 3 "+r" barriers, so a
 * landing needs a fakematch.txt row.  datacheck is silent; the split needs no
 * export.
 *
 * The only relocation asymmetry is our four R_ARM_ABS32 against _AREA_a0..a3, and
 * that is the benign relocation-FORM class: area.sym ids are absolutes and link to
 * the same pool words the hand-written .s writes as literals.  The symbol tell is
 * real -- the ROM POOLS 0xa0..0xa3 against a `cmp r2, #160`, and gcc emits
 * `cmp r3, #160` for the plain literal (verified).
 *
 * WHAT CLOSED IT.  The five-call __MapActor_SetPos / __Func_8092adc block, with its
 * repeated 0xa0<<7 and 0xe4<<17, closed completely under PIN3 blocks.  The
 * twice-used 0xe0<<1 offset needed a "+r" barrier on the FIRST offset so cse cannot
 * feed the iwram site from it, plus a second pair of barriers in the iwram block so
 * the address is materialised (`add r3,r1 / str r2,[r3]`) rather than 0x204 being
 * derived as `add r2,#68`.
 *
 * BLOCKER, named by the pass: reload1.c's allocate_reload_reg round robin.  From
 * .18.greg, `Using reg 3 / 3 / 2 / 2`, so used_spill_regs == {r2, r3}; the ROM's two
 * `ldrsh` scratches are r3 then r1, so ITS spill set contained r1.  This is the
 * class the sibling src/overlays/rom_7b0400/ovl_314_c_c_c_a_a_b.c documents, where
 * the cure is to add a reload ELSEWHERE rather than touch the differing site.  Six
 * spellings measured around it and 19 is the floor; the neighbours are 20 and 24.
 * The other two spots are the head (`add r5,r3,r7` against `add r5,r7,r1`) and the
 * iwram store's offset register.
 *
 * FLAGS DO NOT REACH THE OFFSET CSE, all measured: -fno-gcse 164;
 * -fno-cse-follow-jumps / -fno-expensive-optimizations / -fno-thread-jumps 171;
 * -fno-rerun-cse-after-loop 174, and it converts the ROM's first ldrsh into ldrh
 * plus a sign-extend.  -fno-cse-skip-blocks IS the only flag that restores the
 * ROM's repeated `mov #224` rebuild, but it drops r7 from the push mask, so it is
 * unusable here.  All figures flag-conditional and diagnostic only.
  *
 * BODY REPLACED IN BATCH 305e: 15 pins -> 9 at the IDENTICAL 19 of 184.  The pins buy
 * exactly ONE cluster (the five-call argument setup); clusters H, I and R are identical
 * line-for-line with and without them.  PIN-FREE WAS MEASURED AND COSTS 12: named locals
 * reproduce every instruction, register and constant in all five calls and still read 31,
 * the residue being 12 encodings of `mov r0` POSITION -- the pins work precisely BECAUSE
 * they block constant propagation, and any pin-free spelling lets gcc fold so expand_call
 * fixes the order.  Statement order inert (31 in three placements), -fno-schedule-insns
 * inert, -fno-schedule-insns2 worse at 49.
 *
 * THE 3 BARRIERS ARE IRREPLACEABLE, a firm negative: SIX spellings of the twice-used
 * `0xe0 << 1` -- reuse `off`, a fresh local, the `<<=` two-statement form, a short-index
 * `&((short *)iwram)[0xe0]`, plain inline, and a pointer local -- ALL SIX come out 2
 * instructions SHORT (182 of 184).  gcc commons the value 448 however it is spelled, so no
 * source form rebuilds it; each barrier is individually load-bearing at 2 instructions.
 * This park cannot land pin-free by any known route.
 *
 * CORRECTION to this header's own reading: its -fno-* figures of 146-174 are INFLATED BY A
 * POOL-OFFSET CASCADE -- the first diff sits at index 1 (`ldr r5,[pc,#0x198]` against
 * `#0x194`), so 4 bytes of missing text shifts every pc-relative load after it.  The real
 * residue under those flags is 2-4 instructions, not 148 differences.
*/

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _AREA_a0;
extern int _AREA_a1;
extern int _AREA_a2;
extern int _AREA_a3;
extern unsigned int L5fa4 __asm__(".L5fa4");
extern unsigned int __Random(void);
extern void __Func_80108c4(int a);
extern void OvlFunc_959_200cf60(void);
extern void OvlFunc_959_200d0e4(void);
extern void OvlFunc_959_200d324(void);
extern void OvlFunc_959_200d520(void);
extern void OvlFunc_959_200b054(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_SetPos(int a, int x, int y);
extern int __GetFlag(int id);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

int OvlFunc_959_200cda0(void)
{
    unsigned int base;
    unsigned int off;
    short *p;
    unsigned char *a;

    L5fa4 = __Random() * 7 >> 16;
    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    __asm__ ("" : "+r" (off));
    p = (short *)(base + off);
    if (*p == (int)(&_AREA_a0)) {
        __Func_80108c4(0xe0 << 4);
        OvlFunc_959_200cf60();
    }
    if (*p == (int)(&_AREA_a1))
        OvlFunc_959_200d0e4();
    if (*p == (int)(&_AREA_a2))
        OvlFunc_959_200d324();
    if (*p == (int)(&_AREA_a3)) {
        { unsigned char *q; unsigned int o2;
          o2 = 0xe0; o2 <<= 1;
          __asm__ ("" : "+r" (o2));
          q = iwram_3001ebc + o2;
          __asm__ ("" : "+r" (q));
          *(int *)q = 0x81 << 2; }
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
        __Func_8092adc(0xc, 0, 0);
        __MapActor_SetAnim(0xc, 0);
        OvlFunc_959_200d520();
        a = __MapActor_GetActor(8);
        if (a != 0)
            __Actor_SetSpriteFlags(a, 0);
        a[0x23] = 2;
        a = __MapActor_GetActor(9);
        if (a != 0)
            __Actor_SetSpriteFlags(a, 0);
        a[0x23] = 2;
        a = __MapActor_GetActor(0xa);
        if (a != 0)
            __Actor_SetSpriteFlags(a, 0);
        a[0x23] = 2;
        __Func_80108c4(0xe0 << 4);
        off = 0xe1;
        off <<= 1;
        p = (short *)(base + off);
        if (*p == 4) {
            __Func_80108c4(0xc0 << 4);
            OvlFunc_959_200b054();
        }
        if (*p == 3) {
            __Func_80108c4(0xc0 << 4);
            if (__GetFlag(0x941) != 0) {
                __MapActor_SetPos(0xc, 0, 0);
                { PIN2; q0 = 0x10; q1 = 0xd8 << 17;
                  __MapActor_SetPos(q0, q1, 0xac << 17); }
                { PIN2; q0 = 0x10; q1 = 0xa0 << 7;
                  __Func_8092adc(q0, q1, 0); }
                { PIN2; q0 = 0xd; q1 = 0xe4 << 17;
                  __MapActor_SetPos(q0, q1, 0x90 << 17); }
                { PIN1; q0 = 0xd;
                  __Func_8092adc(q0, 0xa0 << 7, 0); }
                { PIN2; q0 = 0x11; q1 = 0xe4 << 17;
                  __MapActor_SetPos(q0, q1, 0xa0 << 17); }
                __Actor_SetSpriteFlags(__MapActor_GetActor(0x11), 0);
            }
        }
        a = __MapActor_GetActor(0xf);
        if (a != 0)
            __Actor_SetSpriteFlags(a, 0);
        a[0x23] = 2;
        *(int *)(a + 0x18) = 0xcccc;
    }
    return 0;
}
