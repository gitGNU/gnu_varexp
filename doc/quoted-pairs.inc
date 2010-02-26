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

  <para>In addition to the variable expressions discussed in the previous
sections, &varexp; can also be used to expand so called <quote>quoted
pairs</quote> in the text. Quoted pairs are well-known from programming
languages like C, for example. A quoted pair consists of the backslash followed
by another character, for example: <quote>\n</quote>.</para>

  <para><emphasis>Any</emphasis> character can be quoted by a backslash; the
terms <quote>\=</quote> or <quote>\@</quote>, for instance, are valid quoted
pairs. But these quoted pairs don't have any special meaning to the library and
will be expanded to the quoted character itself. There is a number of quoted
pairs, though, that does have a special meaning and expands to some other value.
The complete list is shown below. Please note that the name <quote>quoted
pair</quote> is actually a bit inaccurate, because &varexp; supports some
expressions that are no <quote>pairs</quote> in the sense that they consist of
more than one quoted character. But the name <quote>quoted pair</quote> is very
common for them anyway, so I stuck with it.</para>

  <para>The quoted pairs supported by &varexp; are:</para>

  <variablelist>
    <varlistentry>
      <term><symbol>\t</symbol></term>
      <term><symbol>\r</symbol></term>
      <term><symbol>\n</symbol></term>
      <listitem>
        <para>These expressions are replaced by a <keysym>tab</keysym>, a
<keysym>carrige return</keysym> and a <keysym>newline</keysym>
respectively.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term>\<replaceable>abb</replaceable></term>
      <listitem>
        <para>This expression is replaced by the value of the octal number
<replaceable>abb</replaceable>. Valid digits for <replaceable>a</replaceable>
are in the range from 0 to 3; either position <replaceable>b</replaceable> may
be in the range from 0 to 7. Please note that an octal expression is recognized
only if the backslash is followed by <emphasis>three</emphasis> valid digits!
The expression <quote>\1a7</quote>, for example, is interpreted as the quoted
pair <quote>\1</quote> followed by the verbatim text <quote>a7</quote>, because
<quote>a</quote> is not valid for octal numbers.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><symbol>\x<replaceable>aa</replaceable></symbol></term>
      <listitem>
        <para>This expression is replaced by the value of the hexadecimal number
$<replaceable>aa</replaceable>. Both positions <replaceable>a</replaceable> must
be in the range from 0 to 9 or from <quote>a</quote> to <quote>f</quote>. For
the letters, either case is recognized, so <quote>\xBB</quote> and
<quote>\xbb</quote> will yield the same result.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><symbol>\x{&hellip;}</symbol></term>
      <listitem>
        <para>This expression denotes a set of grouped hexadecimal numbers. The
<replaceable>&hellip;</replaceable> part may consist of an arbitrary number of
hexadecimal pairs, such as in <quote>\x{}</quote>, <quote>\x{ff}</quote>, or
<quote>\x{55ffab04}</quote>. The empty expression <quote>\x{}</quote> is a
no-op; it will not produce any output.</para>

        <para>This construct may be useful to specify multi-byte characters (as
in Unicode). <quote>\x{0102}</quote> is effectively equivalent to
<quote>\x01\x02</quote>, but the grouping of values may be useful in other
contexts, even though for &varexp; it makes no difference.</para>
      </listitem>
    </varlistentry>
  </variablelist>

<!--
Local Variables:
mode: xml
fill-column:80
End:
-->
