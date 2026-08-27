/* OvlFunc_932_200b5ac -- PARKED at 5 differing of 86.  Line count exact.
 * ref: asm/overlays/rom_7b9cb4/ovl_30_c_a.s   first diff at position 40.
 *
 * The residue is ONE high-register exchange: the ROM has x=r9, z=r10, i=r8;
 * we get x=r9, z=r8, i=r10.  All five differing instructions are that swap
 * (`mov r10,r3` vs `mov r8,r3`, `mov r2,r10` vs `mov r2,r8`, `add r8,r3` vs
 * `add r10,r3`).  Nothing else differs anywhere in the function.
 *
 * MEASURED, all still 5 unless noted:
 *   - ALL 24 permutations of the four local declarations: 5 every time.
 *     Declaration order is inert here.
 *   - `for (i = 15; i >= 0; i--)`, `while (1) { ...; if (i<0) break; }`,
 *     `while (i > -1)` (17), `short i` (30).
 *   - moving `i = 15;` earlier (before x, before z, before ang+=, before
 *     __Actor_SetAnim): 51, 25, 16, 13 -- all worse.
 *   - a named `int *p = v;` used throughout; `unsigned int` x and z.
 *   - -fno-rerun-cse-after-loop, -fno-gcse, -fno-schedule-insns,
 *     -fno-strength-reduce: 5 each; -fno-expensive-optimizations: 64.
 *
 * ONE thing that WAS load-bearing: inside the loop the two stores must be
 * written `v[0] = x; v[2] = z;` in that order even though the ROM EMITS the
 * v[2] store first -- source order v[2],v[0] is 8 of 86.
 */
extern void __vec3_translate(int dist, int ang, int *v);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);

void OvlFunc_932_200b5ac(unsigned char *a)
{
    int v[3];
    int ang;
    int x;
    int z;
    int i;

    ang = (*(unsigned short *)(a + 6) + (0x80 << 7)) & (0xc0 << 8);
    v[0] = *(int *)(a + 8);
    v[1] = *(int *)(a + 0xc);
    v[2] = *(int *)(a + 0x10);
    __vec3_translate(0xc0 << 13, ang, v);
    x = (v[0] + (0x80 << 12)) & 0xfff00000;
    z = (v[2] + (0x80 << 12)) & 0xfff00000;
    ang += 0x80 << 8;
    __Actor_SetAnim(a, 5);
    __PlaySound(0xb8);
    i = 15;
    do {
        ang += 0x80 << 3;
        v[0] = x;
        v[2] = z;
        __vec3_translate(0xc0 << 13, ang, v);
        *(int *)(a + 8) = v[0];
        *(int *)(a + 0x10) = v[2];
        *(short *)(a + 6) = ang + (0x80 << 7);
        __WaitFrames(1);
        i--;
    } while (i >= 0);
    __PlaySound(0xe9);
}
