#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <regex.h>
#include "internal.h"

#include <stdio.h>

int search_and_replace(tokenbuf* data, tokenbuf* search, tokenbuf* replace, tokenbuf* flags)
    {
    const char* p;
    regex_t preg;
    tokenbuf tmp;
    int case_insensitive = 0;
    int global = 0;
    int no_regex = 0;
    int rc;

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

        }
    else
        {
        tmp.begin = tmp.end = NULL;
        tmp.buffer_size = 0;
        if (!append_to_tokenbuf(&tmp, search->begin, search->end - search->begin))
            return VAR_OUT_OF_MEMORY;

        rc = regcomp(&preg, tmp.begin, REG_EXTENDED | ((case_insensitive) ? REG_ICASE : 0));
        free_tokenbuf(&tmp);
        if (rc != 0)
            return VAR_INVALID_REGEX_IN_REPLACE;

        }

    return VAR_OK;
    }
