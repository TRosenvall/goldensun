set -e
run() { # name cfile ref [cflags]
  printf '%-24s %-10s ' "$1" "${4:-default}"
  if [ -n "$4" ]; then
    ./scratch/agent2/scr.sh "$2" "$3" --full --align --cflags "$4" 2>&1 | grep -E "^  (OK|XX)|disagreeing" | tr '\n' '|'
  else
    ./scratch/agent2/scr.sh "$2" "$3" --full --align 2>&1 | grep -E "^  (OK|XX)|disagreeing" | tr '\n' '|'
  fi
  echo
}
