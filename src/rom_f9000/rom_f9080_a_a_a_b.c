/* UpdateMusicSettings(void) BGM volume/tempo/pitch slide updater [rom_f9000]
 *
 * MATCHING -- 0 of 121 encodings, 276 bytes, 16 relocations identical.
 *   (batch 346 brief A.  Was 112 of 121; ref 103 instructions, ours 102 --
 *   ONE missing instruction, so that figure was measuring phase offset.)
 *
 * THE MISSING INSTRUCTION was `mov r2, r3` (encoded 1c1a, `adds r2, r3, #0`)
 * right after `ldrb r3, [r1]` at the top: the ROM holds the ewram_2003000 byte
 * in TWO pseudos -- r3, which the `else` arm decrements (`adds r3, #255`), and
 * r2, which both compares read.  The parked body used a local
 * (`state = ewram_2003000;`) and therefore had exactly one pseudo, so gcc
 * compared the load's own destination and no copy existed.
 *
 * THE CURE IS TO DELETE THE LOCAL.  Reading the global directly three times
 * and writing the decrement as `ewram_2003000--;` gives cse a second and third
 * occurrence of the same MEM; it replaces the second with a register copy
 * (cse.c's `(set REG0 REG1)` handling, cse.c:5959-6005 -- the swap path whose
 * gate at :5992-5994 requires prev_nonnote_insn of the copy to be the insn
 * that set the source, which here it is), and the copy survives because the
 * original pseudo is still live into the `else` arm.  That is the ROM's
 * `mov r2, r3`.  Net effect: fewer source-level names, MORE instructions.
 *
 * MEASURED DEAD ENDS (all four of these are BIT-IDENTICAL to the parked body
 * at 112 of 121 / 102 instructions -- they are one program, not four):
 *   - switch (state) with cases 0, 1, default
 *   - switch (ewram_2003000) with a u8 local kept for the arithmetic
 *   - a separate `int s = state;` for the compares, u8 for the arithmetic
 *   - `int state` with `ewram_2003000 = ewram_2003000 - 1;`
 *   - `ewram_2003000 = state + 0xff;` instead of `- 1`
 *   - `s = state & 0xff;` with a u8 local
 * Two variants reached 1 of 121 (103/103 instructions, aligned 120 of 121):
 *   - `int s = ewram_2003000;` with both compares on `(s & 0xff)` and
 *     `ewram_2003000 = s - 1;`  -- combine's simplify_and_const_int drops the
 *     AND on nonzero_bits(r3) == 0xff and leaves the move, so the COUNT is
 *     right; the single residue is the register pair, not the shape.
 *   - the same with `switch (s & 0xff)`.
 * Keeping the local in ANY spelling costs the instruction; the global-read
 * spelling is the only one that produces it.
 *
 * SPLIT REQUIRED.  asm/rom_f9000/rom_f9080_a_a_a.s holds three functions and
 * the other two are still parked (PlaySound 141 of 151, Debug_SoundTest 134 of
 * 146), so this ships by a 3-way cut.  datacheck.py reports NO data in the .s
 * and piece.py reports NO non-function exports, so the cut is code-only:
 *   python3 tools/split_s.py asm/rom_f9000/rom_f9080_a_a_a.s UpdateMusicSettings
 *     -> _a.s (PlaySound, 197 lines)
 *        _b.s (UpdateMusicSettings, 133 lines)   <- replaced by this .c
 *        _c.s (Debug_SoundTest, 163 lines)
 * Installed path: src/rom_f9000/rom_f9080_a_a_a_b.c
 * PINS: 0.  DEVICES: 0.
 *
 * Verify with (against the UNSPLIT .s, which is what exists before install):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_f9000/rom_f9080_a_a_a_b.c \
 *     asm/rom_f9000/rom_f9080_a_a_a.s --func UpdateMusicSettings
 * After the split the same recipe against asm/rom_f9000/rom_f9080_a_a_a_b.s.
 *
 * Per-frame updater: ages a fade counter, then eases the current BGM volume
 * (2003008) toward its target (2003034) and the current tempo (2003038)
 * toward its target (2003030), each by a per-frame step, clamping on
 * overshoot (sign-flip of the target-current delta). Pitch is derived from
 * the tempo. Closes by ticking the sound engine.
 */
extern unsigned char  ewram_2003000;
extern unsigned char  gMPlayInfo_02004210[];
extern short          gMusicCurVolume;   /* current volume */
extern short          gMusicVolume;   /* target  volume */
extern unsigned short gMusicVolumeDelta;   /* volume step     */
extern short          gMusicCurSpeed;   /* current tempo   */
extern short          gMusicSpeed;   /* target  tempo   */
extern unsigned short gMusicSpeedDelta;   /* tempo  step     */
extern void *gMPlayInfo_BGM;

extern void m4aMPlayVolumeControl(void *mplayInfo, unsigned short trackBits, unsigned short volume);
extern void m4aMPlayTempoControl(void *mplayInfo, unsigned short tempo);
extern void m4aMPlayPitchControl(void *mplayInfo, unsigned short trackBits, int pitch);
extern void m4aSoundVSync(void);

void UpdateMusicSettings(void) {
    int diff;

    if (ewram_2003000 != 0) {
        if (ewram_2003000 == 1) {
            if (gMPlayInfo_02004210[4] == 0) {
                ewram_2003000 = 0;
                gMusicVolume = 0x100;
            }
        } else {
            ewram_2003000--;
        }
    }

    if (gMusicVolume != gMusicCurVolume) {
        diff = gMusicVolume - gMusicCurVolume;
        if (diff > 0)
            gMusicCurVolume = (unsigned short)gMusicCurVolume + gMusicVolumeDelta;
        else
            gMusicCurVolume = (unsigned short)gMusicCurVolume - gMusicVolumeDelta;
        if (((gMusicVolume - gMusicCurVolume) ^ diff) < 0)
            gMusicCurVolume = gMusicVolume;
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xff, (unsigned short)gMusicCurVolume);
    }

    if (gMusicSpeed != gMusicCurSpeed) {
        diff = gMusicSpeed - gMusicCurSpeed;
        if (diff > 0)
            gMusicCurSpeed = (unsigned short)gMusicCurSpeed + gMusicSpeedDelta;
        else
            gMusicCurSpeed = (unsigned short)gMusicCurSpeed - gMusicSpeedDelta;
        if (((gMusicSpeed - gMusicCurSpeed) ^ diff) < 0)
            gMusicCurSpeed = gMusicSpeed;
        m4aMPlayTempoControl(&gMPlayInfo_BGM, (unsigned short)gMusicCurSpeed);
        m4aMPlayPitchControl(&gMPlayInfo_BGM, 0xff,
                     (((3 * gMusicCurSpeed) << 18) + (0xf4 << 24)) >> 16);
    }

    m4aSoundVSync();
}
