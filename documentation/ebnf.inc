input      : ( TEXT
              | variable
              | START-INDEX input END-INDEX ( loop-limits )?
              )*

loop-limits: START-DELIM
                (numexp)? ',' (numexp)? ( ',' (numexp)? )?
             END-DELIM


variable   : '$' (name|expression)

expression : START-DELIM (name|variable)+
             (START-INDEX num-exp END-INDEX)?
             (':' command)* END-DELIM

name       : (VARNAME)+

command    : '-' (EXPTEXT|variable)+
           | '+' (EXPTEXT|variable)+
           | 'o' (NUMBER ('-'|',') (NUMBER)?)
           | '#'
           | '*' (EXPTEXT|variable)+
           | 's' '/' (variable|SUBSTTEXT)+ '/' (variable|SUBSTTEXT)* '/'
             ('g'|'i'|'t')*
           | 'y' '/' (variable|SUBSTTEXT)+ '/' (variable|SUBSTTEXT)* '/'
           | 'p' '/' NUMBER '/' (variable|SUBSTTEXT)* '/' ('r'|'l'|'c')
           | 'l'
           | 'u'

num-exp    : operand
           | operand ('+'|'-'|'*'|'/'|'%') num-exp

operand    : ('+'|'-')? NUMBER
           | CURR-INDEX
           | '(' num-exp ')'
           | variable

START-DELIM: '{'

END-DELIM  : '}'

START-INDEX: '['

END-INDEX  : ']'

CURR-INDEX : '#'

VARNAME    : '[a-zA-Z0-9_]+'

NUMBER     : '[0-9]+'

SUBSTTEXT  : '[^$/]'

EXPTEXT    : '[^$}:]+'

TEXT       : '[^$[\\]]+'
