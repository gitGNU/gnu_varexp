  <para>In addition to just inserting the variable's contents into the buffer,
you can use various operations to modify its contents before the expression is
expanded. Such operations are used by appending a colon plus the apropriate
command character to the variable name in complex expression, for example:
<quote>${FOOBAR:l}</quote>. Furthermore, you can chain any number of operations
simply by appending another command to the last one:
<quote>${FOOBAR:l:u:l:u:&hellip;}</quote>.</para>

  <para>The supported operations are:</para>

  <variablelist>
    <varlistentry>
      <term><literal>${NAME:#}</literal></term>
      <listitem>
        <para>This operation will expand the expression to the length of the
contents of <literal>$NAME</literal>. If, for example, <literal>$FOO</literal>
is <quote>foobar</quote>, then <literal>${FOO:#}</literal> will result in
<quote>6</quote>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>${NAME:l}</literal></term>
      <listitem>
        <para>This operation will turn the contents of <literal>$NAME</literal>
to all lower-case, using the system routine
<citerefentry><refentrytitle>tolower</refentrytitle><manvolnum>3</manvolnum></citerefentry>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>${NAME:u}</literal></term>
      <listitem>
        <para>This operation will turn the contents of <literal>$NAME</literal>
to all upper-case, using the system routine
<citerefentry><refentrytitle>toupper</refentrytitle><manvolnum>3</manvolnum></citerefentry>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>${NAME:*<replaceable>word</replaceable>}</literal></term>
      <listitem>
        <para>This operation will expand to <replaceable>word</replaceable> if
<literal>$NAME</literal> is empty. If <literal>$NAME</literal> is not empty, it
will expand to an empty string.</para>

        <para><replaceable>word</replaceable> can be an arbitrary text. In
particular, it may contain other variables or even complex variable expressions,
for example: <quote>${FOO:*${BAR:u}}</quote>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>${NAME:-<replaceable>word</replaceable>}</literal></term>
      <listitem>
        <para>This operation will expand to <replaceable>word</replaceable> if
<literal>$NAME</literal> is empty. If <literal>$NAME</literal> is not empty, it
will evaluate to the <literal>$NAME</literal>'s contents.</para>

        <para><replaceable>word</replaceable> can be an arbitrary text. In
particular, it may contain other variables or even complex variable expressions,
for example: <quote>${FOO:-${BAR:u}}</quote>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>${NAME:+<replaceable>word</replaceable>}</literal></term>
      <listitem>
        <para>This operation will expand to <replaceable>word</replaceable> if
<literal>$NAME</literal> is not empty. If <literal>$NAME</literal> is empty, it
will expand to an empty string.</para>

        <para><replaceable>word</replaceable> can be an arbitrary text. In
particular, it may contain other variables or even complex variable expressions,
for example: <quote>${FOO:+${BAR:u}}</quote>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>${NAME:o<replaceable>start</replaceable>,<replaceable>end</replaceable>}</literal></term>
      <listitem>
        <para>This operation will expand to a part of <literal>$NAME</literal>'s
contents, which starts at <replaceable>start</replaceable> and ends at
<replaceable>end</replaceable>. Both parameters <replaceable>start</replaceable>
and <replaceable>end</replaceable> are unsigned numbers.</para>

        <para>Please note that the character at position
<replaceable>end</replaceable> is <emphasis>included</emphasis> in the result;
<quote>${FOOBAR:o3,4}</quote>, for instance, will return a two-character string.
Also, please note that start positions begin at zero (0)!</para>

        <para>If the <replaceable>end</replaceable> parameter is not specified,
as in <quote>${FOOBAR:o3,}</quote>, the operation will return the string
starting from position 3 to the end of the string.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>${NAME:o<replaceable>start</replaceable>-<replaceable>length</replaceable>}</literal></term>
      <listitem>
        <para>This operation will expand to a part of <literal>$NAME</literal>'s
contents, which starts at <replaceable>start</replaceable> and ends at
<quote><replaceable>start</replaceable>+<replaceable>length</replaceable></quote>.
Both parameters <replaceable>start</replaceable> and
<replaceable>end</replaceable> are unsigned numbers.</para>

        <para><quote>${FOOBAR:o3-4}</quote>, for example, means to return the
next 4 charaters starting at position 3 in the string. Please note that start
positions begin at zero (0)!</para>

        <para>If the <replaceable>end</replaceable> parameter is left out, as in
<quote>${FOOBAR:o3-}</quote>, the operation will return the string from position
3 to the end.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>${NAME:s/<replaceable>pattern</replaceable>/<replaceable>string</replaceable>/<replaceable>gti</replaceable>}</literal></term>
      <listitem>
        <para>This operation will perform a search-and-replace operation on the
contents of <literal>$NAME</literal> and return the result. The behavior of the
search-and-replace may be modified by the following flags: If a
<literal>t</literal> flag has been provided, a plain text search-and-replace is
performed, otherwise, the default is to do a regular expression
search-and-replace as in the system utility
<citerefentry><refentrytitle>sed</refentrytitle><manvolnum>1</manvolnum></citerefentry>.
If the <literal>g</literal> flag has been provided, the search-and-replace will
replace <emphasis>all</emphasis> instances of <replaceable>pattern</replaceable>
by <replaceable>replace</replaceable>, instead of replacing only the first
instance (the default). If the <literal>i</literal> flag has been provided, the
search-and-replace will take place case-insensitively, otherwise, the default is
to search case-sensitively.</para>

        <para>The parameters <replaceable>pattern</replaceable> and
<replaceable>replace</replaceable> can be an arbitrary text. In particular, they
may contain other variables or even complex variable expressions, for example:
<quote>${FOO:s/${BAR:u}/$FOO/ti}</quote>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>${NAME:y/<replaceable>ochars</replaceable>/<replaceable>nchars</replaceable>/}</literal></term>
      <listitem>
        <para>This operation will translate all characters in the contents of
<literal>$NAME</literal> that are found in the <replaceable>ochars</replaceable>
class to the corresponding character in the <replaceable>nchars</replaceable>
class -- just like the system utility
<citerefentry><refentrytitle>tr</refentrytitle><manvolnum>1</manvolnum></citerefentry>
does. Both <replaceable>ochars</replaceable> and
<replaceable>nchars</replaceable> may contain character range specifications,
for example <quote>a-z0-9</quote>. A hyphon as the first or last character of
the class specification is interpreted literally. Both the
<replaceable>ochars</replaceable> and the <replaceable>nchars</replaceable>
class must contain the same number of characters after all ranges are expanded,
or an error is returned.</para>

        <para>If, for example, <quote>$FOO</quote> contains
<quote>foobar</quote>, then <quote>${FOO:y/a-z/A-Z/}</quote> would yield
<quote>FOOBAR</quote>. Another goodie is to use that operation to ROT13-encrypt
or decrypt a string with the expression
<quote>${FOO:y/a-z/n-za-m/}</quote>.</para>

        <para>The parameters <replaceable>ochars</replaceable> and
<replaceable>nchars</replaceable> can be an arbitrary text. In particular, they
may contain other variables or even complex variable expressions, for example:
<quote>${FOO:y/${BAR:u}/$TEST/}</quote>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>${NAME:p/<replaceable>width</replaceable>/<replaceable>string</replaceable>/<replaceable>align</replaceable>}</literal></term>
      <listitem>
        <para>This operation will pad the contents of <literal>$NAME</literal>
with <replaceable>string</replaceable> according to the
<replaceable>align</replaceable> parameter, so that the result is at least
<replaceable>width</replaceable> characters long. Valid parameters for align are
<literal>l</literal> (left), <literal>r</literal> (right), or
<literal>c</literal> (center). The <replaceable>string</replaceable> parameter
may contain multiple characters, if you see any use for that.</para>

        <para>If, for example, <quote>$FOO</quote> is <quote>foobar</quote>,
then <quote>${FOO:p/20/./c}</quote> would yield
<quote>.......foobar.......</quote>; <quote>${FOO:p/20/./l}</quote> would yield
<quote>foobar..............</quote>; and <quote>${FOO:p/20/./r}</quote> would
yield <quote>..............foobar</quote>;</para>

        <para>The parameter <replaceable>string</replaceable> can be an
arbitrary text. In particular, it may contain other variables or even complex
variable expressions, for example: <quote>${FOO:p/20/${BAR}/r/}</quote>.</para>
      </listitem>
    </varlistentry>
  </variablelist>

<!--
Local Variables:
mode: sgml
fill-column:80
sgml-parent-document:("user-manual.sgml" "article" "sect1" "sect2")
End:
-->
