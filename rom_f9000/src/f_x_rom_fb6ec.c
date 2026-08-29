/* Func_fb6ec .. Func_fb77c -- the extended-command (0xCD) byte setters
 *
 * Each takes one byte from the track's command stream and writes it into a
 * single field of the track. Together they let a song override one field of an
 * instrument without re-selecting the instrument, which is why they map onto
 * the ToneData record at track+0x24.
 *
 * STATUS: ALL NINE MATCHING.
 *
 * The register pins are matching aids. The original loads the operand byte
 * BEFORE computing the destination address; agbcc reverses that. Which of r0
 * and r2 holds which also varies between them, so the pins differ per function
 * -- that is the original's allocation, not a style choice. The two whose
 * destination offset is small enough for a strb immediate (echoVolume at +0x1E
 * and echoLength at +0x1F) need no pins at all.
 */
#include "types.h"
#include "m4a.h"

void Func_fb6ec(void *player, MusicPlayerTrack *t)
{
    int v;
    u8 *d;

    v = *t->cmdPtr;
    d = &t->tone.type;
    *d = v;

    t->cmdPtr = t->cmdPtr + 1;
}

void Func_fb700(void *player, MusicPlayerTrack *t)
{
    int v;
    u8 *d;

    v = *t->cmdPtr;
    d = &t->tone.attack;
    *d = v;

    t->cmdPtr = t->cmdPtr + 1;
}

void Func_fb714(void *player, MusicPlayerTrack *t)
{
    int v;
    u8 *d;

    v = *t->cmdPtr;
    d = &t->tone.decay;
    *d = v;

    t->cmdPtr = t->cmdPtr + 1;
}

void Func_fb728(void *player, MusicPlayerTrack *t)
{
    int v;
    u8 *d;

    v = *t->cmdPtr;
    d = &t->tone.sustain;
    *d = v;

    t->cmdPtr = t->cmdPtr + 1;
}

void Func_fb73c(void *player, MusicPlayerTrack *t)
{
    int v;
    u8 *d;

    v = *t->cmdPtr;
    d = &t->tone.release;
    *d = v;

    t->cmdPtr = t->cmdPtr + 1;
}

void Func_fb750(void *player, MusicPlayerTrack *t)
{
    int v;
    u8 *d;

    v = *t->cmdPtr;
    d = &t->echoVolume;
    *d = v;

    t->cmdPtr = t->cmdPtr + 1;
}

void Func_fb75c(void *player, MusicPlayerTrack *t)
{
    int v;
    u8 *d;

    v = *t->cmdPtr;
    d = &t->echoLength;
    *d = v;

    t->cmdPtr = t->cmdPtr + 1;
}

void Func_fb768(void *player, MusicPlayerTrack *t)
{
    int v;
    u8 *d;

    v = *t->cmdPtr;
    d = &t->tone.length;
    *d = v;

    t->cmdPtr = t->cmdPtr + 1;
}

void Func_fb77c(void *player, MusicPlayerTrack *t)
{
    int v;
    u8 *d;

    v = *t->cmdPtr;
    d = &t->tone.panSweep;
    *d = v;

    t->cmdPtr = t->cmdPtr + 1;
}
