/* OvlFunc_957_200909c (0x0200909c) -- NON-MATCHING, 368 of 393 encodings differ.
 * COUNT DOES NOT MATCH: ref 377, ours 393, size 860 against 916. The 368 is NOT
 * a distance; the number that matters is the SIXTEEN-INSTRUCTION EXCESS, and it
 * is one blocker, not sixteen.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7e3e08/200909c.c \
 *     asm/overlays/rom_7e3e08/ovl_30_c_c_c_a_a_a_a.s --func OvlFunc_957_200909c
 *
 * SPLIT SHAPE. asm/overlays/rom_7e3e08/ovl_30_c_c_c_a_a_a_a.s holds THREE
 * functions -- OvlFunc_957_2008f94 (111 instructions, already parked at
 * src/non_matching/ovl_7e3e08/2008f94.c), this one, and OvlFunc_957_20093f8
 * (2316 instructions, unattempted). A three-way split is needed before any of
 * them converts; taking this one alone means
 *     ovl_30_c_c_c_a_a_a_a.s -> _a_a.s (2008f94) + _a_b.c (200909c) + _a_c.s (20093f8)
 * EXPORTS: OvlFunc_957_200909c; it takes the ADDRESS of its file-mate
 * OvlFunc_957_2008f94 (`__StartTask` / `__StopTask`), so that symbol must stay
 * `.global` whichever side of the split it lands on.
 * NO TWIN: dupfuncs.py pairs nothing here, and the callee-signature grep that
 * found the 923/924 twin (see src/non_matching/ovl_7aa430/200a030.c) hits only
 * this function.
 *
 * ============================================================
 * WHAT IS RIGHT, and it is the whole control flow
 * ============================================================
 * The call sequence, the branch structure and every constant line up. The
 * function is the ladder cutscene: a five-actor reset loop, then a
 * `(signed char)ewram_2001001` state machine over 0 / 1 / 2 with an `arg` test
 * inside each arm, then a trailing block guarded by a flag that only the
 * succeeding arms set. The trailing block starts OvlFunc_957_2008f94 as a task,
 * spins on the shared state halfword reaching 0x63 or 2, and on the `> 2` path
 * runs a descending ladder animation before OvlFunc_957_200bad4 and __StopTask.
 *
 * Load-bearing, and each confirmed by the alignment:
 *   - `extern unsigned char *L3f6c __asm__(".L3f6c");` with the state pointer
 *     RE-READ as `*(short **)&L3f6c` at every use -- the same convention the
 *     file-mate park 2008f94.c already uses.
 *   - the three ewram bytes reached through ONE held base: `q = &ewram_2001001`,
 *     `q[0]`, `(signed char)q[1]`, `q[1]`, and in the tail `p[-1]` for
 *     ewram_2001000. Separate symbols cost a second pool load; the ROM's
 *     `sub r3, r7, #1` is the tell that the base is named and the neighbour is
 *     reached from it.
 *   - `ewram_2001002` read BOTH ways in the same breath: `*(signed char *)(q + 1)`
 *     gives the ROM's `ldrsb r6, [r3, r6]` -- Thumb has no immediate-offset
 *     `ldrsb`, which is what the odd `mov r6, #1` beside it is for -- and `q[1]`
 *     gives the `ldrb r2, [r3, #1]` next to it.
 *   - `e[3] = _divsi3_RAM(d << 16, 5) + (0x80 << 7)`.
 *   - the angle recurrence as `t = (short)((unsigned short)t + cc)` with cc a
 *     NAMED LOCAL holding 0xffffcccd. Written as a literal, fold narrows the
 *     constant to 0xcccd (`ldr r3, =0xcccd`) and the `add` loses the high half;
 *     the ROM keeps the full word in r10. This one is required even though it
 *     does not move the count.
 *   - the five loops as `for (iN = 0; iN <= 4; iN++)` over slot `iN + 0xb`.
 *
 * ============================================================
 * THE BLOCKER: THE ROM'S LOOP COUNTER LIVES IN r4 AND CALLER-SAVES
 * ============================================================
 * The ROM carries SEVEN call-saved registers and puts the loop counter in r4:
 *     r9  = flag                 r8  = state
 *     r11 = ewram_2001002, unsigned
 *     r6  = ewram_2001002, signed -- later 0x6666
 *     r7  = arg -- later the angle
 *     r5  = state pointer -- later the slot, later the actor
 *     r10 = &.L3f6c -- later 0xffffcccd
 *     r4  = the loop counter
 * and because `-fcall-used-r4` makes r4 call-clobbered, every call inside a loop
 * is bracketed by `str r4, [sp]` / `ldr r4, [sp]`. There are NINE such pairs in
 * the reference and NONE in ours: that is essentially the whole excess, and it is
 * a DEFICIT of spills, not a surplus of anything else.
 *
 * Ours gives the counter a call-saved register and spills the UNSIGNED
 * ewram_2001002 value to the frame instead -- `sub sp, #0x8` with
 * `str r1, [sp, #4]` against the ROM's `sub sp, #0x4`. The two lowest-priority
 * allocnos have swapped places. This is the corollary in
 * src/non_matching/ovl_7ac2d8/200d5c0.c read in reverse: a caller-save
 * `str rN,[sp] / ldr rN,[sp]` pair in the ROM is a SPEC saying that allocno is
 * last in priority order.
 *
 * ============================================================
 * AND THE COMPILER SAYS OUR READING OF THE SOURCE CANNOT BE RIGHT
 * ============================================================
 * global.c's allocno_compare is
 *     floor_log2 (n_refs) * n_refs / live_length * size
 * -- NO FREQUENCY TERM, unlike the local-alloc formula quoted in batch 277's
 * notes. And find_reg (global.c:1149) only reaches a call-clobbered register on
 * a RETRY, which happens when no call-saved register was free AND
 *     CALLER_SAVE_PROFITABLE (REFS, CALLS) == (4 * CALLS < REFS)     regs.h:184
 * holds. So the allocno that lands in r4 is necessarily the LAST of the eight to
 * be processed, i.e. the LOWEST priority, and it must be reference-rich relative
 * to the calls it crosses.
 *
 * A counter has many references and a short range; the unsigned ewram byte has
 * TWO references and a function-wide range. Under that formula the byte is
 * always the lower priority -- and with refs=2 against a dozen calls crossed,
 * `4 * CALLS < REFS` is false for it, so it gets no register at all and spills.
 * WHICH IS EXACTLY WHAT OURS DOES. There is no spelling of "one byte read at the
 * top, used once in the tail" that produces the ROM's r11, so either the ROM's
 * source reads that byte MORE than twice, or the value in r11 is not that byte
 * but something computed from it with more uses. THAT is the open question, and
 * it is a question about the source, not about a lever.
 *
 * MEASURED (ref 377):
 *   one shared counter, byte read at the top          375 of 395
 *   FIVE separate counters (this file)                368 of 393
 *   five counters, byte read only in the tail         365 of 393  (best number,
 *                     but it deletes the ROM's `ldrb r2,[r3,#1]/mov r11,r2` at
 *                     the top, so it is a worse HYPOTHESIS for 3 encodings --
 *                     not adopted)
 *   five counters, byte referenced twice in the tail  368 of 393 (inert)
 *   0xffffcccd named, ewram reads block-scoped        inert on the count
 *   the ROM's coalescings written by hand (arg reused
 *     for the angle, the signed byte reused for
 *     0x6666, one variable for slot and actor)        377 of 403 -- WORSE, and
 *                     instructive: hand-coalescing does not reproduce what
 *                     global-alloc's conflict graph produces, it just deletes the
 *                     allocnos that were competing, which is the opposite of
 *                     raising pressure.
 *
 * NEXT: find the third and later reference to the unsigned ewram_2001002 byte,
 * or the derived value, that makes it out-prioritise a loop counter. Its one
 * known use is `(signed char)(((unsigned)(__Random() * 4) >> 16) + u + 1)` fed to
 * _modsi3_RAM; look for a second consumer in the `> 2` arm. Do NOT reach for a
 * per-file flag: the excess is all caller-save bookkeeping around calls, so
 * scheduling flags cannot produce or remove it.
 */
extern unsigned char *L3f6c __asm__(".L3f6c");
extern unsigned char ewram_2001001;
extern unsigned char ewram_2001000;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_808e118(void);
extern int __MessageID(int id);
extern int __ActorMessage(int a, int b);
extern char *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern int __MapActor_SetPos(int slot, int x, int z);
extern int __Random(void);
extern int _divsi3_RAM(int a, int b);
extern int _modsi3_RAM(int a, int b);
extern int __StartTask(void (*fn)(void), int prio);
extern int __StopTask(void (*fn)(void));
extern int __CutsceneWait(int n);
extern void __Func_8012330(int a, int b, int c);
extern void OvlFunc_957_2008f10(int slot, int b, int c);
extern void OvlFunc_957_2008f94(void);
extern void OvlFunc_957_200bad4(void);

void OvlFunc_957_200909c(int arg)
{
    unsigned char *p;
    short *e;
    char *a;
    int flag;
    int i1;
    int i2;
    int i3;
    int i4;
    int i5;
    int j;
    int s;
    int d;
    int u;
    int t;
    int v;
    int w;
    int c;
    int z;
    int k;
    int cc;

    flag = 0;
    __CutsceneStart();
    __Func_808e118();
    __MessageID(0x21db);
    __ActorMessage(0x10, 0);
    k = 0x80 << 9;
    __CutsceneEnd();
    z = 0;
    for (i1 = 0; i1 <= 4; i1++) {
        a = __MapActor_GetActor(i1 + 0xb);
        *(int *)(a + 0x6c) = z;
        *(int *)(a + 0x18) = k;
        *(int *)(a + 0x1c) = k;
    }
    {
        unsigned char *q = &ewram_2001001;
        s = q[0];
        e = *(short **)&L3f6c;
        d = *(signed char *)(q + 1);
        u = q[1];
    }
    e[3] = _divsi3_RAM(d << 16, 5) + (0x80 << 7);
    if ((signed char)s == 0) {
        if (arg == 0x10) {
            s = 1;
            __PlaySound(0x6e);
        } else {
            __PlaySound(0x72);
        }
        ewram_2001000 = 0;
    } else if ((signed char)s == 1) {
        if (arg == 0x10) {
            __PlaySound(0x6e);
        } else if (arg == 0x14) {
            s = 2;
            __PlaySound(0x6e);
            __WaitFrames(0x1e);
            t = *(short *)(*(char **)&L3f6c + 6);
            cc = 0xffffcccd;
            for (i2 = 0; i2 <= 4; i2++) {
                j = i2 + 0xb;
                OvlFunc_957_2008f10(j, 0xc0 << 13, (unsigned short)t);
                __PlaySound(0x97);
                a = __MapActor_GetActor(j);
                *(int *)(a + 0x18) = 0;
                v = 0x6666;
                do {
                    *(int *)(a + 0x1c) = v;
                    *(int *)(a + 0x18) = v;
                    __WaitFrames(1);
                    v += 0xc0 << 4;
                } while (*(int *)(a + 0x18) <= 0xffff);
                t = (short)((unsigned short)t + cc);
            }
            __WaitFrames(0x1e);
            flag = 1;
        } else {
            __PlaySound(0x72);
            s = 0;
        }
    } else if ((signed char)s == 2) {
        if (arg == d + 0x10) {
            __PlaySound(0x6e);
            flag = 1;
            __WaitFrames(0x1e);
        } else {
            s = 0;
            __PlaySound(0x72);
            __WaitFrames(0x1e);
            for (i3 = 0; i3 <= 4; i3++) {
                j = i3 + 0xb;
                a = __MapActor_GetActor(j);
                __PlaySound(0x97);
                v = *(int *)(a + 0x18);
                if (v > 0x6666) {
                    do {
                        *(int *)(a + 0x1c) = v;
                        *(int *)(a + 0x18) = v;
                        __WaitFrames(1);
                        v += 0xfffff400;
                    } while (*(int *)(a + 0x18) > 0x6666);
                }
                __MapActor_SetPos(j, 0, 0);
            }
        }
    }
    p = &ewram_2001001;
    *p = s;
    if (flag != 0) {
        c = p[-1] + 1;
        p[-1] = c;
        p[1] = _modsi3_RAM((signed char)(((unsigned int)(__Random() * 4) >> 16) + u + 1) + 5, 5);
        e = *(short **)&L3f6c;
        e[0] = 0;
        e[1] = 0;
        e[4] = 0x80 << 2;
        e[5] = 0xc0 << 6;
        __StartTask(OvlFunc_957_2008f94, 0xc8 << 4);
        if ((unsigned char)c <= 2) {
            while (**(short **)&L3f6c != 0x63)
                __WaitFrames(1);
            __WaitFrames(0xa);
            __PlaySound(0x6e);
        } else {
            *p = 0x63;
            while (**(short **)&L3f6c != 2)
                __WaitFrames(1);
            e = *(short **)&L3f6c;
            e[0] = 2;
            e[1] = 0;
            __Func_8012330(0x80 << 10, 0x80 << 10, 0x80 << 9);
            __CutsceneWait(0x14);
            __Func_8012330(0x80 << 11, 0x80 << 11, 0x80 << 9);
            e = *(short **)&L3f6c;
            e[0] = 0x63;
            __PlaySound(0xbe);
            w = 0xc0 << 13;
            t = *(short *)(*(char **)&L3f6c + 6);
            cc = 0xffffcccd;
            v = 0xc0 << 4;
            do {
                for (i4 = 0; i4 <= 4; i4++) {
                    j = i4 + 0xb;
                    a = __MapActor_GetActor(j);
                    *(int *)(a + 0x18) -= 0x10;
                    *(int *)(a + 0x1c) -= 0x10;
                    OvlFunc_957_2008f10(j, w, (unsigned short)t);
                    t = (short)((unsigned short)t + cc);
                }
                t = (short)((unsigned short)t + v);
                w += cc;
                __WaitFrames(1);
            } while (w > 0);
            for (i5 = 0; i5 <= 4; i5++)
                __MapActor_SetPos(i5 + 0xb, 0, 0);
            OvlFunc_957_200bad4();
            __PlaySound(0x50);
        }
        __StopTask(OvlFunc_957_2008f94);
    }
}
