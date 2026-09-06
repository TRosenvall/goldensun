/* OvlFunc_924_200bc48  --  0x0200bc48
 * [asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_c_a.s, FIRST of two functions]
 *
 * 97 instructions -- 216 bytes, 100 encodings and 6 relocations identical.
 * Exact on the FIRST candidate, under the DEFAULT flag set: tryc's
 * makefile_flags() returns the empty set for this path, objcmp printed no
 * "(built with: ...)" line, and NO flag group is involved.
 *
 * A twelve-tick particle burst: on every odd tick it points a unit vector at a
 * caller-supplied angle, shrinks the radius one step per pair of ticks, jitters
 * the spawn point by two 6-unit randoms and hands a four-word parameter block
 * to the shared spawner. Two frames of wait per tick, odd ticks and even.
 *
 * THE TEMPLATE WAS THE ROM-ADJACENT FUNCTION, NOT THE SCORED NEIGHBOUR. The
 * supplied 0.80 neighbour (rom_7b2078/ovl_314_c_c_c_c_c_c_c_c_b.c) is a
 * cross-bank file whose `struct St` is a DIFFERENT block -- f0/f4/f18 with
 * 8..0x17 padded out -- and this ROM stores at +8 and +0xc, so its layout would
 * not have expressed the function at all. Grepping the callee set instead
 * (`__vec3_translate` AND `OvlFunc_common0_10c` in one asm file, then
 * `OvlFunc_common0_10c` across solved src/) landed
 * src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_b.c -- OvlFunc_924_200bbd4, the
 * function 116 bytes EARLIER in this same ROM region. It handed over `struct P`
 * verbatim, the eight-argument prototype and the fixed-point random idiom. That
 * is the recorded callee-set-identity heuristic (§PICK TARGETS BY TEMPLATE) and
 * costs nothing to re-run once the scored template's struct does not fit.
 *
 * THE BIAS PLACEMENT IS THE OPPOSITE OF THE ROM-ADJACENT SIBLING'S, and that is
 * the whole content of the "every lever has a placement" rule the sibling
 * itself recorded. 200bbd4 shifts first and subtracts the biased constant after
 * (`x += rand << 16; x -= 8 << 16;`). This ROM subtracts INSIDE the parenthesis
 * and shifts the difference: `(6 - (rand * 6 >> 16)) << 16`, one `sub` then one
 * `lsl`. Spelling it the sibling's way -- shift, then a second accumulate of
 * `6 << 16` -- is 48 differing of 100 and moves four of the six relocations:
 * the extra pooled `0x60000` reorders the whole body. Adjacency in ROM does NOT
 * carry the placement over; only the ROM's own instruction order says which.
 *
 * THE PARAMETER BLOCK'S FIRST FIELD IS f4, NOT f0, AND ITS STORE MUST COME
 * FIRST. `st.f4 = 7;` then the `if (i & 2)` override, then the two 0x9999
 * stores, is exact. Hoisting the two 0x9999 stores above the f4 pair -- the
 * ascending-offset spelling -- is 13 differing: gcc sinks `add r6, sp, #0x24`
 * to a different point and rebuilds the pool load inside the arm. f0 is never
 * written here even though the sibling writes it; an uninitialised leading
 * field is correct and costs nothing.
 *
 * `unsigned int i` IS LOAD-BEARING FOR EXACTLY ONE ENCODING, and it is the last
 * one you would find by reading the body. A signed counter is 1 differing at
 * index 87: `ble` where the ROM has `bls`. The `lsr` for `i >> 1` is not the
 * tell -- an explicit `(unsigned)i >> 1` reproduces that -- the loop's own exit
 * test is. Check the terminating branch's condition code before the shifts.
 *
 * The frame is a jigsaw and both pieces are measured. sp = 0x4c decomposes as
 * 0x10 of outgoing stack arguments (the call takes eight), 0x10/0x14 for the
 * two spilled parameters, 0x18 for `int v[3]` and 0x24 for a 0x28-byte
 * `struct P`. Declaring `st` BEFORE `v` is what puts v at 0x18; the reverse
 * order is 2 differing (`add r7, sp, #0x64`). Padding the struct to 0x2c is 2
 * differing (`sub sp, #0x50`). Neither is guessable -- both fell out of the
 * arithmetic sp - 0x24 - 0x18 = 0x28 and were then confirmed by measurement.
 *
 * FOUR OF THE NEIGHBOURS' LEVERS RE-MEASURED AS INERT HERE, which is the
 * sufficient-not-necessary rule paying for itself in the other direction: none
 * of them had to ship. (i) The sibling's "accumulate into the parameter, do not
 * rebuild it" -- `px = x; px += ...` is byte-identical to `px = x + ...`,
 * because x is a spilled parameter read once and gcc folds both to the same
 * three-operand add off the reload. (ii) The scored neighbour's named `zero`
 * for the vector fill -- identical to three literal 0s. (iii) `0x90 << 12`
 * versus a bare `0x90000` -- identical. (iv) `for` versus an explicit
 * `i = 0; do { } while (i <= 0xb);` -- identical, gcc's loop rotation reaches
 * the ROM's shape from either. Shipping any of them would have been inert
 * scaffolding.
 *
 * The repeated constant 6 -- multiplier at three sites, subtrahend at three --
 * needed NO pin. It lands in r10 across the whole loop on its own, which is
 * §"When gcc HOISTS a repeated constant, exactly: dominance" behaving as
 * written: the `(6 - (i >> 1))` use dominates both random sites inside the
 * `if`, so PRE hoists once and pays the callee-saved register. Nothing new.
 *
 * r4 is absent from the push set because `-fcall-used-r4` is in GCC296_CFLAGS;
 * the allocator fills r5..r11 ascending and uniformly, and that is correct
 * here rather than merely cheap.
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

extern void __PlaySound(int id);
extern void __vec3_translate(int dist, int ang, int *v);
extern unsigned int __Random(void);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);
extern void __WaitFrames(int n);

void OvlFunc_924_200bc48(int x, int y, int z, int ang)
{
    struct P st;
    int v[3];
    unsigned int i;
    int px, pz;

    __PlaySound(0xd8);
    for (i = 0; i <= 0xb; i++) {
        if (i & 1) {
            st.f4 = 7;
            if (i & 2)
                st.f4 = 5;
            st.f8 = 0x9999;
            st.fc = 0x9999;
            v[0] = 0;
            v[1] = 0;
            v[2] = 0;
            __vec3_translate((6 - (i >> 1)) * 0x1999, ang, v);
            px = x + ((6 - (__Random() * 6 >> 16)) << 16);
            pz = z + ((6 - (__Random() * 6 >> 16)) << 16);
            OvlFunc_common0_10c(px, y, pz, v[0], v[1], v[2], 0x90 << 12, &st);
        }
        __WaitFrames(2);
    }
}
