/**
 * @file   I2CComm.cpp
 * @Author icke2063
 * @date   12.06.2013
 * @brief  I2C communication class
 *
 * Copyright © 2013-2018 icke2063 <icke2063@gmail.com>
 *
 * This software is free software; you can redistribute it and/or
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

#include <I2CComm.h>

#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <i2c_logging_macros.h>

namespace icke2063 {
namespace I2C {

I2C_Comm::I2C_Comm() :
		m_i2cFD(-1), m_path(""), m_open_i2c_bus(false)
{
	resetLivelist();

}

I2C_Comm::I2C_Comm(std::string& i2c_master_path) :
		m_i2cFD(-1), m_path(""), m_open_i2c_bus(false)
{
	resetLivelist();
	
	/**
	 *
	 * @todo add throw exception ?!?
	 */
	
	i2cOpen(i2c_master_path);
}

I2C_Comm::~I2C_Comm() {
	i2cClose();
}

bool I2C_Comm::i2cOpen(std::string& path)
{
	if (path.compare(m_path) != 0)
	{//different path -> close old connection
		i2cClose(); 	//close existing connection
		m_path = path;	//update path
	}
	
	if(!m_open_i2c_bus)
	{//no connection -> try open connection	
		if (((m_i2cFD = open(m_path.c_str(), O_RDWR))) < 0)
		{//failure
			i2c_ERROR_WRITE("Cannot open I2C path[%s]", m_path.c_str());
			m_path = "";
			return false; //open new connection not working
		}
		m_path = path;
		m_open_i2c_bus = true;
	}
	
	return m_open_i2c_bus;
}

bool I2C_Comm::i2cOpen()
{
  if(!m_open_i2c_bus)
  {
      return i2cOpen(m_path);
  }
  else
  {
    return m_open_i2c_bus;
  }
}

void I2C_Comm::i2cClose()
{
	if(m_i2cFD)close(m_i2cFD);
	m_i2cFD = -1;
	m_open_i2c_bus = false;
}

bool I2C_Comm::i2cSetAddress(unsigned char address)
{
	unsigned char retry = MAX_I2C_RETRY;
	
	//i2c_DEBUG_WRITE("i2cSetAddress[%i]", address);
	
	if (m_i2cFD > 0)
	{
		std::lock_guard<std::mutex> lock(i2cbus_lock);
		while (retry-- != 0 && ioctl(m_i2cFD, I2C_SLAVE, address) < 0)
		{
		  usleep(1000);
		}
		
		if (retry)
		{
			return true;
		}
	}
	else
	{
	  i2c_ERROR_WRITE("Cannot i2cSetAddress[%u]: bus not open", address);
	}
	return false;
}

bool I2C_Comm::Write_I2C_Bytes(unsigned char address, uint8_t *databuffer,
		size_t address_size, size_t write_size) 
{
	size_t n = address_size + write_size;
	unsigned char retry = MAX_I2C_RETRY;

	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[1];

	if (m_i2cFD > 0)
	{
		  
		messages[0].addr = address;
		messages[0].flags = 0;
		messages[0].len = n;
		messages[0].buf = databuffer;

		/* Send the request to the kernel and get the result back */
		packets.msgs = messages;
		packets.nmsgs = 1;

		{
			std::lock_guard<std::mutex> lock(i2cbus_lock);
		
			while (retry-- != 0 && ioctl(m_i2cFD, I2C_RDWR, &packets) < 0)
			{
			  usleep(I2C_RETRY_TIME_US);
			}
			
			if (retry)
			{
				return true;
			}
			else
			{
			  i2c_ERROR_WRITE("Cannot Write_I2C_Bytes: ioctl failure (also after retry[%u])", MAX_I2C_RETRY);
			}
		}
	}
	else
	{
	  i2c_ERROR_WRITE("Cannot Write_I2C_Bytes[%u]: bus not open", address);
	}
	return false;

}

bool I2C_Comm::Read_I2C_Bytes(unsigned char address, uint8_t *databuffer,
		size_t address_size, size_t read_size) {
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];
	unsigned char retry = MAX_I2C_RETRY;

	
	if (m_i2cFD > 0)
	{
	
		/*
		* In order to read a register, we first do a "dummy write" by writing
		* 0 bytes to the register we want to read from.  This is similar to
		* the packet in set_i2c_register, except it's 1 byte rather than 2.
		*/

		messages[0].addr = address;
		messages[0].flags = 0;
		messages[0].len = address_size;
		messages[0].buf = databuffer;

		/* The data will get returned in this structure */
		messages[1].addr = address;
		messages[1].flags = I2C_M_RD/* | I2C_M_NOSTART*/;
		messages[1].len = read_size;
		messages[1].buf = databuffer;

		/* Send the request to the kernel and get the result back */
		packets.msgs = messages;
		packets.nmsgs = 2;
		{
			std::lock_guard<std::mutex> lock(i2cbus_lock);



			while (retry-- != 0 && ioctl(m_i2cFD, I2C_RDWR, &packets) < 0)
			{
			  usleep(I2C_RETRY_TIME_US);
			}
			
			if (retry)
			{
				return true;
			}
			else
			{
			  i2c_ERROR_WRITE("Cannot Read_I2C_Bytes: ioctl failure (also after retry[%u])", MAX_I2C_RETRY);
			}
		}
	}
	else
	{
	  i2c_ERROR_WRITE("Cannot Read_I2C_Bytes[%u]: bus not open", address);
	}
	return false;
}

bool *I2C_Comm::ScanBus(void)
{
	int slave_adr = 0;
	uint8_t c;
	
	i2c_DEBUG_WRITE("scan I2C bus");
	
	resetLivelist();
	
	if (m_i2cFD > 0)
	{
		// loop over whole range
		for (slave_adr = MIN_I2C_SLAVE_ADR; slave_adr <= MAX_I2C_SLAVE_ADR;
				slave_adr++) 
		{
			if (!i2cSetAddress(slave_adr))
				return NULL;

			if (read(m_i2cFD, &c, 1) != 1) {
				livelist[slave_adr] = false;
			} else {
				livelist[slave_adr] = true;
			}
		}
		return livelist;
	}
	else
	{
	  i2c_ERROR_WRITE("ScanBus: bus not open");
	}
	return NULL;
}

void I2C_Comm::resetLivelist(void)
{
	int i;
	for (i = MIN_I2C_SLAVE_ADR; i <= MAX_I2C_SLAVE_ADR; i++)
	{
		livelist[i] = false;
	}
}

} /* namespace I2C */
}/* namespace icke2063 */

