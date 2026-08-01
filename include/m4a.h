#ifndef GUARD_M4A_H
#define GUARD_M4A_H

#include "types.h"

/* rom_f9000 is Nintendo's M4A sound driver.  See the module header in
 * rom_f9080.s for the evidence -- the 'Smsh' identifier, the 0xB1 command base
 * and the verified 36-entry jump table.
 *
 * Only fields this project has actually established are named; the rest is
 * padding.  The names follow the published M4A ones where the behaviour
 * matches, so anyone who knows that driver will recognise them.
 */

/* One instrument.  The extended commands (0xCD) overwrite individual fields of
 * this in place, which is how a song alters an instrument without changing
 * which instrument is selected. */
typedef struct ToneData
{
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 key;
    /* 0x02 */ u8 length;
    /* 0x03 */ u8 panSweep;
    /* 0x04 */ u32 wav;
    /* 0x08 */ u8 attack;
    /* 0x09 */ u8 decay;
    /* 0x0A */ u8 sustain;
    /* 0x0B */ u8 release;
} ToneData;

typedef struct MusicPlayerTrack
{
    /* 0x00 */ u8 flags;        /* bits 0-1 volume/pan changed,
                                   bits 2-3 pitch changed            */
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 patternLevel; /* pattern stack depth, max 3        */
    /* 0x03 */ u8 repN;         /* repeat counter                    */
    /* 0x04 */ u8 unk_04[3];
    /* 0x07 */ u8 runningStatus;
    /* 0x08 */ u8 unk_08[2];
    /* 0x0A */ s8 keyShift;
    /* 0x0B */ u8 unk_0B;
    /* 0x0C */ s8 tune;
    /* 0x0D */ u8 unk_0D;
    /* 0x0E */ s8 bend;
    /* 0x0F */ u8 bendRange;
    /* 0x10 */ u8 volMR;        /* the two mixer levels Func_fac44   */
    /* 0x11 */ u8 volML;        /* rebuilds from vol and pan         */
    /* 0x12 */ u8 vol;
    /* 0x13 */ u8 unk_13;
    /* 0x14 */ s8 pan;
    /* 0x15 */ u8 unk_15;
    /* 0x16 */ s8 modM;         /* modulation accumulator            */
    /* 0x17 */ u8 modDepth;
    /* 0x18 */ u8 modType;      /* 0 pitch, 1 volume, 2 pan          */
    /* 0x19 */ u8 lfoSpeed;
    /* 0x1A */ u8 unk_1A;
    /* 0x1B */ u8 lfoDelay;
    /* 0x1C */ u8 unk_1C;
    /* 0x1D */ u8 priority;
    /* 0x1E */ u8 echoVolume;
    /* 0x1F */ u8 echoLength;
    /* 0x20 */ void *chan;      /* head of this track's channel list */
    /* 0x24 */ ToneData tone;
    /* 0x30 */ u8 unk_30[0x10];
    /* 0x40 */ u8 *cmdPtr;      /* the script cursor                 */
    /* 0x44 */ u8 *patternStack[3];
} MusicPlayerTrack;

#endif /* GUARD_M4A_H */
