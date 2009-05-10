  <para>&varexp; distinguishes variables into simple and complex
expressions. A simple expression has the form <quote>$NAME</quote> and will
basically only replace the variable in the text buffer with its contents.
Complex expressions have the form
<quote>${NAME:operation1:operation2:&hellip;}</quote> and may perform various
operations on the variable's contents before inserting it into the text
buffer.</para>

  <para>Please note that due to the way simple expressions are parsed, it may
not always be possible to use the simple-expression form even though you do not
want to perform any operations. If your input text was <quote>This is a
$FOObar</quote>, but the last <quote>bar</quote> part is meant to be a literal
string, you'd have use <quote>This is a ${FOO}bar</quote>, because the parser
will interpret any valid variable-name character following the dollar as part of
the variable name; it will not recognize that <quote>$FOO</quote> would exist
while <quote>$FOObar</quote> would not.</para>

  <para>Also, &varexp; does not distinguish case in any way. For the library,
<quote>$FoObAr</quote> and <quote>$fOoBaR</quote> are just strings -- whether
they refer to the same variable or not is entirely up to the application that
provides the callback used to resolve variables to their contents.</para>

  <para>If you want to enter a text like <quote>$foo</quote> literally, you'll
have to escape the <quote>$</quote> sign by prefacing it with a backslash:
<quote>\$foo</quote>. Then &varexp; won't interpret this expression as a
variable.</para>

<!--
Local Variables:
mode: sgml
fill-column:80
sgml-parent-document:("user-manual.sgml" "article" "sect1" "sect2")
End:
-->
