// Copyright (c) Acconeer AB, 2016-2019
// All rights reserved

#ifndef ACC_LOG_H_
#define ACC_LOG_H_

#include "acc_definitions.h"

#ifdef ACC_LOG_RSS_H_
#error "acc_log.h and acc_log_rss.h cannot coexist"
#endif

#define ACC_LOG(level, ...)	acc_log(level, MODULE, __VA_ARGS__)

#define ACC_LOG_ERROR(...)	ACC_LOG(ACC_LOG_LEVEL_ERROR, __VA_ARGS__)
#define ACC_LOG_WARNING(...)	ACC_LOG(ACC_LOG_LEVEL_WARNING, __VA_ARGS__)
#define ACC_LOG_INFO(...)	ACC_LOG(ACC_LOG_LEVEL_INFO, __VA_ARGS__)
#define ACC_LOG_VERBOSE(...)	ACC_LOG(ACC_LOG_LEVEL_VERBOSE, __VA_ARGS__)
#define ACC_LOG_DEBUG(...)	ACC_LOG(ACC_LOG_LEVEL_DEBUG, __VA_ARGS__)

#define ACC_LOG_SIGN(a)      ((a < 0.0f) ? (-1) : (1))
#define ACC_LOG_FLOAT_INT(a) ((unsigned long int)(a + 0.0000005f))
#define ACC_LOG_FLOAT_DEC(a) (unsigned long int)((1000000.0f * ( (a + 0.0000005f) - ( (unsigned int)(a + 0.0000005f) ))))

#define ACC_LOG_FLOAT_TO_INTEGER(a) ((a < 0.0f) ? '-' : ' '), ACC_LOG_FLOAT_INT(a * ACC_LOG_SIGN(a)), ACC_LOG_FLOAT_DEC(a * ACC_LOG_SIGN(a))

/**
 * @brief Specifier for printing float type using integers.
 */
#define PRIfloat	"c%lu.%06lu"

#define ACC_UNUSED(VAR)	(void)(VAR)

#if defined(__GNUC__)
#define PRINTF_ATTRIBUTE_CHECK(a, b) __attribute__ ((format (printf, a, b)))
#else
#define PRINTF_ATTRIBUTE_CHECK(a, b)
#endif

extern void acc_log(acc_log_level_t level, const char *module, const char *format, ...) PRINTF_ATTRIBUTE_CHECK(3, 4);

#endif
