  <para>In addition to normal variables, &varexp; also supports arrays of
variables. An array may only be accessed in a complex expression --
<quote>$NAME[1]</quote> is not correct syntax. Use <quote>${NAME[1]}</quote>
instead. The reason for this limitation is that the brackets used to specify the
index (<quote>[</quote> and <quote>]</quote>) have a different meaning in
ordinary text; see <xref linkend="looping"> for further discussion.</para>

  <para>Which variable is an index and which is not is entirely up to the
application developer. In some applications, every array may be accessed as an
array, too. In other applications, normal variables and arrays are different
things. &varexp; does not dictate this. There exists the convention that
accessing an array with a negative index, such as <quote>${ARRAY[-1]}</quote>
should return the number of elements the array contains. But again, this is not
a behavior required by &varexp;; different applications may behave differently
here.</para>


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
mode: sgml
fill-column:80
sgml-parent-document:("user-manual.sgml" "article" "sect1" "sect2")
End:
-->
