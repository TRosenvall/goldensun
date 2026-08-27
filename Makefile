# Default target. Verify the checksums of the built ROM and overlays.

ROM := goldensun.gba
OVERLAYS := $(patsubst %.ld,%.bin,$(wildcard overlays/*/overlay.ld))

.PHONY: compare compare-rom compare-overlays
compare: compare-rom compare-overlays

compare-rom: goldensun.sha1 $(ROM)
	sha1sum -c $<

COMPARE_OVERLAYS := $(OVERLAYS:%/overlay.bin=compare-%)

compare-overlays: $(COMPARE_OVERLAYS)

$(COMPARE_OVERLAYS): compare-%: %/orig.bin %/overlay.bin
	cmp $*/orig.bin $*/overlay.bin

# Empty clean target. Recipes will be added below.
.PHONY: clean
clean::

# The ROM image includes compressed code overlays.
# The overlays reference symbols defined in the main executable.
# We partially link the main executable; build the overlays against it;
# compress the overlays; and then link the final image.

ARM_LDFLAGS :=
ARM_LDLIBS :=

# Partially linked relocatable object
STAGE1 := stage1.o
$(STAGE1): %.o: %.ld
$(STAGE1): ARM_LDFLAGS += -r

# Overlays reference symbols defined in main code
OVERLAY_ELFS := $(OVERLAYS:.bin=.elf)
$(OVERLAY_ELFS): %.elf: %.ld $(STAGE1)
$(OVERLAY_ELFS): ARM_LDLIBS += -R $(STAGE1)

# Final fully linked executable
ELF := $(ROM:.gba=.elf)
$(ELF): %.elf: %.ld

# All of the above
ELFS := $(STAGE1) $(ELF) $(OVERLAY_ELFS)
$(ELFS):
	arm-none-eabi-ld $(ARM_LDFLAGS) -T $< $(ARM_LDLIBS) -Map $(<:.ld=.map) -o $@

# Read dependencies from the linker scripts
define elf_deps
$(1): $(shell grep -o '[A-Za-z0-9/_-]\+\.o' $(addsuffix .ld,$(basename $(1))))
endef
$(foreach elf,$(ELFS),$(eval $(call elf_deps,$(elf))))

# ...and from the symbol files they INCLUDE. Those are not `.o` names, so the
# rule above cannot see them: editing message.sym left stage1.o stale, and the
# overlay that referenced the new symbol died with
#
#     undefined reference to `_MSG_256c'
#
# which reads exactly like a typo in the C rather than a stale object. The
# assignments in a .sym become symbols in the partially-linked stage1.o, and
# every overlay picks them up from there with `-R stage1.o`.
define ld_sym_deps
$(1): $(shell sed -n 's/^INCLUDE "\(.*\)"/\1/p' $(addsuffix .ld,$(basename $(1))))
endef
$(foreach elf,$(ELFS),$(eval $(call ld_sym_deps,$(elf))))


# Convert executables to free-standing binaries
$(ROM) $(OVERLAYS):
	arm-none-eabi-objcopy -O binary $< $@

$(ROM): %.gba: %.elf

$(OVERLAYS): %.bin: %.elf


# Assemble ARM code and generate dependencies
#
# The sed is load-bearing. A gcc-generated .s carries `.file "dummy.c"`, and
# `as -MD` records that name as a dependency exactly as written -- with no
# directory component. The resulting .d says
#
#     src/rom_b0000/dummy.o: dummy.c src/rom_b0000/dummy.s
#
# and the next build that needs that .o dies with
#
#     No rule to make target 'dummy.c', needed by 'src/rom_b0000/dummy.o'
#
# It only bites after a `make clean`, which is why it reads as a clean-build
# problem. Directory-less .c entries are never real dependencies -- the .s
# beside them already is -- so they are dropped.
%.o: %.s
	arm-none-eabi-as -mcpu=arm7tdmi -Iinclude -MD $(@:.o=.d) -o $@ $<
	@sed -E -i 's, [^ /]+\.c( |$$),\1,g' $(@:.o=.d)

# Compile target C with the patched gcc-2.96 build from the camelot-gcc
# submodule (install via camelot-gcc/install-296.sh). Produces byte-identical
# output to Camelot's original compiler (see compiler.md).
# Pipeline: xgcc -S (driver internal cpp -> cc1) -> trailing .align -> as.
# Karathan's -fcall-used-r4 flag is required for byte match. -ffixed-r7 is
# NOT needed under gcc-2.96; the compiler naturally avoids r7 for the same
# allocation patterns Camelot did. Trailing .align 2, 0 is required because
# gcc emits .align with zero-fill BETWEEN functions (via the elf.h patch)
# but NOT AFTER the last function in a TU, so the assembler's default
# Thumb-nop fill leaks in without this explicit append.
GCC296_DIR     ?= tools/gcc296
GCC296_CC      := $(GCC296_DIR)/xgcc
GCC296_CFLAGS  := -B$(GCC296_DIR)/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
                  -fno-builtin -nostdinc -ffreestanding \
                  -fcall-used-r4 -Iinclude

%.o: %.c
	$(GCC296_CC) $(GCC296_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

# Cross-dir rule: build asm/<bank>/X.o from src/<bank>/X.c. Used by the
# split-multifn workflow (tools/split_multifn_s.py); matched .c source-of-
# truth lives at src/<bank>/X.c per the 3.5c layout, but the linker keeps
# referencing asm/<bank>/X.o. Generates asm/<bank>/X.s as a build
# intermediate alongside the .o; safe to commit per the existing matched-
# corpus convention, or leave as a build artifact (regenerable from the .c).
asm/%.o: src/%.c
	$(GCC296_CC) $(GCC296_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

# overlays/common/common2_c was compiled WITHOUT -mthumb-interwork in the
# original ROM: all 14 of its functions return `pop {pc}` (the non-interwork
# epilogue), unique in the corpus; every other TU returns `bx`-form. Drop
# interwork for this one stem so the epilogue byte-matches. Pattern (not explicit)
# so it also covers the splitter's matched _b children (common2_c_b.o, ...). Mirrors
# the src/lib/m4a/%.o per-file override precedent below. Verified: a common2 fn compiled
# without -mthumb-interwork emits `pop {pc}`.
COMMON2_CFLAGS := $(filter-out -mthumb-interwork,$(GCC296_CFLAGS))
asm/overlays/common/common2_c%.o: src/overlays/common/common2_c%.c
	$(GCC296_CC) $(COMMON2_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

# NOTE (batch 50): the phrase "equivalently -O2 -fno-schedule-insns2" that used
# to appear in this comment is NOT generally true, and was removed. For
# Func_809a44c the two differ sharply, and giving its whole stem
# -fno-schedule-insns2 breaks four already-matching siblings. -O1 changes
# register allocation and expression ordering as well as the post-reload
# scheduler. Treat the two as unrelated knobs.
# Two overlay TUs verify byte-exact only at -O1 (probed 2026-07-15: the same
# C bodies sit at a stable 2-line diff under -O2 and byte-match the ROM the
# moment -O2 becomes -O1; every other candidate in the corpus stays a fail at
# -O1/-Os, so this is a per-file flag choice in the original build, not a
# global alternative). Pattern form covers the splitter's future children of
# these stems, mirroring the common2_c precedent above.
O1_CFLAGS := $(subst -O2,-O1,$(GCC296_CFLAGS))

# Two overlay TUs match only with gcc's SECOND common-subexpression pass turned
# off. Both load a save-flag id twice around a call, and at -O2 gcc hoists it
# into a callee-saved register -- costing a push, a pop and two moves to save
# one pool load. The ROM loads it twice.
#
# -fno-rerun-cse-after-loop is the pass responsible, and it is specifically
# that one: -fno-gcse, -fno-cse-follow-jumps, -fno-cse-skip-blocks and
# -fno-expensive-optimizations all leave the hoist in place.
#
# FLAGGED FOR REVIEW. This is an assumption about the original build, in the
# same category as the -O1 rules above and on thinner evidence -- two functions,
# not a stem. It may instead mean gcc-2.96 runs a pass the original compiler did
# not, in which case the right fix is a compiler difference rather than a
# per-file flag. Sweeping all 85 parked files with this flag matched only these
# two, so it is NOT a general lever for the constant-CSE class.
#
# APPLYING IT GLOBALLY WAS TESTED AND FAILS. Adding it to GCC296_CFLAGS and
# rebuilding from clean breaks several overlays (rom_78603c, rom_786f0c,
# rom_787e04 among them) and leaves an undefined reference to `_call_via_sl`
# in the main ROM. So the pass IS wanted for most translation units and not
# for these -- which is what a per-file rule means, and is a point in favour
# of the per-TU reading over a whole-compiler difference.
# Six translation units -- all six the SAME 27-instruction function, duplicated
# byte for byte across the main ROM and five per-area overlays -- match only
# with strict aliasing turned OFF.
#
# The function ends with `p->t->ang += p->spin`, loading a POINTER member two
# words past an INT member it has just stored to. At -O2 gcc-2.96 enables
# -fstrict-aliasing, the post-reload scheduler proves the int store cannot
# alias the pointer load, and hoists the load two instructions earlier to fill
# a load-use stall. The ROM leaves it in place. Every other instruction already
# matches, including the src-before-dst load order in the five accumulates that
# the SAME scheduling pass produces -- so the pass is wanted, only its alias
# information is not.
#
# APPLYING IT GLOBALLY WAS TESTED AND FAILS. Adding -fno-strict-aliasing to
# GCC296_CFLAGS and rebuilding all 5336 objects generated from src/ leaves 2631
# bytes differing across the ROM. So most translation units want the alias
# information and these six do not, which is what a per-file rule means.
#
# A seventh byte-identical copy lives in asm/overlays/common/common0.s and is
# NOT elevated: that object is named by many overlay linker scripts, so
# splitting it touches all of them. See src/non_matching notes in batch 69.
ALIAS_CFLAGS := $(GCC296_CFLAGS) -fno-strict-aliasing
# OvlFunc_957_2008ee0 re-reads its counter halfword after two `int` stores; at
# -O2 strict aliasing lets gcc keep the first read. 20 differing lines -> 1.
asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_c_b.o: src/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_c_b.c
	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)


# One translation unit matches only with GLOBAL CSE turned off. Its inner loop
# steps a value by a constant, and at -O2 gcc sinks the constant's pool load
# PAST the loop label -- so it is re-loaded on every iteration. The ROM loads it
# once, before the loop. That is partial-redundancy motion, and no source
# placement reaches it: the constant assigned at the top of the function, or
# immediately before the counter, or immediately after it, all give identical
# output.
#
# SPECIFICALLY -fno-gcse. -fno-strict-aliasing, -fno-strength-reduce and
# -fno-rerun-cse-after-loop are byte-identical on it; -fno-schedule-insns2 is
# worse.
#
# SWEPT ACROSS EVERY PARK before adopting: -fno-gcse improves six parked
# functions (one from 116 differing lines to 19, two from 18 to 6) and matches
# none of them outright. So it is a real mechanism with more to give, and it is
# NOT a general key -- do not reach for it without reading the diff first.
GCSE_CFLAGS := $(GCC296_CFLAGS) -fno-gcse

# -fno-schedule-insns2 : OvlFunc_945_2009978 hoists `mov r0,#0x8f / lsl r0,#4`
# above the gState[0x22b] store at -O2.  The post-reload scheduler is what does
# it; the named-shifted-local lever does not reach it, and -O1 matches too but
# changes more than is needed.  Measured on this one function only.
SCHED2_CFLAGS := $(GCC296_CFLAGS) -fno-schedule-insns2

# -ffixed-r7 : OvlFunc_945_200d6dc allocates three callee-saved registers and
# the ROM's third one is r8 -- which costs `mov r6,r8 / push {r6}` at entry and
# the matching pop -- where gcc reaches for the cheaper r7.  Reserving r7 makes
# gcc spend r8 and the length matches exactly (55 -> 59 lines, the ROM's count).
# -fno-omit-frame-pointer also reserves r7 but adds frame setup (61 lines), so
# it is the register reservation that is wanted and not the frame.
FIXEDR7_CFLAGS := $(GCC296_CFLAGS) -ffixed-r7

# OvlFunc_881_200b6dc: gcc hoists 0xbc << 2 (shared by __GetFlag and __SetFlag)
# into a callee-saved register at -O2; 15 instructions in disagreeing regions
# without the flag, exact with it.
asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_c.o: src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)


# -fno-strength-reduce : LoadMoveRangeIcons recomputes its table byte offset and
# a shifted shape index every iteration; gcc makes induction variables for both
# and the ROM does not.  27 differing without it, byte-identical with it.  The
# while(1)-increment rewrite is NOT a substitute -- it removes the loop rotation,
# not the givs, and makes this function worse (78 lines against the ROM's 70).
STRENGTH_CFLAGS := $(GCC296_CFLAGS) -fno-strength-reduce
asm/rom_a1000/rom_a8604_a_a_c_b.o: src/rom_a1000/rom_a8604_a_a_c_b.c
	$(GCC296_CC) $(STRENGTH_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)


# OvlFunc_960_2008464: 98 differing at -O2, exact with -fno-rerun-cse-after-loop.
asm/overlays/rom_7eaf28/ovl_314_c_a_c_a_c_c.o: src/overlays/rom_7eaf28/ovl_314_c_a_c_a_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)


# OvlFunc_968_20096a4 is caught by the rom_7f2f14/ovl_30_c_a_c_a_c_c% wildcard,
# which applies O1_CFLAGS -- wrong for this TU: 36 differing at -O1, 5 at -O2,
# exact at -O2 with -fno-rerun-cse-after-loop.  An EXPLICIT rule beats a pattern
# rule, so this overrides the wildcard without having to narrow it.
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_b.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_c.o: src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_c.c
	$(GCC296_CC) $(FIXEDR7_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_a_c_b.o: src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_a_c_b.c
	$(GCC296_CC) $(SCHED2_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/rom_f0000/rom_f0254_a_b.o: src/rom_f0000/rom_f0254_a_b.c
	$(GCC296_CC) $(GCSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/rom_8a000/rom_9a44c_a_a_a_b.o: src/rom_8a000/rom_9a44c_a_a_a_b.c
	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_c_b.o: src/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_c_b.c
	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/rom_7ced6c/ovl_30_a_a_c_c_c_c_b.o: src/overlays/rom_7ced6c/ovl_30_a_a_c_c_c_c_b.c
	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/rom_7ed0a0/ovl_30_a_a_a_c_c_c_c_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_a_a_c_c_c_c_c_b.c
	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c_c_b.o: src/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c_c_b.c
	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/rom_7f2f14/ovl_30_a_a_a_c_a_c_b.o: src/overlays/rom_7f2f14/ovl_30_a_a_a_c_a_c_b.c
	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/common/common0_b.o: src/overlays/common/common0_b.c
	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/rom_7ca63c/ovl_30_a_a_a.o: src/overlays/rom_7ca63c/ovl_30_a_a_a.c
	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/rom_7a67d8/ovl_30_c_a_c_c.o: src/overlays/rom_7a67d8/ovl_30_c_a_c_c.c
	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

CSE_CFLAGS := $(GCC296_CFLAGS) -fno-rerun-cse-after-loop
# OvlFunc_901_2008804 reads and then sets the same save flag, 0x307. At plain
# -O2 gcc hoists the id into r5 across the call, paying a push and a pop to save
# one pool load; the ROM loads it twice. 29 differing lines become none.
asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_a_b.o: src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_a.o: src/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

asm/overlays/rom_7bc690/ovl_314_c_c_a_b.o: src/overlays/rom_7bc690/ovl_314_c_c_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_a_a_b.o: src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_b.o: src/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_c_b.o: src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7d768c/ovl_30_c_a_c_a_b.o: src/overlays/rom_7d768c/ovl_30_c_a_c_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78dee8/ovl_30_c_c_a_c_b.o: src/overlays/rom_78dee8/ovl_30_c_c_a_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_a_b.o: src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_a_c.o: src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_a_c_c.o: src/overlays/rom_7bf5a8/ovl_2e0_a_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_a_c_a.o: src/overlays/rom_7bf5a8/ovl_2e0_a_c_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_c_b.o: src/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_c_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_b.o: src/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_b.o: src/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_794ac0/ovl_30_a_c_a_c_a.o: src/overlays/rom_794ac0/ovl_30_a_c_a_c_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79c738/ovl_30_c_c_a_c_a_a_c.o: src/overlays/rom_79c738/ovl_30_c_c_a_c_a_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79c738/ovl_30_c_c_a_a_c_b.o: src/overlays/rom_79c738/ovl_30_c_c_a_a_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# The EIGHTH on this flag, and the first in main-ROM code rather than an
# overlay -- worth noting for the standing question in HANDOFF.md about whether
# the flag is a real property of the original build.
asm/rom_c9000/rom_cd260_b.o: src/rom_c9000/rom_cd260_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_a_c_c.o: src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_a_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_c_b.o: src/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a_c_b.o: src/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7a04ac/ovl_30_c_c_c_a_c.o: src/overlays/rom_7a04ac/ovl_30_c_c_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_a.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79e5c0/ovl_30_c_a_c.o: src/overlays/rom_79e5c0/ovl_30_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_a_a.o: src/overlays/rom_7c6bac/ovl_30_c_c_a_c_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_b.o: src/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_f84_c_c_b.o: src/overlays/rom_7ac2d8/ovl_f84_c_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7d30e0/ovl_30_c_c_a_c_a_c_c.o: src/overlays/rom_7d30e0/ovl_30_c_c_a_c_a_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_c_a.o: src/overlays/rom_7d0e88/ovl_1528_c_c_c_c_c_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c_c_c_a_b.o: src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c_c_c_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_780898/ovl_30_c_c_a_c_b.o: src/overlays/rom_780898/ovl_30_c_c_a_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_c_b.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_c_c_b.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_c_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7d768c/ovl_30_c_a_a_c_a_b.o: src/overlays/rom_7d768c/ovl_30_c_a_a_c_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_b.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_b.c
	$(GCC296_CC) $(GCC296_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_b.o: src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_c_a.o: src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_c_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_c_a.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_c_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.o: src/overlays/rom_78b2ac/ovl_30_c_c_a_a_b.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.o: src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.o: src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.o: src/overlays/rom_79b154/ovl_30_c_a_a_c_a_c_c_c.c
	$(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap as the rom_7b7f1c rule below: this was
# `ovl_30_a_c_a_a%`, and it also captured `ovl_30_a_c_a_a_c_a_*` -- the OTHER
# half of the split of ovl_30_a_c_a_a_c.s. Those two halves do not share a
# translation unit: OvlFunc_964_2009348 sits in the _c_a half, was parked at 6
# of 18 under the inherited -O1, and byte-matches at -O2.
#
# The two files named here each keep -O1 because each is green with it today.
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.o: src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NOT equivalent to -O2 -fno-schedule-insns2, unlike the rules above.
# OvlFunc_917_20092b4 is cross-jumped at -O2 -- gcc merges two `bl` calls into
# one shared tail and the ROM keeps them separate -- and cross-jumping is a
# jump-pass decision, not a scheduling one. -fno-schedule-insns2 leaves it seven
# positions out; only real -O1 matches.
asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.o: src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# This one IS equivalent to -O2 -fno-schedule-insns2; -O1 is used for
# consistency with the rules above. Two pool loads in the wrong order.
asm/overlays/rom_7bc690/ovl_4e4_a_a_b.o: src/overlays/rom_7bc690/ovl_4e4_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# Unparked by -O1 alone, with the C unchanged from the parked version. The
# speculative literal hoist that parked it in batch 32 is an -O2 behaviour.
asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.o: src/overlays/rom_7c460c/ovl_314_a_c_a_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# 2026-07-16 fakematch de-hack sweep: the TUs below verify byte-exact with
# their asm scaffolds removed only at -O1. (The parenthetical "equivalently
# -O2 -fno-schedule-insns2" that stood here was REMOVED in batch 50: the two
# are not equivalent -- -O1 also changes register allocation and expression
# ordering -- and it was never verified for these TUs, only assumed. If you
# want the scheduler knob alone, test it; do not infer it from -O1.)
# The same per-file flag choice in the original
# build as the rules above. Pattern form covers the splitter's future
# children of a stem; exact-file form is used where a sibling under the
# same stem verifies only at -O2 (per-file flag mixing).
# TODO: consolidate the TUs 
# Exact-file form (2026-07-17): the pattern ovl_30_c_c_c_a_c% swallowed the
# new split child ovl_30_c_c_c_a_c_c_c_b.c, whose match verifies only at -O2
# (judge -O2 pass, in-tree -O1 build failed compare-rom); per-file flag
# mixing inside this chain, so the TU boundary sits between _c_c_b and _c_c_c.
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.o: src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_78603c/ovl_30_c_c_a_c_a%.o: src/overlays/rom_78603c/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.o: src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, same trap: this was `ovl_e90_c_c_a_a%` and captured
# children of ovl_e90_c_c_a_a_c.s as well. OvlFunc_923_2008ed0 lives there, was
# parked at 6 of 41 under the inherited -O1, and byte-matches at -O2.
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.o: src/overlays/rom_7aa430/ovl_e90_c_c_a_a_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.o: src/overlays/rom_7b4558/ovl_30_c_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
# NARROWED 2026-08-24, and the reason matters. This was
# `ovl_30_c_c_a_c_c_c_c%`, which ALSO captured `ovl_30_c_c_a_c_c_c_c_c_*` --
# a different .s further down the same split chain.
#
# The trap is that the split chain is NOT a TU boundary. Splitting carves one
# overlay's assembly into `_a`/`_b`/`_c` pieces by position, and an overlay
# holds many original translation units, so two pieces sharing a name prefix
# say nothing about sharing a compiler invocation. A pattern anchored on a
# prefix therefore spreads a per-TU flag choice to code that never belonged to
# that TU. OvlFunc_930_2008ff0 and _2009028 byte-match at -O2 and sit
# at a clean 4-line argument-fill diff at -O1, which reads exactly like the
# fill-order blocker and cost most of a round before the flag was suspected.
# The `_b_%` form still covers this stem's own future split children, which is
# what the pattern was for.
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_b_%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.o: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o: src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.o: src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a%.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
asm/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.o: src/overlays/rom_7ac2d8/ovl_35b8_a_a_a_c_b.c
	$(GCC296_CC) $(O1_CFLAGS) -S -o $(@:.o=.s) $<
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

# src/lib/m4a/ is the stock m4a / "Sappy" engine, prebuilt by Nintendo with
# old_agbcc (signed char, old ABI), NOT Camelot's gcc296. Per-file rule mirrors
# sa2/Makefile's CC1_OLD override. -D M4A_SIGNED_CHAR gives the engine a signed
# s8 (its ROM loads are signed) without touching the rest of the unsigned-char
# corpus. See SAPPY_IMPORT_PLAN.md.
AGBCC_DIR     ?= tools/agbcc
M4A_CPPFLAGS  := -nostdinc -I$(AGBCC_DIR)/include -Iinclude -D PLATFORM_GBA=1 -D M4A_SIGNED_CHAR
M4A_CC1FLAGS  := -Wimplicit -Wparentheses -fhex-asm -mthumb-interwork -O2

src/lib/m4a/%.o: src/lib/m4a/%.c
	gcc -E $(M4A_CPPFLAGS) $< -o $(@:.o=.i)
	$(AGBCC_DIR)/bin/old_agbcc $(M4A_CC1FLAGS) -o $(@:.o=.s) $(@:.o=.i)
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

# src/lib/agb_flash/ is the launch-SDK "Flash v123" save library. Like m4a it is a
# prebuilt Nintendo lib (old_agbcc, stock r4-callee-save ABI); but -O not -O2, and
# unsigned char (no M4A_SIGNED_CHAR). The lone gcc-2.96 holdout agb_flash_verify.c
# (VerifyEraseSector) rides the default %.o:%.c rule instead.
AGBFLASH_CPPFLAGS := -nostdinc -I$(AGBCC_DIR)/include -Iinclude -D PLATFORM_GBA=1
AGBFLASH_CC1FLAGS := -Wimplicit -Wparentheses -fhex-asm -mthumb-interwork -O

src/lib/agb_flash/agb_flash.o: src/lib/agb_flash/agb_flash.c
	gcc -E $(AGBFLASH_CPPFLAGS) $< -o $(@:.o=.i)
	$(AGBCC_DIR)/bin/old_agbcc $(AGBFLASH_CC1FLAGS) -o $(@:.o=.s) $(@:.o=.i)
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

src/lib/agb_flash/agb_flash_mx.o: src/lib/agb_flash/agb_flash_mx.c
	gcc -E $(AGBFLASH_CPPFLAGS) $< -o $(@:.o=.i)
	$(AGBCC_DIR)/bin/old_agbcc $(AGBFLASH_CC1FLAGS) -o $(@:.o=.s) $(@:.o=.i)
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

src/lib/agb_flash/agb_flash_at.o: src/lib/agb_flash/agb_flash_at.c
	gcc -E $(AGBFLASH_CPPFLAGS) $< -o $(@:.o=.i)
	$(AGBCC_DIR)/bin/old_agbcc $(AGBFLASH_CC1FLAGS) -o $(@:.o=.s) $(@:.o=.i)
	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)

# src/lib/m4a/ excluded from the default gcc296 C_SRCS (built by the rule above).
C_SRCS  := $(filter-out src/lib/m4a/%,$(wildcard *.c */*.c */*/*.c))
C_OBJS  := $(C_SRCS:.c=.o)
C_GEN_S := $(C_SRCS:.c=.s)
C_GEN_I := $(C_SRCS:.c=.i)

# Read additional dependencies (besides .o => .s) from .d files
# generated by the assembler.
SRCS := $(wildcard *.s */*.s */*/*.s)
DEPS := $(SRCS:.s=.d)
-include $(DEPS)


# Clean target.
#
# The legacy wildcard-derived lists (OBJS, C_OBJS, C_GEN_S, C_GEN_I) only go
# to depth 3, so they silently miss overlay sources at depth 4 and the
# cross-dir-rule .s artifacts at asm/<rel>/<name>.s. Replaced with a
# find-based sweep that's depth-agnostic. DEPS is still computed above
# because -include needs it; we don't reference it here (find catches .d).
.PHONY: clean
LDS  := $(wildcard *.ld */*/*.ld)
MAPS := $(LDS:.ld=.map)
clean::
	-$(RM) $(ROM) $(OVERLAYS) $(ELFS) $(MAPS) tags
	-find asm src overlays -type f \( -name '*.o' -o -name '*.d' -o -name '*.i' \) -delete 2>/dev/null
	-find src -name '*.c' -printf '%P\n' 2>/dev/null | sed 's|\.c$$|.s|' | \
	    while read rel; do $(RM) "src/$$rel" "asm/$$rel"; done

# Builds ctags using custom parsing on top of asm.
# Ensure https://github.com/universal-ctags/ctags is installed to use.
# Generates build artifact `tags`. .PHONY so the index refreshes as
# matches land (the recipe writes a file literally named `tags`).
.PHONY: tags
tags:
	ctags -R --options=.opts.ctags .

# Tools are compiled for the host and used during the build.

TOOLS := tools/pack_overlay \
	 tools/pack_strings \
	 tools/unpack_overlay \
	 tools/unpack_strings

CPPFLAGS += -MMD
CFLAGS ?= -O2 -Wall

# Host tool build; explicit rules so they override the generic %.o:%.c
# (which points at the gcc-2.96 target pipeline above). The tools/ prefix
# makes these rules more-specific than the generic ones.
tools/%.o: tools/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

tools/%: tools/%.o
	$(CC) -o $@ $<

$(TOOLS):

TOOL_SRCS := $(wildcard tools/*.c)
TOOL_OBJS := $(TOOL_SRCS:.c=.o)
TOOL_DEPS := $(TOOL_OBJS:.o=.d)

-include $(TOOL_DEPS)

clean::
	-$(RM) $(TOOLS) $(TOOL_OBJS) $(TOOL_DEPS)


data/strings/strings.s: data/strings/strings.txt tools/pack_strings
	tools/pack_strings -i $< -o $(dir $@)

data/strings/strings.txt: baserom.gba tools/unpack_strings
	mkdir -p $(dir $@)
	tools/unpack_strings -r $< -o $@


OVERLAY_LZS := $(OVERLAYS:.bin=.lz)

$(OVERLAY_LZS): %.lz: %.bin tools/pack_overlay
	tools/pack_overlay -i $< -o $@

asm/rom_320000/rom_320000.s: $(OVERLAY_LZS)

clean::
	-$(RM) -r data/strings $(OVERLAY_LZS)


# We need the uncompressed overlays for incbin statements in overlay
# sources. They're also convenient for comparing our build outputs.

OVERLAY_DIRS := $(dir $(OVERLAYS))

define overlay_orig_deps
$(patsubst %.s,%.o,$(wildcard asm/$(strip $(1))*.s)): %.o: $(strip $(1))orig.bin
endef
$(foreach overlay_dir,$(OVERLAY_DIRS),$(eval $(call overlay_orig_deps, $(overlay_dir))))

# The three common overlays incbin from a specific overlay's orig.bin. These
# have to be WILDCARDED over the split descendants, not named directly: once
# common2.s is split into common2_a.s / common2_c_c_..._b.s, the .incbin
# travels into whichever part holds it, while a dependency naming common2.o
# alone keeps pointing at a file that no longer exists.
#
# The symptom appears only in a CLEAN build, because `clean` deletes orig.bin
# and an incremental build still has one lying around from last time:
#
#     common2_c_c_c_c_c_c_c_c_c_c.s:112:
#         Error: file not found: overlays/rom_7bf5a8/orig.bin
define common_orig_deps
$(patsubst %.s,%.o,$(wildcard asm/overlays/common/$(strip $(1))*.s)): %.o: $(strip $(2))
endef
$(eval $(call common_orig_deps, common0, overlays/rom_78ef88/orig.bin))
$(eval $(call common_orig_deps, common1_c, overlays/rom_7db0c8/orig.bin))
$(eval $(call common_orig_deps, common2, overlays/rom_7bf5a8/orig.bin))

overlays/rom_%/orig.bin: baserom.gba tools/unpack_overlay
	tools/unpack_overlay -r $< -a 0x$* -o $@

clean::
	-$(RM) $(addsuffix orig.bin,$(OVERLAY_DIRS))
