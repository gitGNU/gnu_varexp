# Regression tests for libvarexp.

CC		= gcc
AR		= ar
RANLIB 		= ranlib

WARNFLAGS	= -Wall -pedantic
OPTFLAGS	= -O3 -pipe

CPPFLAGS	=
CFLAGS		=
LDFLAGS		=

.SUFFIXES: 	.exe

TESTS		=  expand-named-characters.exe expand-character-class.exe \
		   expand1.exe expand2.exe force-expand.exe expand3.exe \
		   expand4.exe expand5.exe expand6.exe empty-search-pattern.exe \
		   offset-failure.exe

.c.exe:
	$(CC) $(CPPFLAGS) $(WARNFLAGS) $(OPTFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< -L.. -lvarexp

check:
	@(cd .. && $(MAKE) libvarexp.a)
	@$(MAKE) $(TESTS)
	@./.run-tests $(TESTS)

clean::
	rm -f $(TESTS)

# Dependencies

empty-search-pattern.exe: ../internal.h ../varexp.h
expand-character-class.exe: ../internal.h ../varexp.h
expand-named-characters.exe: ../internal.h ../varexp.h
expand1.exe: ../internal.h ../varexp.h
expand2.exe: ../internal.h ../varexp.h
expand3.exe: ../internal.h ../varexp.h
expand4.exe: ../internal.h ../varexp.h
expand5.exe: ../internal.h ../varexp.h
expand6.exe: ../internal.h ../varexp.h
force-expand.exe: ../internal.h ../varexp.h
offset-failure.exe: ../internal.h ../varexp.h
