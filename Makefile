#
# Build libvarexp.a.
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

OBJS		= command.o cut-out-offset.o error.o expand-character-class.o expand.o \
		  expression.o input.o loop.o num-exp.o padding.o parser.o	       \
		  search-and-replace.o text.o transpose.o unescape.o variable.o

.SUFFIXES:
.SUFFIXES:	.cc .o

.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(WARNFLAGS) $(OPTIMFLAGS) -c $< -o $@

all:		libvarexp.a

libvarexp.a:	$(OBJS)
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@

realclean distclean clean::
	@find . -name '*.rpo' -exec rm -f {} \;
	@find . -name '*.a'   -exec rm -f {} \;
	@find . -name '*.o'   -exec rm -f {} \;
	@find . -name '.#*' -exec rm -f {} \;
	@echo All dependent files have been removed.

depend::
	makedepend -Y -fMakefile '-s# Dependencies' `find . -name '*.cc' ` 2>/dev/null
	@sed <Makefile >Makefile.bak -e 's/\.\///g'
	@mv Makefile.bak Makefile

changelog::
	rcs2log -u "peti	Peter Simons	simons@computer.org" >ChangeLog.new
	mv ChangeLog ChangeLog.old
	cat ChangeLog.new ChangeLog.old >ChangeLog
	rm ChangeLog.old ChangeLog.new

# dist:		mapson documentation/man-page.txt documentation/mapson.html \
# 		documentation/mapson.pdf VERSION README
# 	@V=`cat VERSION`; \
# 	cp -ra . /tmp/mapson-$$V; \
# 	$(MAKE) >/dev/null -C /tmp/mapson-$$V distclean; \
# 	find /tmp/mapson-$$V -name 'Odinfile' -exec rm -f {} \; ; \
# 	find /tmp/mapson-$$V -name 'CVS' -print | xargs rm -rf \; ; \
# 	find /tmp/mapson-$$V -name '.cvsignore' -exec rm -f {} \; ; \
# 	(cd /tmp; tar cfz mapson-$$V.tar.gz mapson-$$V); \
# 	rm -rf /tmp/mapson-$$V; \
# 	echo Created distribution archive /tmp/mapson-$$V.tar.gz

# Dependencies

command.o: internal.hh varexp.hh
cut-out-offset.o: internal.hh varexp.hh
error.o: varexp.hh
expand-character-class.o: internal.hh varexp.hh
expand.o: internal.hh varexp.hh
expression.o: internal.hh varexp.hh
input.o: internal.hh varexp.hh
loop.o: internal.hh varexp.hh
num-exp.o: internal.hh varexp.hh
padding.o: internal.hh varexp.hh
parser.o: internal.hh varexp.hh
search-and-replace.o: internal.hh varexp.hh
text.o: internal.hh varexp.hh
transpose.o: internal.hh varexp.hh
unescape.o: internal.hh varexp.hh
variable.o: internal.hh varexp.hh
regression-tests/empty-search-pattern.o: varexp.hh
regression-tests/expand-character-class.o: internal.hh varexp.hh
regression-tests/expand1.o: varexp.hh
regression-tests/expand2.o: varexp.hh
regression-tests/expand3.o: varexp.hh
regression-tests/expand4.o: varexp.hh
regression-tests/expand5.o: varexp.hh
regression-tests/expand6.o: varexp.hh
regression-tests/offset-failure.o: varexp.hh
regression-tests/unescape.o: varexp.hh
