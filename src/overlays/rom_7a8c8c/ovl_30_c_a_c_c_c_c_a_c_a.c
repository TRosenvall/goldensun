/* Cluster OvlFunc_922_2009750..OvlFunc_922_2009750 extracted from goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * UNPARKED. This was filed under constant-CSE and was a documented
 * counter-example: the offset 0x1c0 is built twice in the ROM with a
 * __GetFlag call between, gcc built it once and kept it in r5 across the call
 * -- and `-fno-rerun-cse-after-loop`, which handles the other members of that
 * class, was byte-identical here. The park's guess was that the flag only
 * reaches values used as CALL ARGUMENTS, not values used in address arithmetic.
 *
 * THE FIX IS THE BASIC-BLOCK LEVER WITH TWO SEPARATE LOCALS. `o1` and `o2` hold
 * the same value and are assigned in the entry block; both uses are past a
 * branch, so gcc rematerialises each rather than keeping one alive. That also
 * gets the ROM's `add r3, r1` / `add r3, r2` folded offsets instead of gcc's
 * register-offset addressing, which is where the three missing instructions
 * were.
 *
 * So the park's guess was wrong in an instructive way: the discriminator is not
 * argument-versus-address, it is whether the two uses sit in a different basic
 * block from the assignment. Nothing about this case needed a compiler flag.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int iwram_3001ebc;
extern int _AREA_34;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void OvlFunc_922_20097a8(void);
extern void OvlFunc_922_20097e4(void);

int OvlFunc_922_2009750(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned int o1;
    unsigned int o2;
    int mode;

    o1 = 0xe0 << 1;
    o2 = 0xe0 << 1;
    p = (unsigned char *)iwram_3001ebc;
    p += o1;
    mode = 0x81 << 2;
    *(int *)p = mode;
    if (!__GetFlag(0x109)) {
        g = (unsigned char *)&gState;
        g += o2;
        if (*(short *)g == (int)(&_AREA_34)) {
            __SetFlag(0xa2 << 1);
            OvlFunc_922_20097a8();
            return 0;
        }
    }
    OvlFunc_922_20097e4();
    return 0;
}
