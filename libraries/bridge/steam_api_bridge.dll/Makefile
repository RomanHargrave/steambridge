### Generated by Winemaker 0.8.4
###
### Invocation command line was
### /usr/bin/winemaker steam_api_bridge.dll --nosource-fix --dll --nomfc --wine32 -DVERSION_SAFE_STEAM_API_INTERFACES -I../../common/steam/ -I../../common/include/ -ldl


SRCDIR                = .
SUBDIRS               =
DLLS                  = steam_api_bridge.dll
LIBS                  =
EXES                  =



### Common settings

CEXTRA                = -mno-cygwin \
			-m32 -Wall -Wextra -pipe -g
CXXEXTRA              = -m32 -Wall -Wextra -pipe -g
RCEXTRA               =
DEFINES               = -DVERSION_SAFE_STEAM_API_INTERFACES
INCLUDE_PATH          = -I../../common/steam/ \
			-I../../common/include/ \
			-Iinclude
DLL_PATH              =
DLL_IMPORTS           =
LIBRARY_PATH          =
LIBRARIES             = -ldl


### steam_api_bridge.dll sources and settings

steam_api_bridge_dll_MODULE= steam_api_bridge.dll
steam_api_bridge_dll_C_SRCS=
steam_api_bridge_dll_CXX_SRCS= source/api_get.cpp \
			source/callback.cpp \
			source/core.cpp \
			source/logging.cpp \
			source/meta.cpp \
			source/state.cpp
steam_api_bridge_dll_RC_SRCS=
steam_api_bridge_dll_LDFLAGS= -shared \
			$(steam_api_bridge_dll_MODULE:.dll=.spec) \
			-mno-cygwin \
			-m32 -Wall -Wextra -pipe -g
steam_api_bridge_dll_ARFLAGS=
steam_api_bridge_dll_DLL_PATH=
steam_api_bridge_dll_DLLS= odbc32 \
			ole32 \
			oleaut32 \
			winspool \
			odbccp32
steam_api_bridge_dll_LIBRARY_PATH=
steam_api_bridge_dll_LIBRARIES= uuid

steam_api_bridge_dll_OBJS= $(steam_api_bridge_dll_C_SRCS:.c=.o) \
			$(steam_api_bridge_dll_CXX_SRCS:.cpp=.o) \
			$(steam_api_bridge_dll_RC_SRCS:.rc=.res)



### Global source lists

C_SRCS                = $(steam_api_bridge_dll_C_SRCS)
CXX_SRCS              = $(steam_api_bridge_dll_CXX_SRCS)
RC_SRCS               = $(steam_api_bridge_dll_RC_SRCS)


### Tools

CC = winegcc
CXX = wineg++
RC = wrc
AR = ar


### Generic targets

all: $(SUBDIRS) $(DLLS:%=%.so) $(LIBS) $(EXES)

### Build rules

.PHONY: all clean dummy

$(SUBDIRS): dummy
	@cd $@ && $(MAKE)

# Implicit rules

.SUFFIXES: .cpp .cxx .rc .res
DEFINCL = $(INCLUDE_PATH) $(DEFINES) $(OPTIONS)

.c.o:
	$(CC) -c $(CFLAGS) $(CEXTRA) $(DEFINCL) -o $@ $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CXXEXTRA) $(DEFINCL) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(CXXEXTRA) $(DEFINCL) -o $@ $<

.rc.res:
	$(RC) $(RCFLAGS) $(RCEXTRA) $(DEFINCL) -fo$@ $<

# Rules for cleaning

CLEAN_FILES     = y.tab.c y.tab.h lex.yy.c core *.orig *.rej \
                  \\\#*\\\# *~ *% .\\\#*

clean:: $(SUBDIRS:%=%/__clean__) $(EXTRASUBDIRS:%=%/__clean__)
	$(RM) $(CLEAN_FILES) $(RC_SRCS:.rc=.res) $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)
	$(RM) $(DLLS:%=%.so) $(LIBS) $(EXES) $(EXES:%=%.so)

$(SUBDIRS:%=%/__clean__): dummy
	cd `dirname $@` && $(MAKE) clean

$(EXTRASUBDIRS:%=%/__clean__): dummy
	-cd `dirname $@` && $(RM) $(CLEAN_FILES)

### Target specific build rules
DEFLIB = $(LIBRARY_PATH) $(LIBRARIES) $(DLL_PATH) $(DLL_IMPORTS:%=-l%)

$(steam_api_bridge_dll_MODULE).so: $(steam_api_bridge_dll_OBJS) $(steam_api_bridge_dll_MODULE:.dll=.spec)
	$(CXX) $(steam_api_bridge_dll_LDFLAGS) -o $@ $(steam_api_bridge_dll_OBJS) $(steam_api_bridge_dll_LIBRARY_PATH) $(steam_api_bridge_dll_DLL_PATH) $(DEFLIB) $(steam_api_bridge_dll_DLLS:%=-l%) $(steam_api_bridge_dll_LIBRARIES:%=-l%)


