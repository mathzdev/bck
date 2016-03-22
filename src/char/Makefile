
COMMON_H = $(shell ls ../common/*.h)
COMMON_AR = ../common/obj/common.a
COMMON_INCLUDE = -I../common/

MT19937AR_OBJ = ../../3rdparty/mt19937ar/mt19937ar.o
MT19937AR_H = ../../3rdparty/mt19937ar/mt19937ar.h
MT19937AR_INCLUDE = -I../../3rdparty/mt19937ar

LIBCONFIG_H = $(shell ls ../../3rdparty/libconfig/*.h)
LIBCONFIG_AR = ../../3rdparty/libconfig/obj/libconfig.a
LIBCONFIG_INCLUDE = -I../../3rdparty/libconfig

COMMON_OBJ = ../common/obj/sql.o
COMMON_H = ../common/sql.h

CHAR_OBJ = $(shell ls *.c | sed -e "s/\.c/\.o/g")
CHAR_DIR_OBJ = $(CHAR_OBJ:%=obj/%)
CHAR_H = $(shell ls ../char/*.h)

HAVE_MYSQL=yes
ifeq ($(HAVE_MYSQL),yes)	
	SERVER_DEPENDS=char-server
else
	SERVER_DEPENDS=needs_mysql
endif
ALL_DEPENDS=server



#####################################################################
.PHONY : all server clean help

all: $(ALL_DEPENDS)

server: $(SERVER_DEPENDS)

clean:
	@echo "	CLEAN	char"
	@rm -rf *.o obj ../../char-server

help:
	@echo "possible targets are 'server' 'all' 'clean' 'help'"
	@echo "'server'    - char server"
	@echo "'all'       - builds all above targets"
	@echo "'clean'     - cleans builds and objects"
	@echo "'help'      - outputs this message"

#####################################################################


char-server: obj $(CHAR_DIR_OBJ) $(LIBCONFIG_AR) $(COMMON_AR)
	@echo "	LD	char-server"
	@gcc  -L/usr/lib -o ../../char-server $(CHAR_DIR_OBJ) $(LIBCONFIG_AR) $(COMMON_AR) $(MT19937AR_OBJ) $(LIBCONFIG_OBJ) -lpthread -lm -lz  -ldl -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl

needs_mysql:
	@echo "MySQL not found or disabled by the configure script"
	@exit 1

obj:
	@echo "	MKDIR	obj"
	@-mkdir obj

obj/%.o: %.c $(CHAR_H) $(COMMON_H) $(MT19937AR_H) $(LIBCONFIG_H)
	@echo "	CC	$<"
	@gcc -flto -fuse-linker-plugin -g -O2 -pipe -ffast-math -Wall -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-clobbered -Wempty-body -Wno-switch -Wno-missing-field-initializers -Wshadow -fPIC -fno-strict-aliasing $(COMMON_INCLUDE) $(MT19937AR_INCLUDE) $(LIBCONFIG_INCLUDE) -I/usr/include/mysql  -DMAXCONN=16384 -I../common -DHAS_TLS -DHAVE_SETRLIMIT -DHAVE_STRNLEN  -I/usr/include -DHAVE_MONOTONIC_CLOCK -c $(OUTPUT_OPTION) $<

# missing object files
$(COMMON_AR):
	@$(MAKE) -C ../common server

$(MT19937AR_OBJ):
	@$(MAKE) -C ../../3rdparty/mt19937ar

$(LIBCONFIG_AR):
	@$(MAKE) -C ../../3rdparty/libconfig
