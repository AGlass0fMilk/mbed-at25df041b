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

#if defined(DEVICE_SPI) || defined(DOXYGEN_ONLY)

#include "AT25DF041B.h"

#include "platform/mbed_wait_api.h"

AT25DF041B::AT25DF041B(PinName mosi, PinName miso, PinName sclk, PinName ssel) :
        _spi(mosi, miso, sclk), _slave_select(ssel, 1) {
}

int AT25DF041B::init() {
    int res = exit_standby();
    if (res) {
        return res;
    }

    disable_write_protection();

    // By default all sector protection bits are set (protected)
    // Unprotect all sectors during initialization using global unprotect
    assert_slave_select();

    _spi.write(AT25DF041B_WRITE_STATUS_REG);
    _spi.write(0);

    deassert_slave_select();

    enable_write_protection();

    // Read the status register and make sure SWP bits are 00 (all unprotected)
    if ((get_status_register() & 0x0C) != 0) {
        return -1;
    }

    return res;
}

int AT25DF041B::deinit() {
    // Disable writes
    enable_write_protection();

    // Enter standby
    return enter_standby();
}

int AT25DF041B::read(void *buffer, bd_addr_t addr, bd_size_t size) {
    if (check_device_id() == -1)
        return -1;

    if (!is_valid_operation(addr, size, AT25DF041B_OPERATION_TYPE_READ))
        return -2;

    // For reads, boundary crossings are not an issue
    assert_slave_select();
    _spi.write(AT25DF041B_READ_ARRAY);
    send_address(addr);
    _spi.write(NULL, 0, (char*) buffer, size);
    deassert_slave_select();

    return 0;
}

int AT25DF041B::program(const void *buffer, bd_addr_t addr, bd_size_t size) {
    if (check_device_id() == -1)
        return -1;

    if (!is_valid_operation(addr, size, AT25DF041B_OPERATION_TYPE_PROGRAM))
        return -2;

    int pages = boundary_crossings(addr, size) + 1;

    bd_addr_t start = addr;
    bd_addr_t chunk_size;
    for (int i = 0; i < pages; i++) {
        // Write protection is automatically enabled after
        // a program operation by the AT25DF041B
        disable_write_protection();

        assert_slave_select();

        chunk_size = round_up_to_page_boundary(start) - start;

        // Last iteration, chunk size may be smaller
        if (i == pages - 1)
            chunk_size = (addr + size) - start;

        // Perform program operation
        _spi.write(AT25DF041B_BYTE_PAGE_PROGRAM);
        send_address(start);
        _spi.write(&(((char*) buffer)[(start - addr)]), chunk_size, NULL, 0);

        deassert_slave_select();

        // Blocking wait until the AT25DF041B finishes program operation
        wait_for_ready();

        start += chunk_size;
    }

    return 0;
}

int AT25DF041B::erase(bd_addr_t addr, bd_size_t size) {
    if (check_device_id() == -1)
        return -1;

    if (!is_valid_operation(addr, size, AT25DF041B_OPERATION_TYPE_ERASE))
        return -2;

    /** TODO make it possible to select PAGE BYTE erase sizes */
    //int sectors = size / AT25DF041B_PAGE_BYTE_SIZE;
    //int sectors = size >> 8; // More efficient
    // For compatibility with our bootloader, we need 4kB erase sectors
    //int sectors = size / 4096;
    int sectors = size >> 12; // More efficient

    bd_addr_t start = addr;
    for (int i = 0; i < sectors; i++) {

        // Write protection is automatically enabled after
        // an erase operation by the AT25DF041B
        disable_write_protection();

        assert_slave_select();
        //_spi.write(AT25DF041B_PAGE_ERASE_256B);
        _spi.write(AT25DF041B_BLOCK_ERASE_4KB);
        send_address(start);
        deassert_slave_select();

        // Move to next 256B page
        //start += AT25DF041B_PAGE_BYTE_SIZE;

        // Move to the next 4kB sector
        start += 4096;

        // Blocking wait until the AT25DF041B finishes erase operation
        wait_for_ready();
    }

    return 0;
}

bd_size_t AT25DF041B::get_read_size() const {
    return 1;
}

bd_size_t AT25DF041B::get_program_size() const {
    return 1;
}

bd_size_t AT25DF041B::get_erase_size() const {
    //return AT25DF041B_PAGE_BYTE_SIZE;
    return 4096;
}

int AT25DF041B::get_erase_value() const {
    return AT25DF041B_ERASE_VALUE;
}

bd_size_t AT25DF041B::size() const {
    return AT25DF041B_TOTAL_BYTE_SIZE;
}

const char* AT25DF041B::get_type() const {
    static char bd_type_name[] = "AT25DF041B";
    return bd_type_name;
}

int AT25DF041B::enter_standby(void) {
    assert_slave_select();
    _spi.write(AT25DF041B_ULTRA_POWER_DOWN);
    deassert_slave_select();
    return 0;
}

int AT25DF041B::exit_standby(void) {

    // If the AT25DF041B is in ultra deep power down, this will wake it up
    assert_slave_select();
    wait_ns(100); // Need to pulse CS for at least 20ns
    deassert_slave_select();

    // Wait 100us before continuing (startup latency after exiting UDPD is 70us MAX)
    wait_us(100);

    // Make sure the AT25DF041B is there and awake
    if (check_device_id() == 0)
        return 0;
    else
        return -1;
}

void AT25DF041B::perform_chip_erase(int magic_word) {
    // Not for use in production firmware
#ifndef NDEBUG
    if (magic_word == AT25DF041B_CHIP_ERASE_MAGIC_WORD) {
        disable_write_protection();
        assert_slave_select();
        _spi.write(AT25DF041B_CHIP_ERASE_2);
        deassert_slave_select();

        // NOTE this will wait for a long time!
        wait_for_ready();
    }
#endif
}

void AT25DF041B::enable_write_protection(void) {
    // Issue write disable command
    assert_slave_select();
    _spi.write(AT25DF041B_WRITE_DISABLE);
    deassert_slave_select();
}

void AT25DF041B::disable_write_protection(void) {
    // Issue write enable command
    assert_slave_select();
    _spi.write(AT25DF041B_WRITE_ENABLE);
    deassert_slave_select();
}

uint8_t AT25DF041B::get_status_register(void) {
    uint8_t status;
    assert_slave_select();
    _spi.write(AT25DF041B_READ_STATUS_REG);
    status = _spi.write(AT25DF041B_DUMMY_BYTE);
    deassert_slave_select();
    return status;
}

void AT25DF041B::get_device_id(uint8_t *id) {
    assert_slave_select();
    _spi.write(AT25DF041B_READ_MFG_AND_DEV_ID);
    for (int i = 0; i < 3; i++) {
        id[i] = _spi.write(AT25DF041B_DUMMY_BYTE);
    }
    deassert_slave_select();
}

int AT25DF041B::check_device_id(void) {
    uint8_t id[3];
    get_device_id(id);

    if (id[0] != AT25DF041B_MANUFACTURER_ID)
        return -1;
    if (id[1] != AT25DF041B_DEVICE_ID_BYTE_1)
        return -1;
    if (id[2] != AT25DF041B_DEVICE_ID_BYTE_2)
        return -1;

    return 0;
}

bool AT25DF041B::is_valid_operation(bd_addr_t addr, bd_size_t size, int type) {
    switch (type) {
    case AT25DF041B_OPERATION_TYPE_ERASE:
        // If the size is not erase-block aligned, invalid
        //if(((size % AT25DF041B_PAGE_BYTE_SIZE) != 0))
        if (((size % 4096) != 0)) {
            return false;
        }
        // fall through to next cases
        /* no break */

    case AT25DF041B_OPERATION_TYPE_PROGRAM:
        // fall through to next cases
        /* no break */

    case AT25DF041B_OPERATION_TYPE_READ:
        // Out of bounds are invalid
        if (addr >= AT25DF041B_TOTAL_BYTE_SIZE
                || (addr + size) > AT25DF041B_TOTAL_BYTE_SIZE) {
            return false;
        }
        // fall through to next cases
        /* no break */

    default:
        // Zero size operations are always invalid
        if (size == 0) {
            return false;
        }
        break;

    }

    return true;
}

int AT25DF041B::boundary_crossings(bd_addr_t addr, bd_size_t size) {
    // Calculate the pages of start and end addresses
    //bd_addr_t start_page = addr >> 8; // / AT25DF041B_PAGE_BYTE_SIZE;
    //bd_addr_t end_page = (addr + (size - 1)) >> 8;

    return (((addr + (size - 1)) >> 8) - (addr >> 8));
}

void AT25DF041B::send_address(bd_addr_t addr) {
    char address_bytes[3] = { (char) ((addr & 0xFF0000) >> 16), (char) ((addr
            & 0x00FF00) >> 8), (char) ((addr & 0x0000FF) >> 0) };
    _spi.write(address_bytes, 3, NULL, 0);
}

void AT25DF041B::wait_for_ready(void) {
    uint8_t status = 0;
    do {
        status = get_status_register();
    } while (status & AT25DF041B_STATUS_READY_BUSY_BIT);
}

#endif
