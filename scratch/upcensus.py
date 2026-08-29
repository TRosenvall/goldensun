p='tools/census.py'; s=open(p).read()
s=s.replace('''                   rule held on every function tested, but the test here is
                   crude and OVER-counts. MEASURED: of the 3495 functions whose
                   C ALREADY MATCHES, this filter would flag 526 -- 15.1%. So
                   this line is an UPPER BOUND with a known false-positive
                   rate, not a count of unreachable functions, and it should
                   never be quoted as "N functions cannot be matched".''',
'''                   rule is implemented STRICTLY: two or more argument
                   registers built by an expensive operation, plus a cheap
                   `mov rN, #K` that is not the last setup line.

                   MEASURED against the corpus that already matches: 84 of 3495
                   -- 2.4% false positives. An earlier, looser version of this
                   test (any cheap mov not last, one expensive value enough)
                   flagged 526 of 3495, 15.1%, and inflated this line by about
                   130 functions. The loose version also flagged single-argument
                   calls like `__SetFlag(0xc0 << 2)`, where there is no ordering
                   question at all.

                   Even strictly, 84 matching functions have this shape, so it
                   is not absolute -- see OvlFunc_965_200919c, which passes four
                   arguments with two of them shifted and matches.''')
s=s.replace('''BLOCK = re.compile(r"((?:\\t(?:mov|lsl|neg|ldr|add|sub)\\t[^\\n]*\\n)+)\\tbl\\t")''',
'''BLOCK = re.compile(r"((?:\\t(?:mov|lsl|neg|ldr|add|sub)\\t[^\\n]*\\n)+)\\tbl\\t")
CHEAP = re.compile(r"\\tmov\\t(r[0-3]), #")
EXP = re.compile(r"\\t(?:lsl|neg)\\t(r[0-3])|\\tldr\\t(r[0-3]), =")''')
s=s.replace('''def precompute(body):
    for m in BLOCK.finditer(body):
        lines = [l for l in m.group(1).split("\\n") if l]
        if not any(l.startswith("\\tlsl\\t") or l.startswith("\\tneg\\t") or "=" in l
                   for l in lines):
            continue
        for l in lines[:-1]:
            if re.match(r"\\tmov\\tr\\d+, #", l):
                return True
    return False''',
'''def precompute(body):
    """HANDOFF.md's rule, strictly: TWO OR MORE expensive argument values plus a
    cheap constant that is not last. The "two or more" matters -- a call with a
    single shifted argument is a different and smaller problem, and lumping the
    two together is what inflated this class."""
    for m in BLOCK.finditer(body):
        lines = [l for l in m.group(1).split("\\n") if l]
        exp, cheap = set(), []
        for i, l in enumerate(lines):
            e = EXP.search(l)
            if e:
                exp.add(e.group(1) or e.group(2))
            if CHEAP.match(l):
                cheap.append(i)
        if len(exp) >= 2 and cheap and cheap[-1] != len(lines) - 1:
            return True
    return False''')
open(p,'w').write(s)
