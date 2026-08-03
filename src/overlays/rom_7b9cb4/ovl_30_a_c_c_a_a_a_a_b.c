/* Cluster OvlFunc_932_2008388..OvlFunc_932_2008388 extracted from goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_a.o and asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_c.o in
 * goldensun/overlays/rom_7b9cb4/overlay.ld.
 *
 * THE FIRST FUNCTION ELEVATED OUT OF THE pool-tell BLOCKER.
 *
 * The ROM loads 0x4d from the literal pool rather than building it with
 * `mov r0, #0x4d`. gcc-2.96 never pools a constant it can `mov`, and always
 * pools a symbol's address, so that operand was a SYMBOL REFERENCE in the
 * original -- no spelling of a literal reproduces it. That tell gates 103 of
 * 395 overlay candidates.
 *
 * The fix is to give the value a name. `_ID_4d` is an absolute symbol defined
 * in unknown_id.sym; an absolute symbol definition emits no bytes, so taking
 * its address is byte-identical to the literal while being the shape gcc
 * pools. Its SEMANTICS are still unknown -- see unknown_id.sym.
 *
 * Two siblings follow this function in the same .s (OvlFunc_932_20083b4 with
 * 0x4f and OvlFunc_932_20083e0 with 0x51) and are mechanical once split.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_4d;
extern void __Func_8091f90(int id, int b);
extern void __Func_8091eb0(int map, int entrance);

void OvlFunc_932_2008388(void)
{
    unsigned char *gs;

    gs = (unsigned char *)&gState;
    gs[0x22b] = 3;
    __Func_8091f90((int) (&_ID_4d), 0x63);
    __Func_8091eb0(0x35, 2);
}
