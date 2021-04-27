/**
 * Built with ARM Mbed-OS
 *
 * Copyright (c) 2019-2021 George Beckstein
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _AT25DF041B_H_
#define _AT25DF041B_H_

#include "platform/platform.h"

#if defined(DEVICE_SPI) || defined(DOXYGEN_ONLY)

#include "BlockDevice.h"
#include "drivers/SPI.h"
#include "drivers/DigitalOut.h"

#define AT25DF041B_PAGE_COUNT           (2048)
#define AT25DF041B_PAGE_BYTE_SIZE   		(256)
#define AT25DF041B_TOTAL_BYTE_SIZE		(AT25DF041B_PAGE_COUNT * AT25DF041B_PAGE_BYTE_SIZE)

#define AT25DF041B_ERASE_SECTOR_SIZE    4096

/** AT25DF041B Opcodes
 *  See the datasheet for more info
 */

/** Read Commands */
#define AT25DF041B_READ_ARRAY_FAST      0x0B
#define AT25DF041B_READ_ARRAY           0x03
#define AT25DF041B_DUAL_OUTPUT_READ     0x3B

/** Program and Erase Commands */
#define AT25DF041B_PAGE_ERASE_256B      0x81
#define AT25DF041B_BLOCK_ERASE_4KB      0x20
#define AT25DF041B_BLOCK_ERASE_32KB     0x52
#define AT25DF041B_BLOCK_ERASE_64KB     0xD8
#define AT25DF041B_CHIP_ERASE_1         0x60
#define AT25DF041B_CHIP_ERASE_2         0xC7
#define AT25DF041B_BYTE_PAGE_PROGRAM    0x02
#define AT25DF041B_SEQ_PRGM_MODE_1      0xAD
#define AT25DF041B_SEQ_PRGM_MODE_2      0xAF

/** Protection Commands */
#define AT25DF041B_WRITE_ENABLE         0x06
#define AT25DF041B_WRITE_DISABLE        0x04
#define AT25DF041B_PROTECT_SECTOR       0x36
#define AT25DF041B_UNPROTECT_SECTOR     0x39
#define AT25DF041B_READ_PROTECT_REG     0x3C

/** Security Commands */
#define AT25DF041B_PROGRAM_OTP_SEC_REG  0x9B
#define AT25DF041B_READ_OTP_SEC_REG     0x77

/** Status Register Commands */
#define AT25DF041B_READ_STATUS_REG      0x05
#define AT25DF041B_ACTIVE_STATUS_INT_EN 0x25 // TODO - This is a cool feature... could save some uA
#define AT25DF041B_WRITE_STATUS_REG     0x01
#define AT25DF041B_WRITE_STATUS_REG_2   0x31

/** Miscellaneous Commands */
#define AT25DF041B_SOFT_RESET           0xF0
#define AT25DF041B_READ_MFG_AND_DEV_ID  0x9F
#define AT25DF041B_DEEP_POWER_DOWN      0xB9
#define AT25DF041B_EXIT_DEEP_POWER_DOWN 0xAB
#define AT25DF041B_ULTRA_POWER_DOWN     0x79 // Exit by toggling CS for >20ns (send dummy byte)

/** Data and information */
#define AT25DF041B_DUMMY_BYTE           0xDB
#define AT25DF041B_ERASE_VALUE          0xFF
#define AT25DF041B_MANUFACTURER_ID      0x1F
#define AT25DF041B_DEVICE_ID_BYTE_1     0x44
#define AT25DF041B_DEVICE_ID_BYTE_2     0x02
#define AT25DF041B_EXT_DEVICE_INF_LEN   0x00

/** Magic word for whole chip erase */
#define AT25DF041B_CHIP_ERASE_MAGIC_WORD    0xADE570 // "Adesto"

/** Status Register Bits */
#define AT25DF041B_STATUS_READY_BUSY_BIT    0x01
#define AT25DF041B_STATUS_WEL_BIT           0x02

/** Operation types */
#define AT25DF041B_OPERATION_TYPE_READ      0x00
#define AT25DF041B_OPERATION_TYPE_PROGRAM   0x01
#define AT25DF041B_OPERATION_TYPE_ERASE     0x02

/** Block device-based driver for the AT25DF041B SPI flash chip
 *
 *  @code
 * @endcode
 */
class AT25DF041B: public BlockDevice {

public:

    /** This constructor creates an unshared private member SPI bus object
     * @param[in] mosi MOSI SPI bus pin
     * @param[in] miso MISO SPI bus pin
     * @param[in] sclk SCLK SPI bus pin
     * @param[in] ssel Slave select pin for this AT25DF041B
     */
    AT25DF041B(PinName mosi, PinName miso, PinName sclk, PinName ssel);

    /** Lifetime of a block device
     */
    virtual ~AT25DF041B() {
    }

    /** Initialize an AT25DF041B
     *
     *  @return         0 on success or a negative error code on failure
     */
    virtual int init();

    /** Deinitialize a block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    virtual int deinit();

    /** Read blocks from a block device
     *
     *  If a failure occurs, it is not possible to determine how many bytes succeeded
     *
     *  @param buffer   Buffer to write blocks to
     *  @param addr     Address of block to begin reading from
     *  @param size     Size to read in bytes, must be a multiple of read block size
     *  @return         0 on success, -1 on SPI error, -2 on malformed operation
     */
    virtual int read(void *buffer, bd_addr_t addr, bd_size_t size);

    /** Program blocks to a block device
     *
     *  The blocks must have been erased prior to being programmed
     *
     *  If a failure occurs, it is not possible to determine how many bytes succeeded
     *
     *  @param buffer   Buffer of data to write to blocks
     *  @param addr     Address of block to begin writing to
     *  @param size     Size to write in bytes, must be a multiple of program block size
     *  @return         0 on success, -1 on SPI error, -2 on malformed operation
     */
    virtual int program(const void *buffer, bd_addr_t addr, bd_size_t size);

    /** Erase blocks on a block device
     *
     *  The state of an erased block is undefined until it has been programmed,
     *  unless get_erase_value returns a non-negative byte value
     *
     *  @param addr     Address of block to begin erasing
     *  @param size     Size to erase in bytes, must be a multiple of erase block size
     *  @return         0 on success, -1 on SPI error, -2 on malformed operation
     */
    virtual int erase(bd_addr_t addr, bd_size_t size);

    /** Get the size of a readable block
     *
     *  @return         Size of a readable block in bytes
     */
    virtual bd_size_t get_read_size() const;

    /** Get the size of a programable block
     *
     *  @return         Size of a programable block in bytes
     *  @note Must be a multiple of the read size
     */
    virtual bd_size_t get_program_size() const;

    /** Get the size of a eraseable block
     *
     *  @return         Size of a eraseable block in bytes
     *  @note Must be a multiple of the program size
     */
    virtual bd_size_t get_erase_size() const;

    /** Get the value of storage when erased
     *
     *  If get_erase_value returns a non-negative byte value, the underlying
     *  storage is set to that value when erased, and storage containing
     *  that value can be programmed without another erase.
     *
     *  @return         The value of storage when erased, or -1 if you can't
     *                  rely on the value of erased storage
     */
    virtual int get_erase_value() const;

    /** Get the total size of the underlying device
     *
     *  @return         Size of the underlying device in bytes
     */
    virtual bd_size_t size() const;

    /** Get the BlockDevice class type.
     *
     *  @return         A string represent the BlockDevice class type.
     */
    virtual const char *get_type() const;

    /**
     * Puts the AT25DF041B into low-power standby mode
     *
     * @retval error Any errors that occur
     */
    int enter_standby(void);

    /**
     * Wakes the AT25DF041B up from standby
     *
     * @retval error 0 if the AT25DF041B was successfully woken up
     */
    int exit_standby(void);

    /**
     * Performs an entire chip erase
     *
     * @note THIS CANNOT BE REVERSED! It is unavailable in production firmware
     * This is a blocking call, it will take a while
     *
     * @param[in] magic_word You must input the magic word 0xADE570 to perform this
     */
    void perform_chip_erase(int magic_word);

protected:

    /**
     * Asserts the slave select pin, if there is one
     */
    inline void assert_slave_select(void) {
        _slave_select = 0;
    }

    /**
     * Deasserts the slave select pin, if there is one
     */
    inline void deassert_slave_select(void) {
        _slave_select = 1;
    }

    /**
     * Calculates the page address
     */
    inline bd_addr_t get_page_addr(bd_addr_t addr) {
        return (addr >> 8); // Simply divide by 256
    }

    /**
     * Enable write protection
     */
    void enable_write_protection(void);

    /**
     * Disable write protection
     */
    void disable_write_protection(void);

    /**
     * Gets the status register
     * @retval status status register byte
     */
    uint8_t get_status_register(void);

    /**
     * Gets the device ID
     * @param[in] id 3-byte buffer to store ID bytes
     */
    void get_device_id(uint8_t* id);

    /**
     * Checks the device ID to make sure the AT25DF041B is available
     * @retval available 0 if the AT25DF041B is available, -1 if it is not
     */
    int check_device_id(void);

    /**
     * Checks if the operation is valid
     *
     * @param[in] addr Start address of operation
     * @param[in] size Size of operation in bytes
     * @param[in] type Operation type (0 = read, 1 = program, 2 = erase)
     * @retval true if operation is valid, false otherwise
     */
    static bool is_valid_operation(bd_addr_t addr, bd_size_t size, int type);

    /**
     * Checks how many boundaries the desired operation crosses
     *
     *  @note For the AT25DF041B, a program cannot cross a page boundary
     *  This function will check whether the desired operation does so
     *
     *  @retval number_of_crossings The number of page boundaries this operation spans
     *
     */
    static int boundary_crossings(bd_addr_t addr, bd_size_t size);

    /**
     * Rounds the given address up to the nearest page boundary
     * Uses efficient bit shifting
     */
    inline bd_addr_t round_up_to_page_boundary(bd_addr_t addr) {
        // What this does
        // ((addr / 256) + 1) * 256;
        return ((addr >> 8) + 1) << 8;
    }

    /**
     * Prints an address onto the SPI bus properly formatted
     * for the AT25DF041B
     */
    void send_address(bd_addr_t addr);

    /**
     * Blocking loop that waits until the AT25DF041B is ready
     * Checks the RDY/BSY bit of the status register
     */
    void wait_for_ready(void);

protected:

    mbed::SPI _spi;
    mbed::DigitalOut _slave_select;
};

#endif
#endif

