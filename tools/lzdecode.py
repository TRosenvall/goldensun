#!/usr/bin/env python3
"""lzdecode.py -- decompress a Golden Sun file-table blob.

    python3 tools/lzdecode.py file_table/file_108.raw [out.bin]
    python3 tools/lzdecode.py --survey          # classify every file_table blob

WHERE THE DATA LIVES.  Non-code bytes in this ROM sit in two places:

  1. THE FILE TABLE at ROM 0x320000.  asm/rom_320000/rom_320000.s builds
     `gFileTable` as a pointer table (`.word file_N`) followed by the blobs,
     via 957 `.ft_include` entries.  macros.inc:70 expands that to a plain
     `.incbin`, so the 860 `file_table/*.raw` files ARE the ROM's bytes -- and
     `make compare` passing is the proof.  Edit one and the ROM changes.
     The pointer table is assembler-computed, so a blob may change SIZE and the
     offsets follow; only 58 of the 957 entries pin a size with explicit
     trailing padding.
  2. `.incrom` BLOBS, 534 of them, pulling 1.82 MB straight out of
     baserom.gba by offset (macros.inc:42: `.incrom s,e` is
     `.incbin "baserom.gba", s, e-s`).  These are NOT extracted and cannot be
     edited without carving them out first.

THE FORMAT.  Byte 0 of a blob is a MODE TAG and the stream starts at byte 1.
Observed across the 860 blobs: 535 tag 0x00, 271 tag 0x02, 15 tag 0x01.
tools/pack_overlay.c is this project's COMPRESSOR (the build uses it to make
`overlays/*/overlay.lz`) and it writes tag 0 or tag 1 -- its header comment
documents both.  So:

  tag 0x01  classic LZSS, 4-bit length / 12-bit distance, optional 8-bit
            length.  IMPLEMENTED HERE, derived as the exact inverse of
            encode_mode1() at pack_overlay.c:359.
  tag 0x00  pack_overlay's "mode 0" -- a custom encoding optimised for shorter
            distances, bit-packed rather than byte-packed (see compress_mode0
            at :170, encode_mode0_length at :245, encode_mode0_distance at
            :271).  NOT IMPLEMENTED.  Derivable the same way.
  tag 0x02  NOT PRODUCED BY pack_overlay AT ALL, and measured not to be
            uncompressed either.  An unknown third format and the real
            research task.

NOTE pack_overlay also runs encode_thumb() first, rewriting BL offsets to be
absolute.  That is OVERLAY-CODE preprocessing and does not apply to data
blobs, so a decompressed data blob needs no inverse transform.

WHAT CAME OUT, and why these are graphics.  Every mode-1 blob decompresses to
an EXACT multiple of 32 bytes, which is one 4bpp GBA tile:

    file_108.raw   2,112 ->  6,912 = 216 tiles
    file_88.raw    4,040 -> 14,400 = 450 tiles
    file_198.raw   1,072 ->  3,456 = 108 tiles
    file_776.raw     228 ->    448 = 14 palettes of 16 BGR555 colours
                                     (0 of 224 entries set bit 15, as BGR555
                                      requires)

A GBA image is THREE things -- a tile sheet, a palette, and a tilemap saying
which tile goes where -- so "an image" is not one blob and an editor has to
deal with all three.

HOW TO VERIFY A FUTURE ENCODER.  Decompress, re-compress, and require the
original bytes back; then `make compare`.  The ROM hash is the authority, the
same discipline the C work uses.
"""
import glob
import os
import sys


def decode_mode1(b):
    """Exact inverse of pack_overlay.c encode_mode1(). `b` EXCLUDES the tag byte.

    Groups of 8 tokens, each preceded by a flags byte; bit (7-j) clear means
    token j is a literal.  A non-literal is 2 bytes -- `(dist>>8)<<4 | (len-1)`
    then `dist & 0xff` -- except that a zero low nibble means the 3-byte form
    with `len-17` in the third byte (because len-1 == 0 is unreachable: length
    one is spelled as a literal).  `dist == 0` with a zero nibble is the
    end-of-stream token, which the encoder writes as two zero bytes.
    """
    out = bytearray()
    i = 0
    while i < len(b):
        flags = b[i]
        i += 1
        for j in range(8):
            if i >= len(b):
                return bytes(out)
            if not (flags >> (7 - j)) & 1:
                out.append(b[i])
                i += 1
                continue
            if i + 1 >= len(b):
                return bytes(out)
            b0, b1 = b[i], b[i + 1]
            i += 2
            dist = ((b0 >> 4) << 8) | b1
            n = b0 & 0xf
            if n == 0:
                if dist == 0:
                    return bytes(out)
                length = b[i] + 17
                i += 1
            else:
                length = n + 1
            src = len(out) - dist
            if src < 0:
                raise ValueError(f"distance {dist} precedes output start at byte {i}")
            for k in range(length):
                out.append(out[src + k])
    return bytes(out)


DECODERS = {1: decode_mode1}


def decode(blob):
    """(mode, decompressed) for a file-table blob, tag byte included."""
    if not blob:
        raise ValueError("empty blob")
    mode = blob[0]
    if mode not in DECODERS:
        raise NotImplementedError(
            f"mode 0x{mode:02x} has no decoder yet -- see this file's docstring")
    return mode, DECODERS[mode](blob[1:])


def survey():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    files = sorted(glob.glob(os.path.join(root, "file_table", "file_*.raw")))
    done = todo = 0
    print(f"{len(files)} file-table blobs\n")
    print(f"{'blob':<22} {'mode':>5} {'packed':>8} {'unpacked':>9} {'/32':>8}")
    for f in files:
        blob = open(f, "rb").read()
        if len(blob) < 8:
            continue
        name = os.path.basename(f)
        if blob[0] not in DECODERS:
            todo += 1
            continue
        try:
            _, d = decode(blob)
        except Exception as e:
            print(f"{name:<22} {blob[0]:>5} {len(blob):>8}  FAILED: {e}")
            continue
        done += 1
        # 32 bytes is one 4bpp tile AND one 16-colour BGR555 palette, so the
        # count is reported WITHOUT claiming which.  file_776 is 448 bytes =
        # 14 palettes, not 14 tiles; only the consumer knows.
        exact = "" if len(d) % 32 else f"{len(d)//32} x 32B"
        print(f"{name:<22} 0x{blob[0]:02x} {len(blob):>8} {len(d):>9} {exact:>12}")
    print(f"\ndecoded {done}; {todo} blobs use a mode with no decoder yet "
          f"(modes 0x00 and 0x02 -- see the docstring)")


def main():
    if "--survey" in sys.argv:
        return survey()
    if len(sys.argv) < 2:
        sys.stderr.write(__doc__.split("\n\n")[1] + "\n")
        return 2
    blob = open(sys.argv[1], "rb").read()
    mode, d = decode(blob)
    print(f"{sys.argv[1]}: mode 0x{mode:02x}, {len(blob)} -> {len(d)} bytes "
          f"({len(d)/max(len(blob),1):.2f}x)"
          + (f"  = {len(d)//32} x 32B units (a 4bpp tile or a 16-colour palette"
             f" are both 32 bytes -- this does not say which)"
             if len(d) % 32 == 0 else ""))
    if len(sys.argv) > 2:
        open(sys.argv[2], "wb").write(d)
        print(f"wrote {sys.argv[2]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
