
CC 	= ppc-morphos-gcc
CDEFS   = -DUSE_INLINE_STDARG
CFLAGS  = $(CDEFS) $(CWARN) -O2 -fomit-frame-pointer -mcpu=750 -mstring -mmultiple -noixemul -fstrict-aliasing -Iinclude/
LDFLAGS = -noixemul
CWARN   = -Wall\
	  -Wno-missing-braces\
	  -Wno-parentheses\
          -Wno-switch\
          -Wstrict-prototypes\
	  -Werror-implicit-function-declaration\
          -Wpointer-arith

OBJS = rss.o parseargs.o loc.o utils.o date.o options.o uri.o lineread.o http.o favicon.o md5.o app.o win.o bwin.o search.o\
prefs.o minimail.o cm.o editfeed.o gmt.o popupcodesets.o popph.o agent.o group.o rootbar.o rootstring.o rootscrollgroup.o\
picmos.o grouplist.o entrieslist.o html.o stubs.o

amrss: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lpoolbastard -ldebug
	ppc-morphos-strip $@

loc.o: loc.c rss.h loc.h macros.h rss_protos.h
editfeed.o: editfeed.c rss.h loc.h macros.h rss_protos.h
grouplist.o: grouplist.c rss.h loc.h macros.h rss_protos.h
group.o: group.c rss.h http.h loc.h macros.h rss_protos.h lineread.h
rootscrollgroup.o: rootscrollgroup.c rss.h macros.h rss_protos.h
rootbar.o: rootbar.c rss.h loc.h macros.h rss_protos.h
options.o: options.c rss.h amrss_rev.h macros.h rss_protos.h
lineread.o: lineread.c rss.h http.h macros.h rss_protos.h lineread.h
html.o: html.c rss.h lineread.h uri.h loc.h amrss_rev.h macros.h rss_protos.h
grop.o: grop.c rss.h macros.h rss_protos.h
gmt.o: gmt.c rss.h loc.h macros.h rss_protos.h
entrieslist.o: entrieslist.c rss.h loc.h macros.h rss_protos.h
date.o: date.c rss.h macros.h rss_protos.h
app.o: app.c rss.h loc.h amrss_rev.h macros.h rss_protos.h
win.o: win.c rss.h macros.h rss_protos.h
bwin.o: bwin.c rss.h macros.h rss_protos.h
rss.o: rss.c rss.h loc.h amrss_rev.h macros.h rss_protos.h
rootstring.o: rootstring.c rss.h loc.h macros.h rss_protos.h
parseargs.o: parseargs.c rss.h amrss_rev.h macros.h rss_protos.h
utils.o: utils.c rss.h macros.h rss_protos.h
search.o: search.c rss.h loc.h macros.h rss_protos.h
prefs.o: prefs.c rss.h loc.h macros.h rss_protos.h
minimail.o: minimail.c rss.h loc.h macros.h rss_protos.h
popupcodesets.o: popupcodesets.c rss.h macros.h rss_protos.h
popph.o: popph.c rss.h macros.h rss_protos.h
agent.o: agent.c rss.h amrss_rev.h macros.h rss_protos.h
picmos.o: picmos.c rss.h macros.h rss_protos.h
uri.o: uri.c rss.h uri.h loc.h macros.h rss_protos.h
http.o: http.c rss.h http.h lineread.h uri.h loc.h amrss_rev.h macros.h rss_protos.h lineread.h
favicon.o: favicon.c rss.h http.h lineread.h uri.h loc.h amrss_rev.h macros.h rss_protos.h lineread.h
cm.o: cm.c rss.h loc.h macros.h rss_protos.h
md5.o: md5.c rss.h macros.h rss_protos.h
stubs.o: stubs.c rss.h loc.h macros.h rss_protos.h

