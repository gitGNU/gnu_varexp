#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <regex.h>
#include "internal.h"

#include <stdio.h>

int search_and_replace(tokenbuf* data, tokenbuf* search, tokenbuf* replace, tokenbuf* flags)
    {
    const char* p;
    int case_insensitive = 0;
    int global = 0;
    int no_regex = 0;
    int rc;

    if (search->begin == search->end)
        return VAR_EMPTY_SEARCH_STRING;

    printf("Search '%s' in '%s' and replace it with '%s'.\n",
           search->begin, data->begin, replace->begin);

    for (p = flags->begin; p != flags->end; ++p)
        {
        switch (tolower(*p))
            {
            case 'i':
                case_insensitive = 1;
                printf("case_insensitive = 1;\n");
                break;
            case 'g':
                global = 1;
                printf("global = 1;\n");
                break;
            case 't':
                no_regex = 1;
                printf("no_regex = 1;\n");
                break;
            default:
                return VAR_UNKNOWN_REPLACE_FLAG;
            }
        }

    if (no_regex)
        {
        tokenbuf tmp;
        init_tokenbuf(&tmp);

        for (p = data->begin; p != data->end; )
            {
            if (case_insensitive)
                rc = strncasecmp(p, search->begin, search->end - search->begin);
            else
                rc = strncmp(p, search->begin, search->end - search->begin);
            if (rc != 0)
                {               /* no match, copy character */
                if (!append_to_tokenbuf(&tmp, p, 1))
                    {
                    free_tokenbuf(&tmp);
                    return VAR_OUT_OF_MEMORY;
                    }
                ++p;
                }
            else
                {
                append_to_tokenbuf(&tmp, replace->begin, replace->end - replace->begin);
                p += search->end - search->begin;
                if (!global)
                    {
                    if (!append_to_tokenbuf(&tmp, p, data->end - p))
                        {
                        free_tokenbuf(&tmp);
                        return VAR_OUT_OF_MEMORY;
                        }
                    break;
                    }
                }
            }

        free_tokenbuf(data);
        move_tokenbuf(&tmp, data);
        }
    else
        {
        tokenbuf tmp;
        tokenbuf mydata;
        regex_t preg;
        regmatch_t pmatch[33];
        int regexec_flag;

        /* Copy the pattern and the data to our own buffer to make
           sure they're terminated with a null byte. */

        if (!assign_to_tokenbuf(&tmp, search->begin, search->end - search->begin))
            return VAR_OUT_OF_MEMORY;
        if (!assign_to_tokenbuf(&mydata, data->begin, data->end - data->begin))
            {
            free_tokenbuf(&tmp);
            return VAR_OUT_OF_MEMORY;
            }

        /* Compile the pattern. */

        printf("data is.................: '%s'\n", mydata.begin);
        printf("regex search pattern is.: '%s'\n", tmp.begin);
        printf("regex replace pattern is: '%s'\n", replace->begin);
        rc = regcomp(&preg, tmp.begin, REG_EXTENDED | ((case_insensitive) ? REG_ICASE : 0));
        free_tokenbuf(&tmp);
        if (rc != 0)
            {
            free_tokenbuf(&mydata);
            return VAR_INVALID_REGEX_IN_REPLACE;
            }
        printf("Subexpression in pattern: '%d'\n", preg.re_nsub);

        /* Match the pattern and create the result string in the tmp
           buffer. */

        for (p = mydata.begin; p != mydata.end; )
            {
            if (p == mydata.begin || p[-1] == '\n')
                regexec_flag = 0;
            else
                regexec_flag = REG_NOTBOL;
            if (regexec(&preg, p, sizeof(pmatch) / sizeof(regmatch_t), pmatch, regexec_flag) == REG_NOMATCH)
                {
                printf("No match; appending remainder ('%s') to output string.\n", p);
                append_to_tokenbuf(&tmp, p, mydata.end - p);
                break;
                }
            else
                {
                printf("Match from offset %d to %d in string '%s'.\n",
                       pmatch[0].rm_so, pmatch[0].rm_eo, p);
                if (!append_to_tokenbuf(&tmp, p, pmatch[0].rm_so) ||
                    !append_to_tokenbuf(&tmp, replace->begin, replace->end - replace->begin))
                    {
                    regfree(&preg);
                    free_tokenbuf(&tmp);
                    free_tokenbuf(&mydata);
                    return VAR_OUT_OF_MEMORY;
                    }
                else
                    p += pmatch[0].rm_eo;
                if (!global)
                    {
                    append_to_tokenbuf(&tmp, p, mydata.end - p);
                    break;
                    }
                }
            }

        regfree(&preg);
        free_tokenbuf(data);
        move_tokenbuf(&tmp, data);
        free_tokenbuf(&mydata);
        }

    return VAR_OK;
    }
