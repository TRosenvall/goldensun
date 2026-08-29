import sys, re
t = open('/work/' + sys.argv[1]).read()
fn = sys.argv[2]
m = re.search(r'^\.thumb_func_start ' + re.escape(fn) + r'\b.*$', t, re.M)
i = m.start()
j = t.index('.func_end ' + fn) + len('.func_end ' + fn)
print('\t.include "macros.inc"')
print(t[i:j])
