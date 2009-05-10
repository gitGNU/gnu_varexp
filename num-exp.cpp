#include "internal.hpp"

namespace varexp
{
  namespace internal
  {
    size_t parser::num_exp_read_operand(const char* begin, const char* end, int& result)
    {
      const char* p = begin;
      size_t rc;

      if (begin == end)
        throw incomplete_index_spec();

      if (*p == '(')
      {
        rc = num_exp(++p, end, result);
        p += rc;
        if (p == end)
          throw incomplete_index_spec();
        if (*p != ')')
          throw unclosed_bracket_in_index();
        ++p;
      }
      else if (*p == config.varinit)
      {
        string tmp;
        rc = variable(p, end, tmp);
        p += rc;
        rc = num_exp(tmp.c_str(), tmp.c_str() + tmp.size(), result);
      }
      else if (config.current_index && *p == config.current_index)
      {
        p++;
        result = current_index;
        ++rel_lookup_count;
      }
      else if (isdigit(*p))
      {
        unsigned int tmp;
        rc = number(p, end, tmp);
        result = tmp;
        p += rc;
      }
      else if (*p == '+')
      {
        if (end - p > 1 && isdigit(p[1]))
        {
          p++;
          unsigned int tmp;
          rc = number(p, end, tmp);
          result = tmp;
          p += rc;
        }
        else
          throw invalid_char_in_index_spec();
      }
      else if (*p == '-')
      {
        if (end - p > 1 && isdigit(p[1]))
        {
          p++;
          unsigned int tmp;
          rc = number(p, end, tmp);
          result = tmp;
          p += rc;
          result = 0 - result;
        }
        else
          throw invalid_char_in_index_spec();
      }
      else
        throw invalid_char_in_index_spec();

      return p - begin;
    }

    size_t parser::num_exp(const char *begin, const char *end, int& result)
    {
      const char* p = begin;
      char oper;
      int right;
      int rc;

      if (begin == end)
        throw incomplete_index_spec();

      rc = num_exp_read_operand(p, end, result);
      p += rc;

      while (p != end)
      {
        if (*p == '+' || *p == '-')
        {
          oper = *p++;
          rc = num_exp(p, end, right);
          p += rc;
          if (oper == '+')
            result = result + right;
          else
            result = result - right;
        }
        else if (*p == '*' || *p == '/' || *p == '%')
        {
          oper = *p++;
          rc = num_exp_read_operand(p, end, right);
          p += rc;
          if (oper == '*')
          {
            result = result * right;
          }
          else if (oper == '/')
          {
            if (right == 0)
            {
              throw division_by_zero_in_index();
            }
            else
              result = result / right;
          }
          else if (oper == '%')
          {
            if (right == 0)
            {
              throw division_by_zero_in_index();
            }
            else
              result = result % right;
          }
        }
        else
          break;
      }
      return p - begin;
    }
  }
}
