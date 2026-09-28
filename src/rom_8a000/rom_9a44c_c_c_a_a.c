/* Field_Whirlwind -- 0x0809a8c4.  EXACT.
 * ref: asm/rom_8a000/rom_9a44c_c_c_a_a.s  (ONE function, no data section --
 *      grep -ci func_start = 1; converts WHOLE FILE, no split needed)
 * objcmp: OK whole file -- 468 bytes, 202 encodings and 27 relocations identical.
 *
 * batch 292, brief A, target 5.  NO SHIMS: no pins, no "+r" barriers, no
 * volatile, no do{}while(0), no .equ, default flags.  The one unusual
 * construct is a union, and it is load-bearing -- see lever 5.
 *
 * Structural twin of Field_Halt (asm/rom_8a000/rom_9a44c_c_c_c.s): both read
 * [iwram_3001f30], build a from/to vec3 pair on the stack and interpolate
 * i/10 of the way between them over 11 frames through __divsi3.
 *
 * THE FIVE LOAD-BEARING CONSTRUCTS, each with the measurement that proves it
 * (drop it singly and the number in brackets is what objcmp reports; the
 * candidate is 202 encodings / 468 bytes, so every number below is a TRUE
 * distance -- size and count both agreed):
 *
 * 1. THREE SEPARATE int[3] ARRAYS, declared v, from, to.  sub sp, #0x24 = 36
 *    bytes and "frame layout follows DECLARATION ORDER, last-declared lowest"
 *    puts `to` at sp+0 (r6), `from` at sp+0xc (r5), `v` at sp+0x18.  `to` and
 *    `v` share r6 with disjoint live ranges and must still be two variables.
 *
 * 2. `i * (to[k] - from[k])`, NOT the reverse.  *thumb_mulsi3 ties operand 0
 *    to operand 1, so the ROM's `mov r0, r8 / mul r0, r3` names `i` as the
 *    FIRST source operand.  (The brief's mul lever, confirmed a third time.)
 *
 * 3. A NAMED BOUND for the first loop: `n = 11; for (; i < n; i++)`.
 *    combine.c's simplify_comparison rewrites `LT C` (C>0) to `LE C-1`
 *    unconditionally for a comparison whose constant it can see, so `i < 11`
 *    gives the ROM's neighbour `cmp #0xa / ble`; the bound arriving through a
 *    local escapes the rewrite and restores `cmp #0xb / blt`.  [33 differing]
 *    The SECOND loop is written `j <= 15` -- the ROM's `cmp #0xf / ble` is the
 *    un-rewritten form and needs no local.  Two loops, two spellings, and the
 *    ROM says which is which.
 *
 * 4. AN int CARRIER FOR THE HALFWORD ZERO, WITH ONLY ONE OF THE TWO STORES
 *    GOING THROUGH A NAMED POINTER.  `*(short *)(q+0x64) = 0` pools the zero
 *    (`ldr r2, =0x0`) where the ROM has `mov r2, #0`; an int local fixes that.
 *    But a fresh int local is then LOOP-INVARIANT and loop.c hoists it into
 *    the preheader as `mov r10, r3` [75 differing, ours 202 -- a real
 *    distance].  move_movables' threshold test (loop.c:1803,
 *    `threshold * savings * m->lifetime >= insn_count`) decides it, and
 *    m->lifetime is the knob: the loop is 49 real insns and the effective
 *    threshold puts the cut between lifetime 3 (kept) and 4 (hoisted).
 *      - plain casts for both stores  -> lifetime 4, hoisted  [75]
 *      - a named pointer for BOTH stores (`*h++ = z; *h = z;`) -> lifetime 3,
 *        kept, but the pointer then outranks the zero in local-alloc and the
 *        two swap r2/r3                                        [7-8]
 *      - a named pointer for the FIRST store only -> lifetime 3, kept, AND
 *        the second address stays its own short-lived pseudo, which is what
 *        keeps r3 for the addresses and r2 for the zero (REG_ALLOC_ORDER is
 *        {3,2,1,0,...}, so the address quantity is allocated first)   [3]
 *    Reusing the loop-1 scale local `w` as the zero also defeats the hoist
 *    (its first reference is then in loop 1, so reg_in_basic_block_p fails and
 *    the set is "unsafe to move") but makes it a GLOBAL allocno and it lands
 *    in r0 instead of r2 [5].  Declaration order of z and h: INERT (all five
 *    permutations measured, all identical) -- the documented behaviour for
 *    register-resident locals.
 *
 * 5. THE 0x68 STORE IS A UNION MEMBER ACCESS, AND THAT IS A SCHEDULING
 *    DEPENDENCE, NOT A TYPE PUN.  Written `*(unsigned char **)(q + 0x68) = p`
 *    the store carries the alias set of `unsigned char *` and the halfword
 *    stores carry `short`'s; two distinct non-zero sets do not conflict, so
 *    sched2 is free to sink the word store past both `strh`s and does, by two
 *    slots [3 differing].  A union member access is ALIAS SET 0, which
 *    conflicts with everything, so the anti-dependence comes back and sched2
 *    leaves the store where the ROM has it -- between `add r3, #0x64` and the
 *    first `strh`.  MEASURED ALTERNATIVES THAT DO NOT REACH IT, all [3]:
 *    `*(void **)`, `*(char **)` with a cast, and `volatile` on the pointee.
 *    Only the union is alias set 0.  Putting the union on the halfword stores
 *    instead of the word store is ALSO exact, so either end of the pair works;
 *    this file puts it on the word store because it is one access, not two.
 *    -fno-schedule-insns2 confirms the pass: it pins the word store one slot
 *    EARLIER than the ROM, so this is a sched2 tie and not an RTL-order
 *    artefact.
 *
 * ALSO MEASURED AND INERT: the source position of the 0x68 store among the
 * other four statements (all six positions, 3 differing every time);
 * `*h = z; h++; *h = z;` against `*h++ = z; *h = z;`; `unsigned short *` for
 * the pointer; a `short` zero carrier instead of `int`; `do { } while (0)`
 * barriers in five positions (best 4, never exact).
 */
union blob { unsigned char *pp; short hh; int ii; };

extern int *iwram_3001f30;
extern unsigned char *CreateParticleActor(int a, int b, int c, int d);
extern void Func_8097384(void);
extern void _Actor_SetAnim(unsigned char *p, int n);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern unsigned int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern void _DeleteActor(unsigned char *p);
extern void Func_809748c(void);
extern void Func_809a890(void);
extern void Func_809a7f4(void);

void Field_Whirlwind(void)
{
    int v[3];
    int from[3];
    int to[3];
    int *o;
    int *t;
    unsigned char *e;
    unsigned char *p;
    unsigned char *q;
    int i;
    int j;
    int w;
    int n;
    int z;
    short *h;

    o = iwram_3001f30;
    t = (int *)o[4];
    e = (unsigned char *)o[5];
    i = 0;
    n = 11;
    from[0] = t[2];
    from[1] = t[3];
    from[2] = t[4];
    to[0] = o[1];
    to[1] = o[2] - 0x40000;
    to[2] = o[3];
    p = CreateParticleActor(0xda, 0, 0, 0);
    if (p == 0)
        return;
    Func_8097384();
    _Actor_SetAnim(p, 2);
    for (; i < n; i++) {
        *(int *)(p + 8) = from[0] + i * (to[0] - from[0]) / 10;
        *(int *)(p + 0xc) = from[1] + i * (to[1] - from[1]) / 10;
        *(int *)(p + 0x10) = from[2] + i * (to[2] - from[2]) / 10;
        w = i * 0x10ccc / 10 + (0x80 << 7);
        *(int *)(p + 0x18) = w;
        *(int *)(p + 0x1c) = w;
        WaitFrames(1);
    }
    *(int *)(p + 0x18) = 0x1b333;
    *(int *)(p + 0x1c) = 0x14ccc;
    _PlaySound(0xa3);
    WaitFrames(0x14);
    if (*(signed char *)((char *)o + 0x20) == 0) {
        if (e != 0)
            *(void (**)(void))(e + 0x6c) = Func_809a890;
        for (j = 0; j <= 15; j++) {
            v[0] = *(int *)(p + 8);
            v[1] = *(int *)(p + 0xc) + j * 0xcccc + (0x80 << 11);
            v[2] = *(int *)(p + 0x10);
            vec3_translate(Random() * 5 + (0xc0 << 10), Random(), v);
            q = CreateParticleActor(0xf9, v[0], v[1], v[2]);
            if (q != 0) {
                *(void (**)(void))(q + 0x6c) = Func_809a7f4;
                ((union blob *)(q + 0x68))->pp = p;
                h = (short *)(q + 0x64);
                z = 0;
                *h = z;
                *(short *)(q + 0x66) = z;
                *(unsigned short *)(q + 6) = Random();
            }
            WaitFrames(6);
        }
        WaitFrames(0x14);
        WaitFrames(0x78);
    }
    _Actor_SetAnim(p, 1);
    WaitFrames(0x1e);
    _PlaySound(0x88);
    WaitFrames(0x14);
    _DeleteActor(p);
    Func_809748c();
}
