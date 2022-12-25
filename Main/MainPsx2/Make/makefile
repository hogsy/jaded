#
# general
#
MAKEFILE    = makefile
SHELL       = /bin/bash


#
# targets
#
.SILENT:

.PHONY: ee iop clean_ee clean_iop all clean ee_release ee_debug iop_release iop_debug

all: ee iop

clean: clean_ee clean_iop

#
# EE targets
#
ee_release:
	make -f eeMakefile.mak release

ee_debug:
	make -f eeMakefile.mak debug

ee:
	make -f eeMakefile.mak all

clean_ee:
	make -f eeMakefile.mak clean


#
# IOP targets
#
iop_debug:
	make -f iopMakefile.mak debug

iop_release:
	make -f iopMakefile.mak release

iop:
	make -f iopMakefile.mak all

clean_iop:
	make -f iopMakefile.mak clean
