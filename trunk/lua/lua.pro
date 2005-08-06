TEMPLATE = lib
CONFIG += staticlib dll

DESTDIR = ..
HEADERS += lapi.h lauxlib.h lcode.h ldebug.h ldo.h lfunc.h lgc.h llex.h llimits.h lmem.h lobject.h lopcodes.h lparser.h lstate.h lstring.h ltable.h ltm.h luaconf.h lua.h lualib.h lundump.h lvm.h lzio.h
SOURCES += lapi.c lauxlib.c lbaselib.c lcode.c ldblib.c ldebug.c ldo.c ldump.c lfunc.c lgc.c linit.c liolib.c llex.c lmathlib.c lmem.c loadlib.c lobject.c lopcodes.c loslib.c lparser.c lstate.c lstring.c lstrlib.c ltable.c ltablib.c ltm.c lundump.c lvm.c lzio.c print.c

CONFIG += warn_on
CONFIG -= qt
CONFIG -= debug_and_release

