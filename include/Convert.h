/**
 * @file   Convert.h
 * @Author icke2063
 * @date   16.06.2013
 * @brief  Simple class to convert
 * 			- short <-> BigEndian
 *
 * Copyright © 2013 icke2063 <icke2063@gmail.com>
 *
 * This framework is free software; you can redistribute it and/or
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

#ifndef CONVERT_H_
#define CONVERT_H_

#include <endian.h>

namespace icke2063 {
namespace common_cpp {

class Convert {
public:
	Convert(){}
	virtual ~Convert(){}

	/**
	 * convert short value to BigEndian
	 * @param dst	destination pointer
	 * @param src	source pointer
	 * @param count	count of values (short) to convert
	 */
	virtual void ShorttoBigEndian(uint16_t *dst, const uint16_t *src, size_t count){
		size_t i;
		for(i=0;i<count;i++){
			dst[i] = htobe16(src[i]);
		}
	}

	/**
	 * convert short value from BigEndian to maschine specific endianess
	 * @param dst	destination pointer
	 * @param src	source pointer
	 * @param count	count of values(short) to convert
	 */
	virtual void BigEndiantoShort(uint16_t *dst, const uint16_t *src, size_t count){
		size_t i;
		for(i=0;i<count;i++){
			dst[i] = be16toh(src[i]);
		}
	}

};

} /* namespace common_cpp */
} /* namespace icke2063 */
#endif /* MBCONVERT_H_ */
