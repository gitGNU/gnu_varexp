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
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(WARNFLAGS) $(OPTIMFLAGS) $(LDFLAGS) $< -o $@

check all::	$(TESTS)
	$(SHELL) .run-tests $(TESTS)

realclean distclean clean::
	@find . -name '*.rpo' -exec rm -f {} \;
	@find . -name '*.exe' -exec rm -f {} \;
	@find . -name '*.o'   -exec rm -f {} \;
	@find . -name '.#*'   -exec rm -f {} \;
	@echo All dependent files have been removed.

depend::
	makedepend -Y -fMakefile '-s# Dependencies' `find . -name '*.cc' ` 2>/dev/null

# Dependencies

empty-search-pattern.o: ../varexp.hh
expand-character-class.o: ../internal.hh ../varexp.hh
expand1.o: ../varexp.hh
expand2.o: ../varexp.hh
expand3.o: ../varexp.hh
expand4.o: ../varexp.hh
expand5.o: ../varexp.hh
expand6.o: ../varexp.hh
offset-failure.o: ../varexp.hh
unescape.o: ../varexp.hh
