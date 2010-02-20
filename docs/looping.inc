  <para>Obviously, arithmetic in array indices would be quite pointless without
a looping construct. &varexp; offers such a costruct, which can model both a
<quote>for</quote> and a <quote>while</quote> loop. Let's start with the second
version, which is slightly simpler.</para>

  <para>If the index delimiters <quote>[</quote> and <quote>[</quote> are found
in the text, the start a looping construct. An example would be <quote>This is a
test: [ $FOO ]</quote>. What happens now is that all text between the
loop-delimiters is repeated again and again until all variables found in the
body of the loop say they're undefined for the current index. The current index
starts counting at zero (0) and is increased with every interation of the loop.
In the index-specifier of the variable, it is available as
<quote>#</quote>.</para>

  <para>Hence, if we assume that the variable <quote>ARRAY[]</quote> had three
entries: <quote>entry1</quote>, <quote>entry2</quote>, and
<quote>entry3</quote>, then the loop <quote>[${ARRAY[i]}]</quote> would expand
to <quote>entry1entry2entry3</quote>. Once the conter reached index 4,
<quote>all</quote> arrays in the loop's body are undefined.</para>

  <para>That raises the question what the first example we presented,
<quote>This is a test: [ $FOO ]</quote>, would expand to? The answer is: To the
empty string! The loop would start expanding the body with index 0 and right at
the very first iteration, all arrays in the body were empty -- that is, no array
would have been expanded, because there weren't any arrays.</para>

  <para>Thus, this form of looping only makes sense if you
<emphasis>do</emphasis> specify arrays in the loop's body. If you do, though,
you can do some weird things, like <quote>[${ARRAY[#%2]}]</quote>, which expands
to <quote>${ARRAY[0]}</quote> for even numbers and to <quote>${ARRAY[1]}</quote>
for odd numbers. But the expression has another property: It will never
terminate, because the array-loopup will never fail, assuming that indices 0 and
1 are defined!</para>

  <para>That is unfortunate but can't be helped, I'm afraid. Users of &varexp;
may choose to disable looping for the users of their application to prevent the
end-user from shooting himself in the foot with infinite loops, though. But if
you want to use loops, you must know what you're doing. There ain't no such
thing as a free lunch, right?</para>

  <para>There is another form of the looping construct available, that resembles
a <quote>for</quote> loop more closely. In this form, the start value, the step
value and the stop value of the loop can be specified explicitely like this:
<quote>[$FOO]{<replaceable>start</replaceable>,<replaceable>step</replaceable>,<replaceable>stop</replaceable>}</quote>.
This loop will start to expand the body using index
<replaceable>start</replaceable>, it will increase the current index in each
iteration by <replaceable>step</replaceable>, and it will terminate when the
current index is greater than <replaceable>stop</replaceable>. (Please note that
<quote>greater than</quote> is concept that needs much thought if you use
negative values here! There may be some infinite loops coming. You have been
warned.)</para>

  <para>If any of the first two values are omitted, the following defaults will
be assumed: <replaceable>start</replaceable> = 0 and
<replaceable>step</replaceable> = 1. If <replaceable>stop</replaceable> is
omitted, the loop will terminate if none of the arrays in the loop's body is
defined for the current index. Consequently, using the loop-limits
<quote>{,,}</quote> is equivalent to not specifying any limits at all.</para>

  <para>Since most users will not need the <replaceable>step</replaceable>
parameter frequently, a shorter form
<quote>{<replaceable>start</replaceable>,<replaceable>stop</replaceable>}</quote>
is allowed, too.</para>

  <para>By the way: Loops may be nested. :-)</para>

  <para>To confuse the valued reader completely, let's look at this final
example. Assume that the arrays <quote>${FOO[]}</quote> and
<quote>${BAR[]}</quote> have the following values:</para>

  <informalexample>
    <literallayout>    FOO[0] = "foo0"
    FOO[1] = "foo1"
    FOO[2] = "foo2"
    FOO[3] = "foo3"

and

    BAR[0] = "bar0"
    BAR[1] = "bar1"</literallayout>
  </informalexample>

  <para>Then the expression:</para>

  <informalexample>
    <literallayout>    [${BAR[#]}: [${FOO[#]}${FOO[#+1]:+, }]${BAR[#+1]:+; }]</literallayout>
  </informalexample>

  <para>would expand to:</para>

  <informalexample>
    <literallayout>    bar0: foo0, foo1, foo2, foo3; bar1: foo0, foo1, foo2, foo3</literallayout>
  </informalexample>

  <para>Have fun!</para>

<!--
Local Variables:
mode: xml
fill-column:80
End:
-->
