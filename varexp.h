#ifndef LIB_VARIABLE_EXPAND_H
#define LIB_VARIABLE_EXPAND_H

/*
  The callback will be called by variable_expand(), providing the
  following parameterns:
        context         - passed through from variable_expand()'s
                          parameters
        varname         - pointer to the name of the variable to
                          expand
        name_len        - length of the string starting at varname
        data            - location, where the callback should store
                          the pointer to the contents of the looked-up
                          variable
        data_len        - location, where the callback should store
                          the length of the data
        malloced_buffer - location, where the callback should store
                          either TRUE or FALSE, telling the framework
                          whether the buffer must be free(3)ed.

  The return code is interpreted as follows:
       >0               - OK
        0               - undefined variable
       <0               - error
*/

int (*var_cb_t)(void* context,
		const char* varname, size_t name_len,
		char* const* data, size_t* data_len, char* malloced_buffer);

/*
  This structure configures the parser's specials. I think, the fields
  are pretty self-explanatory. The only one worth mentioning is
  force_expand, which is a boolean. If set to TRUE, variable_expand()
  will fail with an error if the lookup callback returns "undefined
  variable". If set to FALSE, variable_expand() will copy the
  expression that failed verbatimly to the output so that another pass
  may expand it.

  The comments after each field show the default configuration.
*/

typdef struct
    {
    char  varinit;               /* '$' */
    char  startdelim;            /* '{' */
    char  enddelim;              /* '}' */
    char  escape;                /* '\' */
    char* namechars;             /* 'a-zA-Z0-9_' */
    char  force_expand;          /* TRUE */
    }
var_config_t;
extern const var_config_t var_config_default;

/*
  variable_expand() will parse the contents of input for variable
  expressions and expand them using the provided lookup callback. The
  pointer to the resulting buffer is stored in result, its length in
  result_len. The buffer is always terminated by a '\0' byte, which is
  not included in the result_len count. The buffer must be free(3)ed
  by the caller.
*/

int var_expand(const char* input, size_t input_len,
	       char** result, size_t* result_len,
	       const char** error_msg,
	       varexp_lookup_cb lookup, void* lookup_context,
	       struct varexp_configuration* config);

#endif /* !defined(LIB_VARIABLE_EXPAND_H) */
