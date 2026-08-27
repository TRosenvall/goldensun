set -u
while IFS='|' read -r p n r; do
  [ -z "$r" ] && continue
  echo "### $p | $n | $r"
  python3 tools/tryc.py "$p" --ref "$r" --quiet 2>&1
done < /work/scratch/agent4/rescreen/pairs.txt
