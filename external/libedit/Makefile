include objdir.gmk
include generic.paths.gmk

CFLAGS+=-g -O0 -std=c11 #-Wall #-Werror
CPPFLAGS+=-I. -I$(OBJDIR) -isystem $(pkg_include_ncurses)/ncurses -Iedit
#CPPFLAGS+=-I. -I$(OBJDIR) -Iedit

hdrs=vi emacs common
AHDR=$(patsubst %,$(OBJDIR)/%.h,$(hdrs))
ASRC=$(patsubst %,%.c,$(hdrs))

OSRCS=	chared.c common.c el.c eln.c emacs.c filecomplete.c \
	hist.c keymacro.c map.c chartype.c \
	parse.c prompt.c read.c refresh.c search.c sig.c terminal.c tty.c vi.c
OSRCS+=$(patsubst %,$(OBJDIR)/%,fcns.c help.c )

$(OBJDIR):
	@mkdir -p $@

$(AHDR): $(OBJDIR)/%.h : %.c $(OBJDIR)
	sh makelist -h $< > $@

$(OBJDIR)/fcns.h: $(AHDR)
	sh makelist -fh $(AHDR) > $@

$(OBJDIR)/fcns.c: $(AHDR)
	sh makelist -fc $(AHDR) > $@

$(OBJDIR)/help.c: $(ASRC)
	sh makelist -bc $(ASRC) > $@

$(OBJDIR)/help.h: $(ASRC)
	sh makelist -bh $(ASRC) > $@

$(OBJDIR)/editline.c: ${OSRCS}
	sh makelist -e $^ > $@

$(OBJDIR)/tokenizern.c:
	sh makelist -n tokenizer.c > $@

$(OBJDIR)/historyn.c:
	sh makelist -n history.c > $@

name=edit
lib=edit
include=edit
pkg_depends=external/libbsd external/ncurses
sources_c=readline.c tokenizer.c history.c
sources_c+=$(patsubst %,$(OBJDIR)/%,editline.c)
sources_c+=$(patsubst %,$(OBJDIR)/%,tokenizern.c historyn.c)
$(patsubst %,$(OBJDIR)/%.o,$(sources_c)): $(patsubst %,$(OBJDIR)/%,fcns.h help.h)

#$(info ${INCDIR})
pkg_name=libedit
include package.gmk

include generic.lib.gmk
