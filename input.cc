#include "internal.hh"

namespace varexp
    {
    namespace internal
        {
        struct lookup_wrapper : public callback_t
        {
        lookup_wrapper(callback_t& _lookup, int& counter)
                : lookup(_lookup), rel_lookup_count(counter)
            {
            }
        virtual void operator()(const string& name, string& data)
            {
            lookup(name, data);
            }
        virtual void operator()(const string& name, int idx, string& data)
            {
            try
                {
                lookup(name, idx, data);
                }
            catch(const undefined_variable&)
                {
                --rel_lookup_count;
                data = "";
                }
            }
        callback_t& lookup;
        int&        rel_lookup_count;
        };


        size_t parser::input(const char* begin, const char* end, string& result)
            {
            const char* p = begin;
            try
                {
                size_t rc = 0;  // Initialize it to make the compiler happy.

                while (p != end)
                    {
                    if (config.startindex && *p == config.startindex)
                        {
                        p++;

                        int start = 0;
                        int step = 1;
                        int stop = 0;
                        bool open_end = true;
                        bool have_loop_limit = false;
                        size_t loop_limit_len = 0;  // Initialize it to make the compiler happy.
                        size_t last_result_len = 0;  // Initialize it to make the compiler happy.
                        int orig_rel_lookup_count = rel_lookup_count;
                        int orig_current_index = current_index;
                        lookup_wrapper wrapper(*lookup, rel_lookup_count);
                        lookup = &wrapper;

                      restart_loop:
                        for (int i = start; ; i += step)
                            {
                            // Check whether we need to terminate.

                            if (open_end)
                                {
                                if (have_loop_limit)
                                    {
                                    if (rel_lookup_count <= orig_rel_lookup_count)
                                        break;
                                    }
                                }
                            else
                                {
                                if (i > stop)
                                    break;
                                }

                            // Set the relative lookup count to the
                            // original value.

                            rel_lookup_count = orig_rel_lookup_count;

                            // Save length of current result string so
                            // that we can take additions back if
                            // necessary.

                            last_result_len = result.size();

                            // Set current index.

                            current_index = i;

                            // Recurse to parse the contents of the
                            // loop and to append the expanded version
                            // to the result.

                            rc = input(p, end, result);

                            // Check whether the whole loop was parsed
                            // successfully.

                            if (p[rc] != config.endindex)
                                throw unterminated_loop_construct();

                            // If didn't do it yet (first iteration),
                            // parse the loop-limits construct
                            // possibly following here.

                            if (!have_loop_limit)
                                {
                                have_loop_limit = true;
                                int tmp = rel_lookup_count;
                                size_t rc2 = loop_limits(p + rc + 1, end, start, step, stop, open_end);
                                rel_lookup_count = tmp;
                                if (rc2 == 0)
                                    loop_limit_len = 0;
                                else
                                    {
                                    // Throw current result away and
                                    // restart the loop.

                                    loop_limit_len = rc2;
                                    result.erase(last_result_len);
                                    goto restart_loop;
                                    }
                                }
                            }

                        // If we terminated because no variable
                        // expanded anymore, we need to throw the last
                        // result away.

                        if (open_end)
                            result.erase(last_result_len);

                        p += rc;
                        p++;
                        p += loop_limit_len;
                        current_index = 0;
                        lookup = &wrapper.lookup;
                        rel_lookup_count = orig_rel_lookup_count;
                        current_index = orig_current_index;
                        continue;
                        }

                    rc = text(p, end);
                    if (rc > 0)
                        {
                        result.append(p, rc);
                        p += rc;
                        continue;
                        }

                    string tmp;
                    rc = variable(p, end, tmp);
                    if (rc > 0)
                        {
                        result += tmp;
                        p += rc;
                        continue;
                        }

                    if (rc == 0)
                        break;
                    }

                return p - begin;
                }
            catch(error& e)
                {
                e.current_position = p - begin;
                throw;
                }
            }
        }
    }
