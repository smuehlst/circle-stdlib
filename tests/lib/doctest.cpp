#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_NO_POSIX_SIGNALS
#define DOCTEST_CONFIG_NO_MULTITHREADING
#include <doctest/doctest.h>

#include <circle/logger.h>
#include <string>

#include "doctest_logger.h"

REGISTER_LISTENER("doctest_logger", 0, DoctestLogger);