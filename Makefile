# Default target. Verify the checksums of the built ROM and overlays.

ROM := goldensun.gba
OVERLAYS := $(patsubst %.ld,%.bin,$(wildcard overlays/*/overlay.ld))

# 1. Enable 'pipefail' so Make catches errors anywhere in the command chain
SHELL := /bin/bash
.SHELLFLAGS := -o pipefail -c
# Camelot compiled Golden Sun with gcc-2.96 (arm-elf, 2000-07-31 dev snapshot),
# NOT with agbcc. See docs/matching.md.
#
# This lives at /opt/gcc296 inside the build container; override GCC296_DIR if
# you installed it elsewhere. macOS cannot run it -- see
# docs/building-on-macos.md.
GCC296_DIR ?= /opt/gcc296
# NOT named CC: make's builtin rules use CC to build the host tools in tools/,
# which need the host compiler, not this cross-compiler.
GBA_CC     := $(GCC296_DIR)/xgcc
AS         := arm-none-eabi-as

# Unlike agbcc (a bare cc1 needing a separate cpp), xgcc is the full driver and
# runs its own preprocessor, so there is no CPP stage in the rule below.
#
# -O2               Camelot's level; agbcc needed -O
# -fcall-used-r4    r4 is caller-clobbered in Camelot's ABI. 727 of 2202 Thumb
#                   functions use r4 without pushing it. This flag was derived
#                   here independently and still applies under gcc-2.96.
# -ffixed-r7        NOT needed: gcc-2.96 avoids r7 on its own (verified: zero
#                   r7 references in a high-pressure probe).
GBA_CFLAGS := -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
              -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -Iinclude

.PHONY: compare compare-rom compare-overlays check-layouts
compare: compare-rom compare-overlays

# The struct layouts in include/ carry sizeof assertions -- a wrong offset makes
# an array size negative and agbcc refuses the file. Nothing links this; it is
# compiled purely so the layouts cannot drift unnoticed.
check-layouts: tools/layout_check.c
	@$(GBA_CC) -B$(GCC296_DIR)/ $(GBA_CFLAGS) -S $< -o /dev/null && \
	  echo "struct layouts OK"

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


# Convert executables to free-standing binaries
$(ROM) $(OVERLAYS):
	arm-none-eabi-objcopy -O binary $< $@

$(ROM): %.gba: %.elf

$(OVERLAYS): %.bin: %.elf


# Delete a target if its recipe fails. Without this, a C compile that fails
# still leaves the .o behind: the pipeline in the %.o: %.c rule runs `as` on
# whatever agbcc emitted before erroring, so a broken source produces a small
# but valid-looking object that the next build happily links.
.DELETE_ON_ERROR:

# Assemble ARM code and generate dependencies
%.o: %.s
	arm-none-eabi-as -mcpu=arm7tdmi -Iinclude -MD $(@:.o=.d) -o $@ $<
# xgcc is the full driver: it preprocesses, compiles and emits asm in one step,
# so there is no separate cpp stage.
#
# -mthumb-interwork is essential. Without it the epilogue is `pop {..., pc}`,
# which does not switch modes on ARMv4T, rather than the `pop {r0}; bx r0` the
# ROM uses -- so nothing could ever match.
#
# The trailing `.align 2, 0` is required: the patched compiler zero-fills
# between functions but not after the last one in a translation unit, so the
# assembler's default Thumb-nop padding would otherwise leak in.
%.o: %.c
	$(GBA_CC) -B$(GCC296_DIR)/ $(GBA_CFLAGS) -S $< -o - | \
	cat - <(printf '.text\n\t.align\t2, 0\n') | \
	$(AS) -mcpu=arm7tdmi -o $@

# rom_f9000 is the stock m4a ("Sappy") audio engine -- the prebuilt library
# every GBA licensee linked, NOT Camelot's own code. It was compiled with
# old_agbcc and does not match under gcc-2.96: the C is byte-identical in shape
# but lands one scratch register differently, and no C formulation fixes it
# (three were tried). So it gets its own compiler.
#
# old_agbcc is a bare cc1, so this needs a separate preprocessor pass. The host
# gcc serves as cpp here, as it does in sa2 and in Coaltergeist's decomp.
#
# Note the absence of -fcall-used-r4: that is Camelot's ABI, and this library
# is not Camelot's.
AGBCC_DIR    ?= /opt/agbcc
M4A_CPPFLAGS := -nostdinc -I$(AGBCC_DIR)/include -Iinclude
M4A_CC1FLAGS := -Wimplicit -Wparentheses -fhex-asm -mthumb-interwork -O2

# Scoped to the specific TUs that are stock library code. Applying this to all
# of rom_f9000 breaks Func_f94f8/f9538/f954c/f9594, which ARE Camelot's and want
# gcc-2.96 -- the bank is a mix, not uniformly m4a.
M4A_SRCS := rom_f9000/src/f_x_rom_fb6ec.c

# Intermediates go to a temp path, NOT next to the source. Writing a .s beside
# the .c makes the `%.o: %.s` rule -- which precedes `%.o: %.c` -- shadow the
# source on the next build, silently compiling something else. That cost an
# afternoon once already.
$(M4A_SRCS:.c=.o): %.o: %.c
	@gcc -E $(M4A_CPPFLAGS) $< -o /tmp/m4a_$(notdir $*).i
	@$(AGBCC_DIR)/bin/old_agbcc $(M4A_CC1FLAGS) -o /tmp/m4a_$(notdir $*).s /tmp/m4a_$(notdir $*).i
	@printf '\n\t.text\n\t.align\t2, 0\n' >> /tmp/m4a_$(notdir $*).s
	$(AS) -mcpu=arm7tdmi -o $@ /tmp/m4a_$(notdir $*).s

# Read additional dependencies (besides .o => .s) from .d files
# generated by the assembler.
SRCS_S := $(wildcard *.s */*.s */*/*.s)
SRCS_C := $(wildcard *.c */*.c */*/*.c)

OBJS := $(SRCS_S:.s=.o) $(SRCS_C:.c=.o)

DEPS := $(SRCS_S:.s=.d) $(SRCS_C:.c=.d)
-include $(DEPS)


# Clean target.
.PHONY: clean
LDS  := $(wildcard *.ld */*/*.ld)
MAPS := $(LDS:.ld=.map)
# NOTE: this used to read `OBJS := $(SRCS:.s=.o)`. SRCS is never defined -- the
# variables are SRCS_S and SRCS_C -- so OBJS expanded to nothing and silently
# overrode the correct definition above. `make clean` therefore never removed a
# single object file, and a "clean" rebuild after a toolchain change quietly
# relinked stale objects. Do not reintroduce a second OBJS assignment here.
clean::
	-$(RM) $(ROM) $(OVERLAYS) $(ELFS) $(MAPS) $(OBJS) $(DEPS)


# Tools are compiled for the host and used during the build.

TOOLS := tools/pack_overlay \
	 tools/pack_strings \
	 tools/unpack_overlay \
	 tools/unpack_strings

CPPFLAGS += -MMD
CFLAGS ?= -O2 -Wall

$(TOOLS):

TOOL_SRCS := $(wildcard tools/*.c)
TOOL_OBJS := $(TOOL_SRCS:.c=.o)
TOOL_DEPS := $(TOOL_OBJS:.o=.d)

-include $(TOOL_DEPS)

clean::
	-$(RM) $(TOOLS) $(TOOL_OBJS) $(TOOL_DEPS)


rom_15000/data/strings/strings.s: rom_15000/data/strings/strings.txt tools/pack_strings
	tools/pack_strings -i $< -o $(dir $@)

rom_15000/data/strings/strings.txt: baserom.gba tools/unpack_strings
	mkdir -p $(dir $@)
	tools/unpack_strings -r $< -o $@


OVERLAY_LZS := $(OVERLAYS:.bin=.lz)

$(OVERLAY_LZS): %.lz: %.bin tools/pack_overlay
	tools/pack_overlay -i $< -o $@

rom_320000/src/rom_320000.s: $(OVERLAY_LZS)

clean::
	-$(RM) -r rom_15000/data $(OVERLAY_LZS)


# We need the uncompressed overlays for incbin statements in overlay
# sources. They're also convenient for comparing our build outputs.

OVERLAY_DIRS := $(dir $(OVERLAYS))

define overlay_orig_deps
$(patsubst %.s,%.o,$(wildcard $(1)*.s)): %.o: $(1)orig.bin
endef
$(foreach overlay_dir,$(OVERLAY_DIRS),$(eval $(call overlay_orig_deps, $(overlay_dir))))

overlays/common/common0.o: overlays/rom_78ef88/orig.bin

overlays/common/common1.o: overlays/rom_7db0c8/orig.bin

overlays/common/common2.o: overlays/rom_7bf5a8/orig.bin

overlays/rom_%/orig.bin: baserom.gba tools/unpack_overlay
	tools/unpack_overlay -r $< -a 0x$* -o $@

clean::
	-$(RM) $(addsuffix orig.bin,$(OVERLAY_DIRS))
