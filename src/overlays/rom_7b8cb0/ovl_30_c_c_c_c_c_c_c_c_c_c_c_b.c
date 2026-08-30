/* Cluster OvlFunc_931_2008c44..OvlFunc_931_2008c44 extracted from
 * goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c.s.
 *
 * Total .text for this TU = 196 bytes.
 * Preserves the original ROM layout when slotted before
 * asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c.o in the overlay script.
 *
 * A particle step: advance position by a random jitter, drift differently
 * above and below a size threshold, occasionally retrigger, and delete the
 * actor when its life counter reaches zero.
 *
 * THE COUNTER FIELD IS `unsigned short` BUT EVERY ARITHMETIC USE IS
 * `(short)`-CAST, and that combination is what the assembly is telling you.
 * The plain read is `ldrh [r6, #0]` -- an immediate offset, so the field is
 * UNSIGNED -- while the three `mov rN, #0 / ldrsh [r6, rN]` sites and the
 * `lsl #16 / cmp #0` zero test are the casts. Getting this wrong is 70
 * differing lines, nearly all of it offset cascade behind three real
 * instructions.
 *
 * Also settled: the fall-through arm is the `<= 3` one, so the source
 * condition is `<= 3` with the `> 3` work in the else; `mul r3, r0` with r3
 * pre-loaded by `ldrsh` puts the field on the RIGHT of the multiply; and the
 * `str r3, [r5, #0x6c]` reuses the decremented counter, so the source stores
 * `n` rather than a literal zero.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[0x44];
    unsigned short f64;
    unsigned char pad66[2];
    int f68;
    int f6c;
};

extern unsigned int __Random(void);
extern void __Func_80929d8(struct Actor *a, int n);
extern void __DeleteActor(struct Actor *a);

void OvlFunc_931_2008c44(struct Actor *a)
{
    int n;

    a->f8 += ((short)a->f64 << 12)
           + ((int)((__Random() * 2 >> 16) - 1) << 16 >> 1);
    if ((short)a->f64 <= 3) {
        a->f10 = a->f10 - ((__Random() << 15) >> 16) + 0xffff0000;
        a->f18 += 0x2666;
        a->f1c += 0xfffff5c3;
    } else {
        a->f10 += 0x80 << 10;
        a->f18 += 0x7ae;
        a->f1c += 0x7ae;
    }
    if ((__Random() * (short)a->f64 >> 16) == 0)
        __Func_80929d8(a, 7);
    if ((short)a->f64 != 0)
        a->f64 = (short)a->f64 - 1;
    else
        a->f64 = (__Random() * 5 >> 16) * 2 + 2;
    n = a->f68 - 1;
    a->f68 = n;
    if (n == 0) {
        a->f6c = n;
        __DeleteActor(a);
    }
}
