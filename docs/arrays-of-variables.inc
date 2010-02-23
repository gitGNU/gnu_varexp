<!--
   Copyright (c) 2002-2010 Peter Simons <simons@cryp.to>
   Copyright (c) 2001 The OSSP Project <http://www.ossp.org/>
   Copyright (c) 2001 Cable & Wireless Deutschland <http://www.cw.com/de/>

   Permission to use, copy, modify, and distribute this software for any purpose
   with or without fee is hereby granted, provided that the above copyright
   notice and this permission notice appear in all copies.

   THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
   FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
   AUTHORS AND COPYRIGHT HOLDERS AND THEIR CONTRIBUTORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-->

  <para>In addition to normal variables, &varexp; also supports arrays of
variables. An array may only be accessed in a complex expression --
<quote>$NAME[1]</quote> is not correct syntax. Use <quote>${NAME[1]}</quote>
instead. The reason for this limitation is that the brackets used to specify the
index (<quote>[</quote> and <quote>]</quote>) have a different meaning in
ordinary text; see <xref linkend="looping"/> for further discussion.</para>

  <para>Which variables are arrays -- and which are not -- is entirely up to the
application developer. In some applications, every variable may be accessed as
both a normal variable and an array. In other applications, normal variables and
arrays are different things. &varexp; does not dictate this. There exists the
convention that accessing an array with a negative index, such as
<quote>${ARRAY[-1]}</quote> should return the number of elements the array
contains. But again, this is not a behavior required by &varexp;; different
applications may behave differently here.</para>

  <para>When specifying the index of the array's element you wish to access, you
can use complete arithmetic expressions to calculate the entry. &varexp;
supports the operands <quote>+</quote> (addition), <quote>-</quote>
(subtractin), <quote>*</quote> (multiplication), <quote>/</quote> (division),
and <quote>+</quote> (modulo).</para>

  <para>These operations may be used on any signed integer. A valid expression
is, for example: <quote>${ARRAY[-12/4+5]}</quote>. Please note that &varexp;
follows the usual operator precedence. To group expressions explicitely, put
brackets around them: <quote>${ARRAY[-12/(2+4)]}</quote>.</para>

  <para>In any place you can write a number in such an expression, you can also
use a simple or complex variable expression. If <quote>$TWO</quote> is
<quote>2</quote>, the following expression would access the 5th entry in the
<quote>$FOO</quote> array: <quote>${FOO[10/$TWO}</quote>.</para>

<!--
Local Variables:
mode: xml
fill-column:80
End:
-->
