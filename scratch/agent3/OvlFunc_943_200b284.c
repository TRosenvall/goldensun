/* OvlFunc_943_200b284 -- MATCHES on the default flags (and unchanged under
 * -fno-rerun-cse-after-loop).  ref: asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_c.s
 * tryc.py: OK (92 lines).  Byte-verified: 252 bytes of .text identical, with
 * the R_ARM_ABS32 against .L5b90 (scratch/agent1/bytecheck.sh).  Matched first try.
 *
 * The table is the overlay's own `.lcomm .L5b90, 4` slot (declared 4 bytes,
 * indexed to +0x1c), reached with the asm-label extension:
 *     extern int L5b90[] __asm__(".L5b90");
 * No .s change is needed -- .L5b90 is already `.global` in
 * asm/overlays/rom_7c7b9c/ovl_30_c_c.s and is referenced from two other .s
 * files, so it must NOT be renamed.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_943_200b380(int slot);
extern void OvlFunc_943_200b3b8(void);
extern int L5b90[] __asm__(".L5b90");

void OvlFunc_943_200b284(void)
{
    int z;
    int w;

    z = 0;
    *(__MapActor_GetActor(8) + 0x59) = z;
    *(__MapActor_GetActor(9) + 0x59) = z;
    *(__MapActor_GetActor(0xa) + 0x59) = z;
    *(__MapActor_GetActor(0xb) + 0x59) = z;
    OvlFunc_943_200b380(8);
    OvlFunc_943_200b380(9);
    OvlFunc_943_200b380(0xa);
    OvlFunc_943_200b380(0xb);
    OvlFunc_943_200b380(0xc);
    OvlFunc_943_200b380(0xd);
    OvlFunc_943_200b380(0xe);
    OvlFunc_943_200b380(0xf);
    L5b90[0] = *(int *)(__MapActor_GetActor(0xc) + 0x10);
    L5b90[1] = *(int *)(__MapActor_GetActor(0xd) + 0x10);
    L5b90[2] = *(int *)(__MapActor_GetActor(0xe) + 0x10);
    L5b90[3] = *(int *)(__MapActor_GetActor(0xf) + 0x10);
    OvlFunc_943_200b380(0x10);
    OvlFunc_943_200b380(0x11);
    OvlFunc_943_200b380(0x12);
    OvlFunc_943_200b380(0x13);
    w = 0xffff0000;
    *(int *)(__MapActor_GetActor(0x10) + 0x18) = w;
    *(int *)(__MapActor_GetActor(0x11) + 0x18) = w;
    *(int *)(__MapActor_GetActor(0x12) + 0x18) = w;
    *(int *)(__MapActor_GetActor(0x13) + 0x18) = w;
    L5b90[4] = *(int *)(__MapActor_GetActor(0x10) + 0x10);
    L5b90[5] = *(int *)(__MapActor_GetActor(0x11) + 0x10);
    L5b90[6] = *(int *)(__MapActor_GetActor(0x12) + 0x10);
    L5b90[7] = *(int *)(__MapActor_GetActor(0x13) + 0x10);
    OvlFunc_943_200b3b8();
}
