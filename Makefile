all:
.SILENT:
.SECONDARY:
PRECMD=echo "  $(@F)" ; mkdir -p $(@D) ;

# Defines for z26:
# do_linux_desktop_integration
# X LINUX/WINDOWS/MAC

EHCFG:=../ra3/out/emuhost-config
EH_CFLAGS:=$(shell $(EHCFG) --cflags)
EH_LDFLAGS:=$(shell $(EHCFG) --ldflags)
EH_LIBS:=$(shell $(EHCFG) --libs)
EH_DEPS:=$(shell $(EHCFG) --deps)

CC:=gcc -std=gnu99 -no-pie -Wall -Wno-comment -DLINUX -O3 -c -MMD $(EH_CFLAGS)
LD:=gcc $(EH_LDFLAGS)
LDPOST:=$(EH_LIBS)

# Everything I'm adding, which just be Emuhost glue, builds in the usual fashion.
SRCFILES:=$(shell find src -type f)
CFILES:=$(filter-out src/z26/% src/z26-piecemeal-abort/%,$(filter %.c,$(SRCFILES)))
OFILES:=$(patsubst src/%,mid/%.o,$(basename $(CFILES)))
mid/%.o:src/%.c;$(PRECMD) $(CC) -o$@ $<

# z26 was written to be compiled in a single gcc run, and teasing that apart proved too difficult.
OFILES_z26:=mid/z26/z26.o
OFILES+=$(OFILES_z26)
mid/z26/z26.o:src/z26/z26.c;$(PRECMD) $(CC) -o$@ $<

-include $(OFILES:.o=.d)

EXE:=out/akz26
all:$(EXE)
$(EXE):$(OFILES) $(EH_DEPS);$(PRECMD) $(LD) -o $@ $(OFILES) $(LDPOST)

clean:;rm -rf mid out

run:$(EXE);$(EXE) ~/rom/atari2600/p/pacman.bin
emuhost:;make -C../ra3 ; rm -f $(EXE)
