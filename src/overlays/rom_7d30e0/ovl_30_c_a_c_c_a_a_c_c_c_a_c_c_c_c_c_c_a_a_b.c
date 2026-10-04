/* OvlFunc_948_2009308 -- MATCHING (0 of 59), PIN-FREE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_c_c_c_c_c_c_a_a_b.c \
 *     asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_c_c_c_c_c_c_a_a_b.s --whole
 *
 * SPLIT REQUIRED (two-way, as the park said). The .s carries OvlFunc_948_2009308
 * and OvlFunc_948_200938c; `tools/datacheck.py` is clean, and
 *
 *   python3 tools/split_s.py \
 *     asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_c_c_c_c_c_c_a_a.s \
 *     OvlFunc_948_2009308
 *
 * reports
 *
 *   would write ..._a_a_b.s  (1 function(s), 67 lines)
 *   would write ..._a_a_c.s  (1 function(s), 65 lines)
 *   would REMOVE ..._a_a.s
 *   would rewrite overlays/rom_7d30e0/overlay.ld
 *
 * PINS: 0.  FAKEMATCH: no.  DEVICES: none.
 *
 * ================= THE PIN WAS THE BLOCKER. ONE TOKEN. =================
 *
 * The park's own body matches with the pin DELETED:
 *
 *     register int p0 __asm__("r0");   ->   int p0;
 *
 * Nothing else changes. 4 of 59 -> 0 of 59, 132 bytes, 5 relocations identical.
 *
 * WHAT THE PARK CLAIMED, AND WHAT SURVIVED.
 *
 * The park carried two stacked diagnoses, and the batch-272 header on top of it
 * said the residue "IS DECLARATION ORDER" -- that `w`(34) and `tx`(35) tie in
 * `allocno_compare` and `allocno_compare` falls through to ascending pseudo
 * number, so moving `w` below the ints flips the tie. The batch-272 candidate
 * (scratch_elev/b272/D/t3pin1.c) applied that reordering AND KEPT THE PIN, and
 * was held back only because a pin needs a fakematch row.
 *
 *   - The park's OBSERVATION survived: the residue is exactly `tx` and the iwram
 *     base in each other's callee-saved registers (ROM `asr r6,r3,#20` /
 *     `ldr r7,[r3]`, ours swapped), four encodings, length exact.
 *   - The park's verdict "this is not something the source expresses" is WRONG.
 *   - The batch-272 verdict "it is declaration order" is ALSO WRONG, and I can
 *     separate the two because the depin works at BOTH declaration orders:
 *
 *         park order (p, g, w, tx, ty, v, q)  + depin   ->  0
 *         b272 order (p, g, tx, ty, v, w, q)  + depin   ->  0
 *         b272 order (p, g, tx, ty, v, w, q)  + pin     ->  0   (the b272 candidate)
 *         park order (p, g, w, tx, ty, v, q)  + pin     ->  4   (the park)
 *
 *     So the reordering and the depin are two INDEPENDENT cures for the same
 *     tie, and declaration order is INERT once the pin is gone. The park's own
 *     line "THE PIN IS ACTIVELY DESTRUCTIVE HERE and that is the useful part"
 *     was the right instinct aimed at the wrong pin: it tested pinning `tx` and
 *     `w`, concluded pins cannot relocate a COMPUTED value, and never retested
 *     the `p0` pin it was already carrying.
 *
 * WHY THE r0 PIN COSTS THE TIE. `p0` pinned to r0 is a hard register, and a hard
 * register is not an allocno. With it pinned, `p0`'s two-step fill
 * (`mov r0,#0x88 / lsl r0,#2`) never becomes a pseudo, so local-alloc's quantity
 * list is one shorter and the `w`/`tx` pair is considered at a different point in
 * the walk. Unpinned, `p0` is pseudo-numbered between them and the tie breaks the
 * ROM's way. This is the brief's "a pinned hard register is not an allocno" lever
 * running in the UNWANTED direction -- the pin made one side ineligible and that
 * is precisely what moved the pair wrong.
 *
 * WHAT IS STILL LOAD-BEARING, measured (all against --func OvlFunc_948_2009308,
 * ref 59 encodings / 132 bytes):
 *
 *   | form | figure |
 *   |---|---|
 *   | park, as parked (1 pin) | 4 of 59 |
 *   | **park with `int p0;` (shipped)** | **0** |
 *   | b272 decl order + `int p0;` | 0 |
 *   | `int p0;` but `__SetFlag(0x88 << 2)` inline | 65 insns, 144 bytes -- WORSE |
 *   | `int p0;` but `__GetFlag(0x88 << 2)` inline | 65 insns, 144 bytes -- WORSE |
 *   | `p0 = 0x88 << 2;` as one statement at both sites | 65 insns, 144 bytes -- WORSE |
 *   | a SECOND local `p1` for the `__SetFlag` site | 65 insns, 144 bytes -- WORSE |
 *   | `extern int __SetFlag(int)` (int-return lever) | 0 (exactly inert) |
 *
 * So ONE local, reused at BOTH call sites, with `= 0x88` and `<<= 2` as separate
 * statements, is the construct that produces the ROM's in-place `mov r0,#0x88 /
 * lsl r0,#2` at each site. That part of the park is right and must be kept; the
 * `register`/`__asm__` on it was never doing the work.
 *
 * THE TWIN. src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_c_c_c_c_c_c_a_b.c
 * (OvlFunc_948_200941c) is the same shape with five constants changed and is
 * already elevated; everything it established is kept here -- the two signed
 * divisions by 0x100000 (NOT shifts), the unsigned-offset idiom for the
 * three-wide `tx` range against two plain compares for the two-wide `ty` range,
 * the stored constant assigned to a local as its own statement, and one variable
 * serving the offset and then the value so the `mov r3,#0x5b` lands after the
 * `add r2, r7, r3`.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);

void OvlFunc_948_2009308(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned char *w;
    int tx, ty, v;
    short *q;
    int p0;

    p = __MapActor_GetActor(0);
    tx = *(int *)(p + 8) / 0x100000;
    ty = *(int *)(p + 0x10) / 0x100000;
    w = iwram_3001ebc;
    p0 = 0x88; p0 <<= 2;
    if (__GetFlag(p0) == 0) {
        g = gState;
        if (*(short *)(g + (0x93 << 2)) == 0
            && *(short *)(g + 0x24a) != 8
            && (unsigned)(tx - 0x15) <= 2
            && ty > 9 && ty <= 0xb) {
            p0 = 0x88; p0 <<= 2;
            __SetFlag(p0);
            v = 0xc1;
            v <<= 1;
            q = (short *)(w + v);
            v = 0x5b;
            *q = v;
        }
    }
}
