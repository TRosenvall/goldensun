/* Cluster OvlFunc_939_2008ac4..OvlFunc_939_2008ac4 extracted from goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a.s.
 *
 * Total .text for this TU = 62 bytes (= 0x3e).
 * Preserves the original ROM layout when slotted immediately before
 * asm/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_b.o in
 * goldensun/overlays/rom_7c460c/overlay.ld.
 *
 * THIS TU NEEDS -fno-rerun-cse-after-loop (CSE_CFLAGS in the Makefile), which
 * is why the function is split into a TU of its own. The flag ID 0x243 is
 * passed to __GetFlag and then, three instructions later, to __SetFlag. With
 * the default flags gcc hoists it into a callee-saved register and copies it
 * into r0 twice; the ROM reloads it from the literal pool each time. That is
 * the 23rd TU in this project to need the flag.
 *
 * Two more things were needed on top of it:
 *
 *   - The rounding fixup and shift are ONE DIVISION, not three statements:
 *
 *          cmp r3, #0 / bge .L / ldr r2, =0xfffff / add r3, r2 / asr r5, r3, #20
 *
 *     is what gcc emits for `s = v / 0x100000;`. Spelling the fixup out by hand
 *     is unnecessary -- the single divide reproduces all five instructions.
 *   - The stored halfword's address is a NAMED pointer. Written as
 *     `*(short *)(g + k) = t`, gcc uses the register-offset form
 *     `strh r3, [r1, r2]`; the ROM computes the address first and stores at
 *     offset zero, `add r3, r2 / strh r2, [r3]`.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);

void OvlFunc_939_2008ac4(void)
{
    unsigned char *a;
    unsigned char *g;
    unsigned char *q;
    unsigned int k;
    int v;
    int s;
    int t;

    a = __MapActor_GetActor(0);
    v = *(int *)(a + 0x10);
    s = v / 0x100000;
    if (__GetFlag(0x243) != 0)
        return;
    if (s != 0xa)
        return;
    __SetFlag(0x243);
    g = iwram_3001ebc;
    k = 0xb6 << 1;
    q = g + k;
    t = 0x14;
    *(short *)q = t;
}
