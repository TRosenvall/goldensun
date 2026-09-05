// fakematch
/* OvlFunc_959_200d324  --  0x0200d324
 * [asm/overlays/rom_7e7574/ovl_9dc_c_c_c_a_a_c.s, second of four functions]
 *
 * Map setup: set the area word, run a chain of flag tests that park actors and
 * clear flags, then give five actors their idle state. Byte-exact: 332 bytes,
 * 129 encodings and 30 relocations identical.
 *
 * THE FOURTH ACTOR BLOCK IS NOT LIKE THE OTHER THREE, and the C has to say so.
 * Blocks one to three are `p = GetActor(n); if (p != 0) p[0x23] = 2;`. The
 * fourth branches around only the __Actor_SetSpriteFlags CALL and then falls
 * INTO the store, so `p[0x23] = 2` runs even when p is null. That is what the
 * ROM does -- `beq .L5410` lands on the store -- and it is transcribed as
 * written rather than tidied into the shape of its neighbours.
 *
 * TWO PINS, AND THE SECOND ONE IS THE INTERESTING ONE.
 *
 * The pooled flag id 0x217 is used twice, at a __SetFlag and a __ClearFlag in
 * different blocks. Unpinned, cse_main commons it into r5 -- which the ROM
 * needs for the actor pointer -- and the function grows a second callee-saved
 * register. Pinning the FIRST use is enough; pinning the second as well is
 * inert, exactly as the first-use rule says.
 *
 * `0x86 << 2` is also used twice, at a __SetFlag and a __ClearFlag. EACH PIN IS
 * INDIVIDUALLY INERT AND THE PAIR IS NOT JOINTLY REMOVABLE -- dropping both
 * costs 36 differing and shifts every relocation after the site by four bytes.
 * Exactly one of the two must survive, and BOTH CHOICES ARE BYTE-EXACT: the
 * variant keeping the first pin and the variant keeping the second both give
 * 332 bytes and 129 identical encodings. This is the clearest small example of
 * "N pins is a size, not a set" -- the two sites are a same-value chain, and
 * the set is not unique. The first is kept here only because it reads better.
 *
 * The last four instructions were a pure register-number swap in the if-join
 * block: the ROM puts the store's ADDRESS in r3 and its value in r2, while the
 * three guarded blocks use r2 for the address -- the ROM is inconsistent with
 * itself here, and only that one block differs. Four spellings of the store
 * (`p[0x23]`, a cast, a temp pointer, a goto) are all byte-identical and none
 * moves it; binding the address to r3 with a local register variable does.
 *
 * 0x200 comes off the offset register for free (`add r2, #0x40` on the 0x1c0
 * already there) by writing the store plainly.
 *
 * No wildcard captures this object -- the rom_7e7574 rules in the Makefile are
 * all exact rules for other basenames -- so the tree default -O2 applies.
 */
extern char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_80108c4(int a);
extern void OvlFunc_959_200d4dc(void);
extern void OvlFunc_959_2008c78(void);
extern void OvlFunc_959_200a2d4(void);
extern void OvlFunc_959_200a26c(void);
extern void OvlFunc_959_200a2a0(void);

#define PIN1 register int q0 __asm__("r0")

void OvlFunc_959_200d324(void)
{
    unsigned char *p;

    *(int *)(iwram_3001ebc + 0x1c0) = 0x200;
    OvlFunc_959_200d4dc();
    if (__GetFlag(0x943))
        OvlFunc_959_2008c78();
    { PIN1; q0 = 0x217; __SetFlag(q0); }
    { PIN1; q0 = 0x86; q0 <<= 2; __SetFlag(q0); }
    if (__GetFlag(0x944)) {
        __MapActor_SetPos(8, 0, 0);
        __ClearFlag(0x217);
    }
    if (__GetFlag(0x945)) {
        __MapActor_SetPos(9, 0, 0);
        OvlFunc_959_200a2d4();
    }
    if (__GetFlag(0x946)) {
        __MapActor_SetPos(0xa, 0, 0);
        __ClearFlag(0x86 << 2);
    }
    if (__GetFlag(0x947))
        OvlFunc_959_200a26c();
    if (__GetFlag(0x948))
        OvlFunc_959_200a2a0();
    __CutsceneStart();
    p = __MapActor_GetActor(8);
    if (p != 0)
        p[0x23] = 2;
    p = __MapActor_GetActor(9);
    if (p != 0)
        p[0x23] = 2;
    p = __MapActor_GetActor(0xa);
    if (p != 0)
        p[0x23] = 2;
    p = __MapActor_GetActor(0xb);
    if (p != 0)
        __Actor_SetSpriteFlags(p, 0);
    { register unsigned char *q3 __asm__("r3"); q3 = p + 0x23; *q3 = 2; }
    p = __MapActor_GetActor(0xc);
    if (p != 0)
        p[0x59] |= 0x10;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
    __CutsceneEnd();
    __Func_80108c4(0xe0 << 4);
}
