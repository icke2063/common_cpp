/**
 * @file   Logger.cpp
 * @Author icke2063
 * @date   02.06.2013
 * @brief  Logger implementation
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

#include "Logger.h"

namespace icke2063 {
namespace common_cpp {
/**
 * connect logger console with stdout
 */
log4cpp::Appender *Logger::console = new log4cpp::OstreamAppender("console", &std::cout);
Logger::Logger(){
	 console->setLayout(new log4cpp::BasicLayout());
}

Logger::~Logger() {}

} /* namespace common_cpp */
} /* namespace icke2063 */
