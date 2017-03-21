#include "stdafx.h"

#ifndef NDEBUG

#include "AssertHelper.h"

void _Assert_Print(char const* logger,
                    char const* condition_string,
                    char const* file_name,
                    unsigned int line_number,
                    std::stringstream const& message)
{
  CLOG(FATAL, logger) << "\nAssertion Failed: " << condition_string
    << " -- " << message.str()
    << " @ " << file_name
    << ":" << line_number;
}

#endif