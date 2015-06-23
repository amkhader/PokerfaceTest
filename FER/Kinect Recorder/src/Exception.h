#pragma once

#include <string>
#include "stdarg.h"
#include "stdio.h"

/// \brief A simple string-based class used for exception handling.

/// Has a printf-like constructor for easy error string formating
/// and a corversion operator to const char *.\n
/// Example: throw Exception ("Error code %d", code);\n
/// Example: catch (const Exception &e) { printf (e); }\n


class Exception
{
  public:
    Exception (const char *format, ...)
              {
                static char buffer [1024];
                va_list argptr;
                va_start (argptr, format);

                vsprintf_s (buffer, format, argptr);
                va_end (argptr);
                string = buffer;
              }
    operator const char *(void) const { return string.c_str(); }
  protected:
    std :: string string;
};

