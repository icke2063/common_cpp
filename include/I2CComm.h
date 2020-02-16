/**
 * @file   I2CComm.h
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
#ifndef I2CCOMM_H_
#define I2CCOMM_H_

//std lib
 #include <mutex>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdint.h>
#include <string>

#ifndef MAX_I2C_RETRY
#define MAX_I2C_RETRY			0x10
#endif

#ifndef I2C_RETRY_TIME_US
#define I2C_RETRY_TIME_US		1000
#endif

#define MIN_I2C_SLAVE_ADR		0x00
#define MAX_I2C_SLAVE_ADR		0x77

namespace icke2063 {
namespace I2C {

class I2C_Comm {
public:
	/**
	 * Default constructor
	 * - reset variables/livelist
	 */
	I2C_Comm();
	
	/**
	 * Constructor for automatic startup
	 * - reset variabes/livelist
	 * - open connection with given path
	 */
	I2C_Comm(std::string& i2c_master_path);
	virtual ~I2C_Comm();

	// Initialize functions

	/**
	 * 
	 * Open communication with internal stored path
	 * or return true if already open
	 */
	bool i2cOpen();
	
	/**
	 * open i2c bus with given path
	 * - store given path
	 * - reopen i2cbus if different path as already stored
	 */
	bool i2cOpen(std::string& path);
	
	/**
	 * Close open connection
	 * - disable open bus flag
	 */
	void i2cClose(); // close i2cbus
	bool i2cSetAddress(unsigned char address); // changes slave address

	// Communication functions
	bool Write_I2C_Bytes(unsigned char address, uint8_t *databuffer,
			size_t address_size, size_t write_size);
	bool Read_I2C_Bytes(unsigned char address, uint8_t *databuffer,
			size_t address_size, size_t read_size);

	// special functions
	virtual bool *ScanBus(void);

private:
	int m_i2cFD; //file descriptor for I2C connection
	std::string m_path;
	bool m_open_i2c_bus;

	void resetLivelist(void);
	bool livelist[MAX_I2C_SLAVE_ADR+1]; //list of found I2C slaves

	std::mutex i2cbus_lock; //lock for functor list
};

} /* namespace I2C */
}/* namespace icke2063 */
#endif /* I2CCOMM_H_ */
