#!/usr/bin/env python3
"""funcindex.py -- resolve a function to the file that currently defines it.

    python3 tools/funcindex.py --stats
    python3 tools/funcindex.py OvlFunc_968_200c2bc
    python3 tools/funcindex.py --park src/non_matching/ovl_7f2f14/200c2bc.c

Also importable:

    from funcindex import resolve, park_ref, strip_comments, real_pins

WHY THIS EXISTS. Three separate defects, all of them RESOLUTION defects rather
than storage defects, and all of them measured rather than assumed:

1. THE PARK CORPUS IS MOSTLY INVISIBLE TO ITS OWN TOOLING. close_parks.py finds
   a park's reference with

       re.compile(r"Source asm:\\s*goldensun/(\\S+\\.s)")

   i.e. it requires one exact English phrase. Measured over 521 park files:

       141  match that phrasing            <- the tool's entire input
        75  ...and the path still exists   <- what it actually screens
       286  name an asm path in OTHER prose <- invisible to it
        94  name no path at all

   So the dominant cause is PROSE FORMAT, not staleness. rank_parks.py reports
   the same shape ("445 skipped, no reference"). Parking notes are written by
   hand in free prose and always will be; a tool that parses that prose will
   always see a minority of them.

   The fix is to stop parsing prose. A park's SUBJECT is knowable from its
   filename (200c2bc.c) or its header, and the file that currently defines that
   function is knowable by looking. Resolve, do not remember.

2. A RECORDED PATH GOES STALE THE MOMENT ITS .s IS SPLIT. 167 of the 427 parks
   naming an asm path name one that no longer exists, because split_s.py renamed
   it. docs/elevation.md:4326 records this ("a worklist ref path goes stale the
   moment its .s is split") and the remedy has been to re-record the path by
   hand, which does not scale and silently rots.

3. FINDING A FUNCTION COST FIVE MINUTES. A recursive grep over asm/ for one
   .thumb_func_start takes long enough that a 17-function loop timed out twice
   in one session. The index below builds once and caches.

AND A FOURTH, SMALLER ONE. Ad-hoc greps for `register ... __asm__("r` to decide
whether a landing needs a fakematch.txt row COUNT MATCHES INSIDE HEADER
COMMENTS, because elevated files document their lever tables in prose there. One
file showed three apparent pins that were all documentation. An audit of 80 rows
found no spurious entry, so it happened not to cost anything -- but the check was
wrong and every caller was repeating it. strip_comments()/real_pins() below are
the shared correct primitive.

WHAT THIS IS NOT. It is deliberately not a database. Every defect above is a
parsing or resolution bug; re-encoding the same bad inputs into SQLite would fix
none of them and would add a schema to keep in sync with the tree. The index is a
cache of something derivable, and is safe to delete at any time.
"""
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CACHE = os.path.join(ROOT, "tools", ".funcindex.json")

# Hand-written disassembly marks functions with .thumb_func_start/.arm_func_start.
# gcc-generated .s (the sibling of an elevated .c) marks them with .type NAME,function
# -- note gcc emits a space after the directive, hence \s+ rather than a literal tab.
HAND = re.compile(r"^\s*\.(?:thumb|arm)_func_start(?:_noalign)?\s+(\S+)", re.M)
GEN = re.compile(r"^\s*\.type\s+(\S+?)\s*,\s*function", re.M)


def _walk():
    """Yield every asm/**/*.s path, relative to ROOT."""
    base = os.path.join(ROOT, "asm")
    for dirpath, _dirnames, filenames in os.walk(base):
        for fn in filenames:
            if fn.endswith(".s"):
                p = os.path.join(dirpath, fn)
                yield os.path.relpath(p, ROOT)


def _newest():
    """Newest mtime across the inputs the index derives from.

    Walking is far cheaper than parsing, so this is the invalidation test: if
    nothing under asm/ or src/ is newer than the cache, the cache is good.
    """
    newest = 0.0
    for base in ("asm", "src"):
        for dirpath, _d, filenames in os.walk(os.path.join(ROOT, base)):
            for fn in filenames:
                if fn.endswith((".s", ".c")):
                    try:
                        m = os.stat(os.path.join(dirpath, fn)).st_mtime
                    except OSError:
                        continue
                    if m > newest:
                        newest = m
    return newest


def _build():
    """{name: {"asm": <path>, "src": <path or None>, "kind": "c"|"asm"}}"""
    out = {}
    for rel in _walk():
        try:
            with open(os.path.join(ROOT, rel), errors="replace") as fh:
                text = fh.read()
        except OSError:
            continue
        src = "src/" + rel[len("asm/"):-2] + ".c"
        elevated = os.path.exists(os.path.join(ROOT, src))
        names = set(HAND.findall(text)) | set(GEN.findall(text))
        for n in names:
            # A name should live in exactly one file. If two claim it, prefer the
            # elevated one -- that is the file the build actually compiles.
            if n in out and out[n]["kind"] == "c":
                continue
            out[n] = {"asm": rel,
                      "src": src if elevated else None,
                      "kind": "c" if elevated else "asm"}
    return out


def index(force=False):
    """The function map, from cache when the cache is still valid."""
    newest = _newest()
    if not force and os.path.exists(CACHE):
        try:
            with open(CACHE) as fh:
                blob = json.load(fh)
            if blob.get("newest", -1) >= newest:
                return blob["map"]
        except (OSError, ValueError, KeyError):
            pass
    m = _build()
    try:
        with open(CACHE, "w") as fh:
            json.dump({"newest": newest, "map": m}, fh)
    except OSError:
        pass                      # a read-only tree is fine; just do not cache
    return m


def resolve(name, force=False):
    """Where is this function defined RIGHT NOW? None if nowhere."""
    return index(force).get(name)


def strip_comments(text):
    """C source minus comments.

    Elevated files and park notes document their lever tables in the header
    comment, so any grep for a construct must run on the body or it counts
    documentation as code.
    """
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    return re.sub(r"//[^\n]*", "", text)


def real_pins(path):
    """Count `register T x __asm__("rN")` declarations OUTSIDE comments."""
    try:
        with open(path, errors="replace") as fh:
            body = strip_comments(fh.read())
    except OSError:
        return 0
    return len(re.findall(r'register\s[^;=]*__asm__\s*\(\s*"r', body))


ADDR = re.compile(r"([0-9a-f]{6,8})\.c$")
# Deliberately broad: the library functions carry lowercase names (free, gfree,
# ply_pend) and an earlier capitalised-only pattern silently missed all of them.
# Breadth is safe because every candidate is checked against the index.
NAME = re.compile(r"\b([A-Za-z_]\w{2,})\b")


def park_subject(park, force=False):
    """The function a park file is about, resolved against the live index.

    Filename first -- src/non_matching/ovl_7f2f14/200c2bc.c is about the
    function whose name ends _200c2bc. Class parks (arg_interleave_flat.c,
    common1_78.c) carry no address, so fall back to the first name in the
    header that the index actually knows.
    """
    idx = index(force)
    m = ADDR.search(os.path.basename(park))
    if m:
        suffix = "_" + m.group(1)
        hits = [n for n in idx if n.endswith(suffix)]
        # OVERLAYS SHARE ADDRESS SPACE, so an address alone is ambiguous -- one
        # park address here has THIRTEEN candidates across thirteen overlays.
        # The park's own directory names the overlay (src/non_matching/ovl_799abc/
        # -> asm/overlays/rom_799abc/), and that is what disambiguates. Without
        # it the resolver silently returns a same-address function from an
        # unrelated overlay.
        ovl = re.search(r"/ovl_([0-9a-f]+)/", park)
        if ovl and len(hits) > 1:
            want = "/rom_%s/" % ovl.group(1)
            scoped = [n for n in hits if want in idx[n]["asm"]]
            if scoped:
                hits = scoped
        if len(hits) == 1:
            return hits[0]
        if hits:
            # Still ambiguous: prefer one not yet elevated, since a park's
            # subject is by definition unsolved.
            unel = [n for n in hits if idx[n]["kind"] == "asm"]
            return (unel or hits)[0]
    try:
        with open(park, errors="replace") as fh:
            head = fh.read(4000)
    except OSError:
        return None
    # Prefer a name the index still has in asm/: a park's subject is by
    # definition unelevated, so an elevated hit is almost always a CALLEE named
    # in the prose rather than the function the note is about.
    fallback = None
    for cand in NAME.findall(head):
        ent = idx.get(cand)
        if not ent:
            continue
        if ent["kind"] == "asm":
            return cand
        if fallback is None:
            fallback = cand
    return fallback


def park_ref(park, force=False):
    """The .s that currently holds a park's subject. None if it cannot be found.

    This is the replacement for parsing "Source asm: goldensun/<path>" out of
    the note. It cannot go stale, because it is recomputed.
    """
    subj = park_subject(park, force)
    if not subj:
        return None
    ent = index(force).get(subj)
    return ent["asm"] if ent else None


def main():
    args = sys.argv[1:]
    force = "--force" in args
    args = [a for a in args if a != "--force"]

    if not args or args[0] == "--stats":
        idx = index(force)
        c = sum(1 for v in idx.values() if v["kind"] == "c")
        print("%d functions indexed  (%d from C, %d still in asm)"
              % (len(idx), c, len(idx) - c))
        print("cache: %s" % (CACHE if os.path.exists(CACHE) else "(not written)"))
        return 0

    if args[0] == "--park":
        ok = miss = 0
        for p in args[1:]:
            r = park_ref(p, force)
            print("%-52s %s" % (os.path.relpath(p, ROOT), r or "UNRESOLVED"))
            ok, miss = (ok + 1, miss) if r else (ok, miss + 1)
        if len(args) > 2:
            print("\n%d resolved, %d unresolved" % (ok, miss))
        return 1 if miss else 0

    for name in args:
        e = resolve(name, force)
        if not e:
            print("%-32s NOT FOUND" % name)
        else:
            print("%-32s %-8s %s" % (name, e["kind"], e["src"] or e["asm"]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
