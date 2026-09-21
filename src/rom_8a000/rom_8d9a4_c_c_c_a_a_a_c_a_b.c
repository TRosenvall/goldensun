/* Cluster Func_809088c..Func_809088c extracted from goldensun/asm/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_a.s.
 *
 * Total .text for this TU = 84 bytes (= 0x54). Parked in an earlier batch; elevated in
 * batch 277. No pins, no flags, NO per-object symbol rename and NO flag group.
 *
 * NEAR-TWIN: Func_80f2ebc, at src/rom_f2000/rom_f2028_c_c_a_a_a_b.c. tools/dupfuncs.py
 * paired them and they are NOT quite identical -- ONE constant differs, the loop bound
 * (`ldr r2, =0x53f` here against `=0x5ff` there, i.e. 0x540 against 0x600). Everything
 * else is the same instruction stream, and both compile clean from this body with that one
 * literal changed. They are landed as two files rather than one shared body for that
 * reason; if you edit one, check the other.
 *
 * `divsi3_RAM` IS REACHABLE FROM PLAIN C. THE PARK SAID OTHERWISE AND SAID SO EMPHATICALLY.
 *
 * The park concluded the C was finished at 2 differing, that a flag group AND a per-object
 * symbol rename were both required, and instructed: "Do not re-try the function-pointer
 * route." That instruction was the error.
 *
 *     extern int divsi3_RAM(int, int);
 *     int (*f)(int, int);
 *     f = divsi3_RAM;        <- ASSIGNED INSIDE the `if (n > 0)` guard, just before the loop
 *
 * emits `ldr r1, =divsi3_RAM / mov r10, r1` in the preheader and `bl _call_via_sl` in the
 * body. Exact, under the tree's default flags. The park's 35-line measurement for the
 * pointer route was taken against a different loop body; with three pointer cursors and
 * the pointer named INSIDE the guard, the counter's spill to `[sp]` falls out on its own.
 * Note the placement is load-bearing -- this is the batch-276 rule that where a value is
 * assigned is an axis separate from whether it is named.
 *
 * The declaration is honest, not a convenient alias: `arm-none-eabi-nm goldensun.elf`
 * gives `03000380 T divsi3_RAM` and `080022ec T __divsi3` as two distinct real symbols,
 * the first being the copy that runs from RAM. Verified again at landing.
 */
extern int divsi3_RAM(int a, int b);

void Func_809088c(short *a, short *b, short *c, int n)
{
    int i;
    int u;
    int v;
    int (*f)(int, int);

    if (n > 0) {
        f = divsi3_RAM;
        for (i = 0; i < 0x540; i++) {
            u = *a;
            v = *b;
            *c = f(v - u, n);
            a++;
            b++;
            c++;
        }
    }
}
