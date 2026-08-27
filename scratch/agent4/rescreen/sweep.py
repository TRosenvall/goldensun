import json, os, re, subprocess, sys
ROOT='/work'
sel=json.load(open('/work/scratch/agent4/rescreen/sel.json'))
out=[]
for r in sel:
    p=r['path']; ref=r['ref']
    if not ref:
        out.append((p,None,'NO-REF-IN-HEADER')); continue
    if not os.path.exists(os.path.join(ROOT,ref)):
        out.append((p,ref,'REF-MISSING')); continue
    cmd=['python3','tools/tryc.py',p,'--ref',ref,'--quiet']
    try:
        res=subprocess.run(cmd,cwd=ROOT,capture_output=True,text=True,timeout=120)
    except Exception as e:
        out.append((p,ref,'TIMEOUT')); continue
    out.append((p,ref,(res.stdout+res.stderr).strip()))
for p,ref,s in out:
    print('###',p,'|',ref)
    print(s)
