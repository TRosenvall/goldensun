/* GetMercuryDjinni  --  0x080965a8
 *
 * MATCHES.  572 bytes, 239 encodings, 46 relocations identical.
 *
 * The ONLY function in asm/rom_8a000/rom_944ec_a_c_a_c_c_c.s, so this is a
 * WHOLE-FILE landing with no split.  stage1.ld line 955 names the object once
 * and must stay verbatim with its asm/ prefix:
 *
 *              asm/rom_8a000/rom_944ec_a_c_a_c_c_c.o(.text)
 *
 * No Makefile flag rule; the generic `asm/%.o: src/%.c` rule builds it at the
 * tree default -O2.
 *
 * THE .s's HEADER COMMENT IS WRONG -- it reads "RunVehicleCutscene ... takes no
 * arguments".  The .thumb_func_start says GetMercuryDjinni and the body takes a
 * slot argument.  Do not trust it.
 *
 * This closes the 17-instruction park formerly at
 * src/non_matching/rom_8a000/80965a8.c.  Levers 1-8 of that note all still
 * apply and are not repeated here; the two things that changed are:
 *
 * 1. THE SECOND PIN.  The park's residue was a clean three-cycle over the last
 *    60 instructions -- ROM bp->r5, p->r6, loop-4 counter->r7 against ours
 *    p->r5, counter->r6, bp->r7 -- with `register unsigned char *e __asm__("r6")`
 *    already closing the first 175 instructions.  The park concluded a pin
 *    "cannot reach" the second cycle because r5/r6/r7 are all live elsewhere in
 *    the body.  That is the wrong test.  What matters is whether the pinned
 *    variable's OWN live range conflicts with the hard register, not whether
 *    the register is busy somewhere else in the function.  `bp` is born after
 *    `g` (the gState pointer, also r5) dies, so `bp __asm__("r5")` is conflict-free
 *    and it drops straight in: 17 -> 0 on the first try.  `p asm("r6")` is the
 *    same cycle attacked from the other member and gets 6, so the pin does have
 *    to land on the highest-priority member, not just any of them.
 *
 * 2. THE LOOP-4 COUNTER SPLIT WAS A SYMPTOM OF THE SAME DEFECT, NOT A LEVER.
 *    The park carried a separate `i4` for loop 4 because sharing one counter
 *    across all five loops measured 248 lines against 244.  With `bp` pinned,
 *    ONE shared `i` is byte-identical -- the split had been buying back length
 *    that the misallocation was costing.  The simpler source is shipped.
 *    (Re-measured without the pin: shared counter still comes out long, so the
 *    park's 248 was correct for the candidate it was measured on.)
 *
 * Both `register` declarations are BARE pins with no `__asm__ volatile`
 * barrier, which per docs/elevation.md ("Try the BARE register pin before the
 * barrier") is the weaker and preferred form.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001f30;
extern unsigned char *MapActor_GetActor(int slot);
extern void Func_80958a8(void);
extern void _Func_80b0840(int a);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void Func_80925cc(int a, int b);
extern void MapActor_Jump(int a, int b, int c);
extern unsigned char *_CreateActor(int id, int x, int y, int z);
extern void _Actor_SetColorswap(unsigned char *a, int n);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _Actor_SetSpriteFlags(unsigned char *a, int n);
extern unsigned char *Func_8096c48(int a, unsigned char *b);
extern void Func_8096574(void);
extern void _DeleteActor(unsigned char *e);
extern void Func_8003f3c(int n);
extern void Func_8092adc(int a, int b, int c);
extern void Func_80974d8(int *v);
extern void Func_809ba90(unsigned char *p, int a, int b, int c);
extern void Func_809ba7c(unsigned char *p, void (*f)(void));
extern void Func_809641c(void);
extern void Func_809ba70(unsigned char *p, int n);
extern void _Sprite_SetColorswap(int a, int b);
extern void _Func_80b0894(void);
extern void Func_80958e4(void);

void GetMercuryDjinni(int slot)
{
    unsigned char *arr[8];
    int v[3];
    register int *bp __asm__("r5");
    unsigned char **q;
    register unsigned char *e __asm__("r6");
    unsigned char *base;
    unsigned char **ptr;
    unsigned char *a;
    unsigned char *prev;
    unsigned char *h;
    unsigned char *p;
    unsigned char *w;
    unsigned char *g;
    int id;
    int last;
    int val;
    int i;
    int z;

    e = MapActor_GetActor(slot);
    if (e == 0)
        return;
    Func_80958a8();
    base = iwram_3001f30;
    _Func_80b0840(0x204084);
    WaitFrames(0x1e);
    *(char *)(e + 0x5b) = 0;
    _PlaySound(0xad);
    Func_80925cc(slot, 1);
    _PlaySound(0xaf);
    Func_80925cc(slot, 1);
    WaitFrames(0x14);
    _PlaySound(0x98);
    MapActor_Jump(slot, 3, 0xe);
    _PlaySound(0x98);
    MapActor_Jump(slot, 5, 0x10);
    _PlaySound(0x98);
    MapActor_Jump(slot, 7, 0x12);
    WaitFrames(0x14);
    id = *(short *)(*(int *)(*(int *)(e + 0x50) + 0x28));
    h = 0;
    prev = e;
    ptr = arr;
    i = 7;
loop1:
    a = _CreateActor(id, *(int *)(e + 8), *(int *)(e + 0xc), *(int *)(e + 0x10));
    *ptr++ = a;
    if (a != 0) {
        *(int *)(a + 0x1c) = 0xf0 << 8;
        *(int *)(a + 0x18) = 0xf0 << 8;
        z = 0;
        *(char *)(a + 0x55) = z;
        *(char *)(a + 0x23) = 2;
        *(unsigned char *)(a + 0x5a) |= 1;
        *(void **)(a + 0x6c) = Func_8096574;
        *(unsigned short *)(a + 6) = *(unsigned short *)(e + 6);
        _Actor_SetColorswap(a, 9);
        _Actor_SetAnim(a, 0);
        _Actor_SetSpriteFlags(a, 0);
        h = Func_8096c48(*(int *)(a + 0x50), h);
        *(unsigned char **)(a + 0x68) = prev;
        prev = a;
    }
    i--;
    if (i >= 0)
        goto loop1;
    last = *(unsigned char *)(h + 0x1c);
    _PlaySound(0x99);
    *(char *)(e + 0x55) = 0;
    i = 0xe;
loop2:
    *(int *)(e + 0xc) += 0x80 << 12;
    WaitFrames(1);
    i--;
    if (i >= 0)
        goto loop2;
    _DeleteActor(e);
    q = arr;
    i = 7;
loop3:
    _DeleteActor(*q++);
    i--;
    if (i >= 0)
        goto loop3;
    if (last != 0x60)
        Func_8003f3c(last);
    WaitFrames(0xa);
    g = gState;
    g += (0xfa << 1);
    Func_8092adc(*(int *)g, 0x80 << 7, 0);
    WaitFrames(0x14);
    _Actor_SetAnim(MapActor_GetActor(*(int *)g), 0x1c);
    WaitFrames(0x14);
    bp = v;
    bp[0] = *(int *)(e + 8);
    bp[1] = *(int *)(e + 0xc);
    bp[2] = *(int *)(e + 0x10);
    Func_80974d8(bp);
    p = base + 0x58;
    i = 0x17;
loop4:
    Func_809ba90(p, 0xf0, bp[0], bp[2]);
    Func_809ba7c(p, Func_809641c);
    Func_809ba70(p, 7);
    _Sprite_SetColorswap(*(int *)p, 9);
    i--;
    WaitFrames(1);
    p += 0x48;
    if (i >= 0)
        goto loop4;
    WaitFrames(0x78);
    w = base;
    val = 2;
    w += 0x98;
    i = 0x17;
loop5:
    if (*(signed char *)(w + 5) != 0)
        *w = val;
    i--;
    w += 0x48;
    if (i >= 0)
        goto loop5;
    WaitFrames(0x32);
    _Func_80b0894();
    Func_80958e4();
}
