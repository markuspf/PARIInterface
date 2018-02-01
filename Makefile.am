#
# PARIInterface: Interface to PARI
#
# This file is part of the build system of a GAP kernel extension.
# Requires automake.
#
ACLOCAL_AMFLAGS = -I cnf/m4

BINARCHDIR = bin/$(GAPARCH)
GAPINSTALLLIB = $(BINARCHDIR)/PARIInterface.so

lib_LTLIBRARIES = PARIInterface.la

PARIInterface_la_SOURCES = src/PARIInterface.c
PARIInterface_la_CPPFLAGS = $(GAP_CPPFLAGS) -DCONFIG_H
PARIInterface_la_LDFLAGS = -module -avoid-version -lpari
if SYS_IS_CYGWIN
PARIInterface_la_LDFLAGS += -no-undefined -version-info 0:0:0 
PARIInterface_la_LDFLAGS += -Wl,$(GAPROOT)/bin/$(GAPARCH)/gap.dll
endif

all-local: $(GAPINSTALLLIB)

$(GAPINSTALLLIB): PARIInterface.la
	$(mkdir_p) $(BINARCHDIR)
if SYS_IS_CYGWIN
	cp .libs/PARIInterface.dll $(GAPINSTALLLIB)
else
	cp .libs/PARIInterface.so $(GAPINSTALLLIB)
endif

clean-local:
	rm -f $(GAPINSTALLLIB)

distclean-local:
	rm -rf bin/*

doc:
	($(GAPROOT)/bin/gap.sh -A makedoc.g)
