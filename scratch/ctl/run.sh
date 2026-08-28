set -e
n=0
while read f; do
  o=/work/scratch/ctl/$(echo "$f" | tr '/' '_' | sed 's/\.c$/.s/')
  /opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
     -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -I/work/include \
     -S -o "$o" "/work/$f" 2>/dev/null && n=$((n+1)) || true
done < /work/scratch/ctl/list.txt
echo "compiled $n"
