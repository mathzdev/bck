
#COMMON_OBJ = $(ls *.c | grep -viw sql.c | sed -e "s/\.c/\.o/g")
COMMON_OBJ = core.o socket.o timer.o db.o nullpo.o malloc.o showmsg.o strlib.o utils.o \
	grfio.o mapindex.o ers.o md5calc.o minicore.o minisocket.o minimalloc.o random.o des.o \
	conf.o thread.o mutex.o raconf.o mempool.o msg_conf.o cli.o sql.o
COMMON_DIR_OBJ = $(COMMON_OBJ:%=obj/%)
COMMON_H = $(shell ls ../common/*.h)
COMMON_AR = obj/common.a

MT19937AR_OBJ = ../../3rdparty/mt19937ar/mt19937ar.o
MT19937AR_H = ../../3rdparty/mt19937ar/mt19937ar.h
MT19937AR_INCLUDE = -I../../3rdparty/mt19937ar

LIBCONFIG_H = $(shell ls ../../3rdparty/libconfig/*.h)
LIBCONFIG_AR = ../../3rdparty/libconfig/obj/libconfig.a
LIBCONFIG_INCLUDE = -I../../3rdparty/libconfig

HAVE_MYSQL=yes
ifeq ($(HAVE_MYSQL),yes)
	SERVER_DEPENDS=common
else
	SERVER_DEPENDS=needs_mysql
endif
ALL_DEPENDS=server



#####################################################################
.PHONY : all server clean help

all: $(ALL_DEPENDS)

server: $(SERVER_DEPENDS)

clean:
	@echo "	CLEAN	common"
	@rm -rf *.o obj

help:
	@echo "possible targets are 'server' 'all' 'clean' 'help'"
	@echo "'server' - builds object files used in servers"
	@echo "'all'    - builds all above targets"
	@echo "'clean'  - cleans builds and objects"
	@echo "'help'   - outputs this message"

#####################################################################

needs_mysql:
	@echo "MySQL not found or disabled by the configure script"
	@exit 1

obj:
	@echo "	MKDIR	obj"
	@-mkdir obj

$(COMMON_AR): $(COMMON_DIR_OBJ)
	@echo "	AR	$@"
	@/usr/bin/ar rcs $(COMMON_AR) $(COMMON_DIR_OBJ)

common: obj $(COMMON_DIR_OBJ) $(MT19937AR_OBJ) $(LIBCONFIG_AR) $(COMMON_AR)

obj/%.o: %.c $(COMMON_H) $(MT19937AR_H) $(LIBCONFIG_H)
	@echo "	CC	$<"
	@gcc -flto -fuse-linker-plugin -ffat-lto-objects -flto -fuse-linker-plugin -g -O2 -pipe -ffast-math -Wall -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-clobbered -Wempty-body -Wno-switch -Wno-missing-field-initializers -Wshadow -fPIC -fno-strict-aliasing $(MT19937AR_INCLUDE) $(LIBCONFIG_INCLUDE) -I/usr/include/mysql  -DMAXCONN=16384 -I../common -DHAS_TLS -DHAVE_SETRLIMIT -DHAVE_STRNLEN  -I/usr/include -DHAVE_MONOTONIC_CLOCK -c $(OUTPUT_OPTION) $<

obj/mini%.o: %.c $(COMMON_H) $(MT19937AR_H) $(LIBCONFIG_H)
	@echo "	CC	$<"
	@gcc -flto -fuse-linker-plugin -ffat-lto-objects -flto -fuse-linker-plugin -g -O2 -pipe -ffast-math -Wall -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-clobbered -Wempty-body -Wno-switch -Wno-missing-field-initializers -Wshadow -fPIC -fno-strict-aliasing $(MT19937AR_INCLUDE) $(LIBCONFIG_INCLUDE) -I/usr/include/mysql -DMINICORE  -DMAXCONN=16384 -I../common -DHAS_TLS -DHAVE_SETRLIMIT -DHAVE_STRNLEN  -I/usr/include -DHAVE_MONOTONIC_CLOCK -c $(OUTPUT_OPTION) $<

# missing object files
$(MT19937AR_OBJ):
	@$(MAKE) -C ../../3rdparty/mt19937ar

$(LIBCONFIG_AR):
	@$(MAKE) -C ../../3rdparty/libconfig
