#
# Run varexp regression tests.
#
# This Makefile assumes you are using the GNU compiler. Please change
# these defines if you are not.
#
CXX		= g++
AR		= ar
RANLIB		= ranlib

OPTIMFLAGS	= -O3
WARNFLAGS	= -Wall -pedantic
CPPFLAGS	=
CXXFLAGS	=
LDFLAGS		=

TESTS		= unescape.exe expand-character-class.exe expand1.exe expand2.exe \
                  expand3.exe expand4.exe expand5.exe expand6.exe                 \
                  empty-search-pattern.exe offset-failure.exe

.SUFFIXES:
.SUFFIXES:	.cc .exe

.cc.exe:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(WARNFLAGS) $(OPTIMFLAGS) $(LDFLAGS) $< ../libvarexp.a -o $@

check all::	../libvarexp.a $(TESTS)
	$(SHELL) .run-tests $(TESTS)

../libvarexp.a:
	(cd .. && $(MAKE) libvarexp.a)

realclean distclean clean::
	@find . -name '*.rpo' -exec rm -f {} \;
	@find . -name '*.exe' -exec rm -f {} \;
	@find . -name '*.o'   -exec rm -f {} \;
	@find . -name '.#*'   -exec rm -f {} \;
	@echo All dependent files have been removed.

# Dependencies

$(TESTS): ../varexp.hh
