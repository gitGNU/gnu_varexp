#include <sys/types.h>
#include <regex.h>
#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        static void expand_regex_replace(const char* data, tokenbuf_t* orig,
                                         regmatch_t* pmatch, tokenbuf_t* expanded)
            {
            const char* p = orig->begin;
            size_t i;

            expanded->clear();

            while (p != orig->end)
                {
                if (*p == '\\')
                    {
                    if (orig->end - p <= 1)
                        {
                        throw incomplete_quoted_pair();
                        }
                    p++;
                    if (*p == '\\')
                        {
                        expanded->append(p, 1);
                        ++p;
                        continue;
                        }
                    if (!isdigit((int)*p))
                        {
                        throw unknown_quoted_pair_in_replace();
                        }
                    i = *p - '0';
                    p++;
                    if (pmatch[i].rm_so == -1)
                        {
                        throw submatch_out_of_range();
                        }
                    expanded->append(data + pmatch[i].rm_so, pmatch[i].rm_eo - pmatch[i].rm_so);
                    }
                else
                    {
                    expanded->append(p, 1);
                    ++p;
                    }
                }
            }

        void search_and_replace(tokenbuf_t* data, tokenbuf_t* search,
                                tokenbuf_t* replace, tokenbuf_t* flags)

            {
            const char* p;
            int case_insensitive = 0;
            int global = 0;
            int no_regex = 0;
            int rc;

            if (search->begin == search->end)
                throw empty_search_string();

            for (p = flags->begin; p != flags->end; ++p)
                {
                switch (tolower(*p))
                    {
                    case 'i':
                        case_insensitive = 1;
                        break;
                    case 'g':
                        global = 1;
                        break;
                    case 't':
                        no_regex = 1;
                        break;
                    default:
                        throw unknown_replace_flag();
                    }
                }

            if (no_regex)
                {
                tokenbuf_t tmp;

                for (p = data->begin; p != data->end;)
                    {
                    if (case_insensitive)
                        rc = strncasecmp(p, search->begin,
                                         search->end - search->begin);
                    else
                        rc = strncmp(p, search->begin,
                                     search->end - search->begin);
                    if (rc != 0)
                        {
                        /* no match, copy character */
                        tmp.append(p, 1);
                        ++p;
                        }
                    else
                        {
                        tmp.append(replace->begin,
                                   replace->end - replace->begin);
                        p += search->end - search->begin;
                        if (!global)
                            {
                            tmp.append(p, data->end - p);
                            break;
                            }
                        }
                    }

                data->shallow_move(&tmp);
                }
            else
                {
                struct sentry
                    {
                    sentry() : preg(0) { }
                    ~sentry()          { if (preg) regfree(preg); }
                    regex_t* preg;
                    } s;
                tokenbuf_t tmp;
                tokenbuf_t mydata;
                tokenbuf_t myreplace;
                regex_t preg;
                regmatch_t pmatch[10];
                int regexec_flag;

                /* Copy the pattern and the data to our own buffer to make
                   sure they're terminated with a null byte. */

                tmp.append(search->begin, search->end - search->begin);
                mydata.append(data->begin, data->end - data->begin);

                /* Compile the pattern. */

                rc = regcomp(&preg, tmp.begin, REG_EXTENDED|((case_insensitive)?REG_ICASE:0));
                tmp.clear();
                if (rc != 0)
                    {
                    throw invalid_regex_in_replace();
                    }
                else
                    s.preg = &preg;

                /* Match the pattern and create the result string in the tmp
                   buffer. */

                for (p = mydata.begin; p != mydata.end; )
                    {
                    if (p == mydata.begin || p[-1] == '\n')
                        regexec_flag = 0;
                    else
                        regexec_flag = REG_NOTBOL;
                    if (regexec
                        (&preg, p, sizeof(pmatch) / sizeof(regmatch_t), pmatch,
                         regexec_flag) == REG_NOMATCH)
                        {
                        tmp.append(p, mydata.end - p);
                        break;
                        }
                    else
                        {
                        expand_regex_replace(p, replace, pmatch, &myreplace);
                        tmp.append(p, pmatch[0].rm_so);
                        tmp.append(myreplace.begin, myreplace.end - myreplace.begin);
                        p += (pmatch[0].rm_eo > 0) ? pmatch[0].rm_eo : 1;
                        myreplace.clear();
                        if (!global)
                            {
                            tmp.append(p, mydata.end - p);
                            break;
                            }
                        }
                    }

                data->shallow_move(&tmp);
                }
            }
        }
    }
