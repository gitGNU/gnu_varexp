  <para>&varexp; is a C++ library that allows its users to detach any kind of
information from the representation of that information by providing a
simple-to-use but powerful text-template mechanism. Similar mechanisms have been
available in tools like
<citerefentry><refentrytitle>sh</refentrytitle><manvolnum>1</manvolnum></citerefentry>,
<citerefentry><refentrytitle>make</refentrytitle><manvolnum>1</manvolnum></citerefentry>,
or
<citerefentry><refentrytitle>perl</refentrytitle><manvolnum>1</manvolnum></citerefentry>
forever and have proven to be very useful. The basic idea is that the relevant
information is made available in variables, which the author of the template can
than use within the text itself as he or she sees fit.</para>

  <para>Consider, for example, a tool that will calculate the monthly financial
reports of a small company. Such a program should only
<emphasis>calculate</emphasis> the required values, it should not worry about
writing the resulting reports into an HTML file, a CSV file, or whatever format
is desired. Instead, it should make the results of the calculation available in
the variables <quote>$TURNOVER</quote>, <quote>$PROFIT</quote>, and
<quote>$INCREASE</quote>. Then, using &varexp;, it could load an arbitrary
template file and have the actual values inserted at the apropriate positions.
Without changing a single line of code, one could generate the monthly report in
HTML:</para>

  <informalexample>
    <screen>&lt;!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"&gt;
&lt;html&gt;
  &lt;head&gt;
    &lt;title&gt;Financial Report&lt;/title&gt;
  &lt;/head&gt;
  &lt;body&gt;

    &lt;h1&gt;Financial report&lt;/h1&gt;

    &lt;p&gt;This month, our glorious company reached a
total turnover of ${TOTAL} Euros, totalling ${PROFIT}
Euros before taxes. That means we have increased our
profit by ${INCREASE} percent compared to last
month.&lt;/p&gt;

  &lt;/body&gt;
&lt;/html&gt;</screen>
  </informalexample>

  <para>Or you can send it out as a plain-text e-mail:</para>

  <informalexample>
    <screen>From: nobody@example.org (Monthly Financial Data)
Subject: This month's financial report

Dear Colleagues,

we have earned a total of ${PROFIT} Euros this month!
This means that we have increased profits by ${PROFIT}
percent compared to last month, totalling a turnover
of ${TURNOVER} of Euros.

Sincerely yours,
    The Statistics Program</screen>
  </informalexample>

  <para>Even better, by using such templates to generate the output you are
effectively independent of the language you choose! The last report, for
example, could also read:</para>

  <informalexample>
    <screen>From: nobody@example.org (Grssss&ouml;tmpf!)
Subject: Ajahaha Mzoodeutschmark

Nuwarskvu,

Quhu&szlig;aour Maou Ahosetuh Cravullitstziki Nakaou ${PROFIT}
Akqiss&auml;e&uuml;&szlig; Blaga: ${TURNOVER} Stauhr ${INCREASE}!!!!</screen>
  </informalexample>

  <para>This version is -- as you can probably easily recognize -- in
German.</para>

  <para>&varexp; offers application developers two functions, that will do all
this for them, plus, the end-user has <emphasis>numerous</emphasis> ways not
only to insert variables into his template files but to modify the variables
contents on the fly, do full-blown regular expression search-and-replaces, or
loop over the contents of arrays of variables.</para>

  <para>Furthermore, the parser included in &varexp; can be re-configured to use
tokens different from the ones shown in the examples; one could as well use
<quote>%{NAME}</quote>, change the set of allowed characters for variable names,
etc.</para>

  <para>And last but certainly not least, these variables are not limited to
<quote>environment variables</quote> at all. The programmer is free to provide a
callback function to &varexp; that will be used to map a variable name to its
contents. Thus, the variables your application provides, can reside internally
completely. In fact, they can reside pretty much anywhere you want and they
contain pretty much anything you want, as long as <emphasis>you</emphasis> write
the callback.</para>

<!--
Local Variables:
mode: sgml
fill-column:80
sgml-parent-document:("user-manual.sgml" "article" "sect1")
End:
-->
