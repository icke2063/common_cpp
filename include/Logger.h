/**
 * @file   Logger.h
 * @Author icke2063
 * @date   02.06.2013
 *
 * Copyright © 2013 icke2063 <icke2063@gmail.com>
 *
 * This software is free; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"

namespace icke2063 {
namespace common_cpp {
class Logger {
public:
	Logger();
	virtual ~Logger();
protected:
	/*
	 * default appender for console logging
	 */
	static log4cpp::Appender *console;
	log4cpp::Category *logger;
};

} /* namespace common_cpp */
} /* namespace icke2063 */
#endif /* LOGGER_H_ */
