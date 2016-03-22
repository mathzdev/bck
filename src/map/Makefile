
COMMON_H = $(shell ls ../common/*.h)
COMMON_AR = ../common/obj/common.a
COMMON_INCLUDE = -I../common/

MT19937AR_OBJ = ../../3rdparty/mt19937ar/mt19937ar.o
MT19937AR_H = ../../3rdparty/mt19937ar/mt19937ar.h
MT19937AR_INCLUDE = -I../../3rdparty/mt19937ar

LIBCONFIG_H = $(shell ls ../../3rdparty/libconfig/*.h)
LIBCONFIG_AR = ../../3rdparty/libconfig/obj/libconfig.a
LIBCONFIG_INCLUDE = -I../../3rdparty/libconfig

MAP_OBJ = $(shell ls *.c | sed -e "s/\.c/\.o/g")
MAP_DIR_OBJ = $(MAP_OBJ:%=obj/%)
MAP_H = $(shell ls ../map/*.h) \
	$(shell ls ../config/*.h) 

HAVE_MYSQL=yes
ifeq ($(HAVE_MYSQL),yes)
	SERVER_DEPENDS=map-server
else
	SERVER_DEPENDS=needs_mysql
endif
ALL_DEPENDS=server

HAVE_PCRE=
ifeq ($(HAVE_PCRE),yes)
	PCRE_CFLAGS=-DPCRE_SUPPORT 
else
	PCRE_CFLAGS=
endif



#####################################################################
.PHONY : all server clean help

all: $(ALL_DEPENDS)

server: $(SERVER_DEPENDS)

clean:
	@echo "	CLEAN	map"
	@rm -rf *.o obj ../../map-server

help:
	@echo "possible targets are 'server' 'all' 'clean' 'help'"
	@echo "'server' - map server"
	@echo "'all'    - builds all above targets"
	@echo "'clean'  - cleans builds and objects"
	@echo "'help'   - outputs this message"

#####################################################################

needs_mysql:
	@echo "MySQL not found or disabled by the configure script"
	@exit 1

# object directories

obj:
	@echo "	MKDIR	obj"
	@-mkdir obj

# executables

map-server: obj $(MAP_DIR_OBJ) $(COMMON_AR) $(LIBCONFIG_AR)
	@echo "	LD	map-server"
	@gcc  -L/usr/lib -o ../../map-server $(MAP_DIR_OBJ) $(COMMON_AR) $(MT19937AR_OBJ) $(LIBCONFIG_AR) -lpthread -lm -lz  -ldl  -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl


# map object files

obj/%.o: %.c $(MAP_H) $(COMMON_H)  $(MT19937AR_H) $(LIBCONFIG_H)
	@echo "	CC	$<"
	@gcc -flto -fuse-linker-plugin -g -O2 -pipe -ffast-math -Wall -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-clobbered -Wempty-body -Wno-switch -Wno-missing-field-initializers -Wshadow -fPIC -fno-strict-aliasing $(COMMON_INCLUDE) $(MT19937AR_INCLUDE) $(LIBCONFIG_INCLUDE) $(PCRE_CFLAGS) -I/usr/include/mysql  -DMAXCONN=16384 -I../common -DHAS_TLS -DHAVE_SETRLIMIT -DHAVE_STRNLEN  -I/usr/include -DHAVE_MONOTONIC_CLOCK -c $(OUTPUT_OPTION) $<

# missing object files
$(COMMON_AR):
	@$(MAKE) -C ../common server

$(MT19937AR_OBJ):
	@$(MAKE) -C ../../3rdparty/mt19937ar

$(LIBCONFIG_AR):
	@$(MAKE) -C ../../3rdparty/libconfig
