/**
 * @file   logging.h
 * @Author icke2063
 * @date   26.03.2015
 *
 * simple libabcc clone
 *
 * Copyright © 2013 icke2063 <icke2063@gmail.com>
 *
 * This software is free; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef ICKE2063_LOGGING_H_
#define ICKE2063_LOGGING_H_

#include <stdarg.h>
#include <stdint.h>

struct logging{
	uint8_t loglevel;
	void(*log_fn)(uint8_t loglevel, const char *file, const int line, const char *fn, const char *format, va_list args);
};

static inline void write_to_log(struct logging *handle, uint8_t log_priority,
		const char *file, const int line, const char *fn, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	if(handle->loglevel >= log_priority)      handle->log_fn(log_priority, file, line, fn, format, args);
	va_end(args);
}

static inline void default_logfn(uint8_t loglevel, const char *file, const int line, const char *fn, const char *format, va_list args)
{

}

#endif /* ICKE2063_LOGGING_H_ */
