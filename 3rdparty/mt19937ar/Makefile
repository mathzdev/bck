
MT19937AR_OBJ = mt19937ar.o
MT19937AR_H = mt19937ar.h



#####################################################################
.PHONY : all clean help

all: $(MT19937AR_OBJ)

clean:	
	@echo "	CLEAN	mt19937ar"
	@rm -rf *.o

help:
	@echo "possible targets are 'all' 'clean' 'help'"
	@echo "'all'    - builds $(MT19937AR_OBJ)"
	@echo "'clean'  - deletes $(MT19937AR_OBJ)"
	@echo "'help'   - outputs this message"

#####################################################################

%.o: %.c $(MT19937AR_H)
	@echo "	CC	$<"
	@gcc -flto -fuse-linker-plugin -ffat-lto-objects -flto -fuse-linker-plugin -g -O2 -pipe -ffast-math -Wall -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-clobbered -Wempty-body -Wno-switch -Wno-missing-field-initializers -Wshadow -fPIC -fno-strict-aliasing  -DMAXCONN=16384 -I../common -DHAS_TLS -DHAVE_SETRLIMIT -DHAVE_STRNLEN  -I/usr/include -DHAVE_MONOTONIC_CLOCK -c $(OUTPUT_OPTION) $<
