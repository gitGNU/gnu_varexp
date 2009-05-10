#
# Build libvarexp
#

lib_LIBRARIES      = libvarexp.a
libvarexp_a_SOURCES   = command.cpp cut-out-offset.cpp          \
			expand-character-class.cpp unescape.cpp \
			expand.cpp expression.cpp input.cpp     \
			loop.cpp num-exp.cpp padding.cpp        \
			search-and-replace.cpp text.cpp         \
			transpose.cpp variable.cpp error.cpp	\
			parser.cpp
include_HEADERS       = varexp.hpp
noinst_HEADERS        = internal.hpp
EXTRA_DIST            = LICENSE README libvarexp.html
MAINTAINERCLEANFILES  = README readme.txt readme.html libvarexp.html
SUBDIRS               = tests
data_DATA	      = libvarexp.html

libvarexp.html:
	docbook2html -u >$@ documentation/user-manual.sgml

README:
	docbook2txt -u documentation/readme.sgml
	@mv readme.txt $@
	@rm readme.html
