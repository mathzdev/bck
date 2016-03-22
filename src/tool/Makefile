
COMMON_OBJ = minicore.o malloc.o showmsg.o strlib.o utils.o des.o grfio.o
COMMON_DIR_OBJ = $(COMMON_OBJ:%=../common/obj/%)
COMMON_H = $(shell ls ../common/*.h)
COMMON_INCLUDE = -I../common/

LIBCONFIG_H = $(shell ls ../../3rdparty/libconfig/*.h)
LIBCONFIG_AR = ../../3rdparty/libconfig/obj/libconfig.a
LIBCONFIG_INCLUDE = -I../../3rdparty/libconfig

OTHER_H = ../config/renewal.h

MAPCACHE_OBJ = obj_all/mapcache.o



#####################################################################
.PHONY : all mapcache clean help

all: mapcache

mapcache: obj_all $(MAPCACHE_OBJ) $(COMMON_DIR_OBJ) $(LIBCONFIG_OBJ)
	@echo "	LD	$@"
	@gcc  -L/usr/lib -o ../../mapcache $(MAPCACHE_OBJ) $(COMMON_DIR_OBJ) $(LIBCONFIG_AR) -lpthread -lm -lz  -ldl

clean:
	@echo "	CLEAN	tool"
	@rm -rf obj_all/*.o ../../mapcache

help:
	@echo "possible targets are 'mapcache' 'all' 'clean' 'help'"
	@echo "'mapcache'  - mapcache generator"
	@echo "'all'       - builds all above targets"
	@echo "'clean'     - cleans builds and objects"
	@echo "'help'      - outputs this message"

#####################################################################

obj_all:
	-mkdir obj_all

obj_all/%.o: %.c $(COMMON_H) $(OTHER_H) $(LIBCONFIG_H)
	@echo "	CC	$<"
	@gcc -flto -fuse-linker-plugin -g -O2 -pipe -ffast-math -Wall -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-clobbered -Wempty-body -Wno-switch -Wno-missing-field-initializers -Wshadow -fPIC -fno-strict-aliasing $(COMMON_INCLUDE) $(LIBCONFIG_INCLUDE)  -DMAXCONN=16384 -I../common -DHAS_TLS -DHAVE_SETRLIMIT -DHAVE_STRNLEN  -I/usr/include -DHAVE_MONOTONIC_CLOCK -c $(OUTPUT_OPTION) $<

# missing common object files
$(COMMON_DIR_OBJ):
	@$(MAKE) -C ../common server

$(LIBCONFIG_AR):
	@$(MAKE) -C ../../3rdparty/libconfig
