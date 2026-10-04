/*
 *
 * NON-MATCHING, 57 of 61 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 61, ours 55) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 136 bytes, ours 124.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/overlays/2008e00.c \
 *     asm/overlays/rom_7ced6c/ovl_30_c_c_a_a_a_c.s --func OvlFunc_946_2008e00
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * OvlFunc_946_2008e00 -- asm/overlays/rom_7ced6c/ovl_30_c_c_a_a_a_c.s
 *
 * BLOCKER: register pressure. 53 lines against 59 -- SIX SHORT. The ROM keeps
 * three values in high callee-saved registers across the loop (r8 = the
 * counter, r9 = the 0x28-byte block at sp+0x10, r10 = the actor) and pushes
 * {r5, r6, r7} on top; we keep fewer and gcc recomputes instead.
 *
 * SETTLED, and it is worth keeping:
 *
 *   The STACK LAYOUT is controlled by declaration order, and it is backwards
 *   from what you would guess. The frame is 0x44: outgoing args at sp+0,
 *   a 0x28-byte block at sp+0x10, a three-int vector at sp+0x38. Declaring the
 *   big array FIRST puts it at sp+0x1c -- gcc allocates in REVERSE declaration
 *   order. Declaring the vector first and the array second gives the ROM's
 *   sp+0x10 and sp+0x38. That is a clean, checkable lever and it fixed every
 *   stack offset in the function.
 *
 * TRIED AND REJECTED:
 *
 *   * Naming the two block addresses in explicit pointer locals (`ap = a;
 *     vp = v;`) to force them into registers the way the ROM holds them. NO
 *     CHANGE AT ALL -- gcc CSEs them straight back to sp-relative addressing.
 *     Getting r9 and r10 populated needs something other than a pointer local.
 *
 * Also note `add r2, sp, #0x10`: the ROM uses the single-instruction
 * ADD Rd, SP, #imm form and we emit `mov r2, #0x10 / add r2, sp`. That is part
 * of the six-instruction gap and it follows from the same pressure problem, not
 * from the offset being wrong -- the offset is now right.
 */
extern unsigned char *__MapActor_GetActor(void);
extern int __cos(int a);
extern int __sin(int a);
extern int _divsi3_RAM(int a, int b);
extern void OvlFunc_946_2008da4(void);
extern void OvlFunc_946_2008ae8(int a, int b, int c, int d, int e, int f, int g, int *h);

void OvlFunc_946_2008e00(void)
{
    int v[3];
    int a[10];
    unsigned char *act;
    unsigned int i;
    int s;

    act = __MapActor_GetActor();
    a[9] = (int)OvlFunc_946_2008da4;
    i = 0;
    do {
        v[1] = 0;
        v[0] = __cos(i << 12);
        s = __sin(i << 12);
        v[2] = s;
        v[0] = v[0] + _divsi3_RAM(v[0], 3);
        OvlFunc_946_2008ae8(*(int *)(act + 8), *(int *)(act + 0xc), *(int *)(act + 0x10),
                            v[0], v[1], s, 0x1000001, a);
        i += 2;
    } while (i <= 0x10);
}
