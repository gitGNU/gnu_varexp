dnl configure.ac for libvarexp

AC_INIT([varexp], m4_esyscmd([build-aux/git-version-gen .tarball-version]), [simons@cryp.to], [varexp], [http://savannah.nongnu.org/projects/varexp/])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 -Wall gnu])
AC_CONFIG_SRCDIR([varexp.hpp])
AC_COPYRIGHT([dnl
Copyright (c) 2002-2010 Peter Simons <simons@cryp.to>
Copyright (c) 2001 The OSSP Project <http://www.ossp.org/>
Copyright (c) 2001 Cable & Wireless Deutschland <http://www.cw.com/de/>

Permission to use, copy, modify, and distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright notice
and this permission notice appear in all copies.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS
AND COPYRIGHT HOLDERS AND THEIR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.])

AC_MSG_NOTICE([Variable Expression Library version $VERSION])

AC_PROG_CXX
AC_PROG_RANLIB

AC_CONFIG_FILES([Makefile tests/Makefile])
AC_OUTPUT

dnl Responsibility shifts to the user after this point.
