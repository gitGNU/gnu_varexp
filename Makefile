# Build libvarexp.

CC		= gcc
AR		= ar
RANLIB 		= ranlib

WARNFLAGS	= -Wall -pedantic
OPTFLAGS	= -O3 -pipe

CPPFLAGS	=
CFLAGS		=
LDFLAGS		=

OBJS		= expand-named-characters.o expand-character-class.o command.o \
		  expression.o variable.o text.o expand.o input.o tokenbuf.o   \
		  search-and-replace.o cut-out-offset.o transpose.o padding.o

.c.o:
	$(CC) $(CPPFLAGS) $(WARNFLAGS) $(OPTFLAGS) $(CFLAGS) -c $<

all:	libvarexp.a

libvarexp.a:	$(OBJS)
	@rm -f $@
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@

clean::
	@(cd regression-tests && $(MAKE) clean)
	rm -f $(OBJS)
	rm -f libvarexp.a

check::
	(cd regression-tests && $(MAKE) check)

# Dependencies

command.o: internal.h varexp.h
cut-out-offset.o: internal.h varexp.h
expand-character-class.o: internal.h varexp.h
expand-named-characters.o: internal.h varexp.h
expand.o: internal.h varexp.h
expression.o: internal.h varexp.h
input.o: internal.h varexp.h
padding.o: internal.h varexp.h
search-and-replace.o: internal.h varexp.h
text.o: internal.h varexp.h
tokenbuf.o: internal.h varexp.h
transpose.o: internal.h varexp.h
variable.o: internal.h varexp.h
