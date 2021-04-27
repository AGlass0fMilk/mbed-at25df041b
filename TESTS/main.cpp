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

/** Standard test headers */
#include "greentea-client/test_env.h"
#include "utest/utest.h"
#include "unity/unity.h"

/** mbed drivers */
#include "drivers/SPI.h"
//#include "mbedtls/entropy.h"
//#include "mbedtls/ctr_drbg.h"

#include "AT25DF041B.h"
#include "PinNames.h"

using namespace utest::v1;

/** Test class to access protected member functions */
class AT25DF041BTest : public AT25DF041B
{
	public:
		AT25DF041BTest(PinName mosi, PinName miso, PinName sclk, PinName cs) :
		    AT25DF041B(mosi, miso, sclk, cs) {
		    _spi.frequency(250E3); // Set to a slow frequency for easier capture
		}

		int check_device_id_wrapper(void)
		{
			return check_device_id();
		}

		static bool is_valid_operation_wrapper(bd_addr_t addr, bd_size_t size, int type)
		{
			return is_valid_operation(addr, size, type);
		}

		static int boundary_crossings_wrapper(bd_addr_t addr, bd_size_t size)
		{
			return boundary_crossings(addr, size);
		}

		bd_addr_t page_boundary_round_wrapper(bd_addr_t addr)
		{
			return round_up_to_page_boundary(addr);
		}
};

AT25DF041BTest flash(SPI_MOSI, SPI_MISO, SPI_SCLK, SPI_CS);

/** Static test data */
const static uint8_t static_bytes[1024] =
{
		0xD1,0xE8,0xFE,0x53,0x6D,0x49,0xCC,0xCD,0x16,0x85,0xF1,0xF7,0x4,0x71,0xFB,0x9C,
		0xEE,0x9F,0x48,0x66,0x46,0x2D,0x77,0x93,0xC4,0xDE,0xFC,0xB9,0xB4,0x1C,0xEC,0xD9,
		0x5C,0x5A,0xE1,0x6B,0x84,0xC8,0xE0,0xDE,0xB4,0x33,0x63,0x26,0xAA,0xBF,0x34,0x1E,
		0xAD,0xB,0xF9,0x4E,0x3F,0x22,0x58,0x6,0x81,0x7C,0xA8,0x81,0x6E,0xBB,0x3D,0xB8,
		0xC9,0x4C,0x62,0x37,0x8B,0xCA,0x2D,0xD4,0xF3,0x86,0xE1,0x6E,0xD1,0x0,0x6D,0xA8,
		0x85,0x54,0x2E,0x21,0x43,0xEB,0xAD,0x54,0x3F,0x93,0xA6,0x3C,0x60,0x4E,0xFB,0xD2,
		0x89,0xF1,0x4C,0x9E,0x42,0xF6,0x9D,0xB8,0x90,0x8C,0x82,0x18,0xC4,0xF8,0x45,0xD1,
		0x6,0xFB,0x93,0x42,0xF4,0x5C,0xB1,0x7F,0x52,0x56,0x69,0xB2,0xCE,0xDD,0x9B,0xF0,
		0x33,0x16,0xBD,0x77,0x1F,0xF1,0x8F,0x14,0x70,0x50,0x85,0x5B,0x52,0x2,0xF6,0x2C,
		0xC,0xDA,0x8B,0x3D,0x18,0xD5,0xFD,0x43,0x64,0xEC,0xE,0xB9,0x60,0x6E,0x7D,0xC3,
		0xD1,0x70,0x3C,0x8,0xE3,0x18,0xD7,0xED,0x88,0xAE,0xCD,0x73,0x91,0x31,0x71,0x8B,
		0x93,0xEE,0x55,0xF,0x4D,0xFE,0x60,0x6A,0x40,0x64,0x11,0x5A,0xC,0xAB,0xB,0xF3,
		0x7B,0xEE,0xBB,0x74,0x62,0x87,0x65,0xAC,0x8C,0x4A,0xB0,0xBF,0x39,0x71,0xB4,0x41,
		0xEE,0x86,0x80,0x7C,0xA4,0x9F,0x44,0xD,0xA9,0x87,0x80,0x84,0x52,0xB9,0xD1,0x8E,
		0xB4,0x2E,0xC6,0xE9,0x79,0x7B,0x5,0xC1,0xAC,0x0,0x11,0x81,0x2D,0x76,0xA,0x46,
		0x33,0x0,0x14,0x9D,0xAE,0xF7,0x2D,0x94,0xD7,0xF0,0x9E,0x33,0x40,0xE,0xE5,0x77,
		0xBC,0xDE,0x36,0xCD,0xCB,0xAC,0x4D,0xF5,0xEA,0xF5,0xE,0x8E,0x66,0xEC,0xA9,0xF7,
		0x60,0xBE,0x8D,0x99,0x18,0xA1,0x2D,0x8F,0x4C,0xE3,0x86,0xE9,0x3B,0x8D,0x91,0x14,
		0x4E,0x79,0xD,0xBA,0xEA,0xB0,0x20,0xF5,0xF,0x7B,0x4A,0xA3,0xFB,0xD3,0xCA,0x8B,
		0x6A,0xE,0xD3,0xFD,0x6A,0x65,0x3F,0x9D,0xAD,0x4B,0x4,0xAE,0xBB,0xF4,0x50,0x11,
		0x17,0x3C,0xF,0xB7,0x50,0x7B,0xE1,0x8E,0xC1,0x62,0xAB,0x9E,0xC8,0xDF,0xB7,0x2D,
		0xDD,0x48,0xD2,0xB6,0xF7,0x82,0x1A,0x9A,0x8C,0x85,0x44,0x85,0x2F,0x9C,0xB0,0x77,
		0x17,0xAB,0xA3,0x82,0xA0,0xD6,0xC5,0x64,0x66,0x63,0x38,0x34,0x78,0x49,0x6B,0xCA,
		0xA9,0x67,0xCF,0x92,0x5F,0x47,0xA9,0xAD,0x7E,0xDD,0xA8,0x14,0x5E,0xDB,0xDC,0xC3,
		0x17,0xDA,0xDA,0x33,0xCD,0x55,0xFC,0xE,0xDF,0xE8,0xB5,0x20,0x52,0xE1,0x51,0xC9,
		0x6C,0x2B,0x8,0xF4,0x9,0x45,0xE7,0x10,0x6B,0x7F,0x74,0x55,0xBA,0x91,0xEE,0x67,
		0xC4,0x3C,0x35,0xE,0x50,0x8C,0xD6,0x52,0x5A,0x99,0x69,0x95,0x69,0xB4,0x33,0xD6,
		0x8F,0x96,0xD0,0x63,0x7F,0x44,0x86,0x3,0x64,0x84,0xD1,0x19,0x1F,0x5B,0x1,0xAB,
		0x7B,0xBF,0xF8,0x31,0x62,0x9C,0x63,0x66,0xBA,0x97,0x3F,0x76,0x66,0xE8,0xA8,0x64,
		0x17,0x8,0x51,0xCF,0xA6,0x3A,0x2D,0xA2,0x7E,0xAC,0xF8,0x4B,0x8A,0xC8,0x82,0xDD,
		0xE7,0x38,0x13,0x11,0xFE,0x3A,0x66,0xBD,0x97,0xEF,0xB4,0x11,0x40,0x5,0x6D,0x94,
		0x11,0x32,0x39,0xF2,0x2C,0xE5,0x55,0x4F,0xD3,0xDA,0x5B,0xC3,0x68,0xBC,0x99,0x2,
		0x1A,0x2E,0x31,0xE8,0x85,0x46,0x51,0x3B,0xF9,0xAC,0xA9,0x6F,0x8,0xD6,0x9B,0x4B,
		0xAE,0x14,0x9C,0x81,0x9,0xC9,0x4,0x92,0xF1,0xA5,0xBB,0xF9,0xB6,0xF,0xF7,0xD9,
		0x32,0xE3,0x0,0xF1,0x57,0xDB,0xC,0x76,0x8C,0x17,0xC7,0xF8,0x28,0x88,0xA5,0xC7,
		0x37,0x19,0xD8,0xAF,0x1F,0x39,0x5E,0x2E,0x48,0x33,0xEF,0x1,0xA0,0xC8,0x1D,0xFF,
		0x51,0xAF,0xC9,0xF7,0xCA,0xC5,0xF1,0x42,0x7,0x6,0xD9,0x66,0x68,0x92,0xC8,0x4,
		0xBE,0xF2,0xC2,0xAE,0x63,0x7A,0xB4,0x56,0xDF,0xC8,0x78,0xF4,0xBB,0xD9,0x7A,0xB7,
		0x2D,0x93,0xB,0x9B,0x62,0xEF,0xB,0xA7,0x59,0xC6,0x8E,0xA,0x7C,0xF3,0xFE,0x8,
		0xE,0x38,0x4D,0x72,0xB9,0xCF,0x27,0x9C,0x53,0xEF,0xA4,0x40,0x84,0x31,0x55,0x98,
		0x2A,0x22,0xD8,0x78,0xAF,0xCD,0xF2,0xDC,0xBD,0x96,0x99,0x16,0xE3,0xA3,0xB0,0xCC,
		0xA,0x30,0xBB,0x1,0x46,0x8B,0xCF,0xDF,0x5D,0xF9,0xF8,0x96,0x76,0x72,0x1E,0xDB,
		0x18,0x53,0xB9,0x65,0x7F,0x77,0x1F,0x60,0xFD,0x88,0xA0,0xBF,0x23,0xE7,0x9E,0x64,
		0xB9,0x8,0x95,0x62,0xA2,0x72,0xB7,0xD8,0x62,0x71,0x78,0x6B,0xEE,0x36,0xC6,0x9,
		0x69,0xA0,0x67,0xEF,0xDE,0x20,0x7F,0xDC,0xF3,0x4A,0x8A,0x5,0xFF,0x67,0x79,0xE,
		0xB5,0x2C,0x11,0x68,0x58,0x52,0xD2,0x3C,0x5B,0xE6,0x34,0xFB,0xC8,0x92,0x22,0x3F,
		0x1F,0xC2,0x4E,0x7D,0xFC,0xB3,0xD5,0xAB,0xA2,0x71,0x92,0xDD,0x58,0x74,0xC2,0xFA,
		0x55,0xDC,0x87,0x94,0x83,0xC7,0x41,0xB9,0x44,0xDD,0xD4,0x81,0x4B,0x94,0xF3,0x86,
		0x38,0xC6,0x4F,0x65,0xC7,0x87,0x14,0xD0,0xFB,0x9D,0x3F,0x19,0x69,0x95,0x35,0x3C,
		0x22,0x5C,0x61,0xAA,0xE4,0xC5,0x7F,0xE9,0x56,0xA,0x64,0xAB,0xE3,0xE7,0x12,0x65,
		0xC1,0xD,0x90,0x1C,0xC4,0x88,0x9E,0x68,0xFF,0xD7,0x35,0x6C,0x33,0xF3,0x95,0xE5,
		0x37,0x75,0x30,0xE6,0x9,0x6A,0x7D,0xB4,0xC,0x49,0x90,0x75,0x86,0xAF,0xFD,0x91,
		0x7F,0x77,0x9,0x9E,0xFC,0xD2,0xB4,0xEE,0x26,0xC2,0x6B,0x6,0x81,0xCB,0xBE,0x23,
		0xB3,0x6,0x66,0x82,0x11,0x30,0xEB,0x32,0x2C,0x74,0x17,0x83,0xC3,0xB6,0x7D,0x51,
		0x41,0x5A,0xB,0xD2,0xC7,0xAF,0xA7,0x13,0x1E,0xC1,0x95,0x89,0xE4,0x9A,0x6D,0x14,
		0x95,0xA3,0x95,0xBA,0x19,0x91,0xB8,0xDD,0x0,0x11,0x2D,0x6D,0xBE,0x6,0x8C,0xEC,
		0xC2,0x91,0x56,0x1F,0x72,0x58,0x53,0x61,0x5F,0x19,0x50,0xED,0xCB,0xFE,0xD3,0x2B,
		0x78,0xCD,0x39,0xA4,0x7,0xDB,0xAD,0x6D,0xDD,0x39,0x29,0x63,0x62,0x6B,0x81,0xCC,
		0xBF,0xEF,0xED,0x93,0x64,0x6C,0xFB,0x24,0xE9,0x80,0x80,0x60,0x9E,0x57,0x22,0xEB,
		0x33,0x5A,0xE,0xE7,0x37,0xDA,0x87,0xD1,0x5,0x97,0x37,0x61,0x5,0x35,0xF4,0x52,
		0xBF,0xE1,0x18,0xA4,0x12,0x69,0x8D,0x1,0x7D,0xAC,0x9B,0xB5,0x20,0xFB,0x57,0xCB,
		0x5D,0x5F,0xE5,0x87,0x95,0xCA,0x2,0xAD,0x52,0xCA,0xC2,0xFA,0x7E,0x25,0xDE,0xCB,
		0xD5,0x13,0x4D,0x9F,0x80,0xE1,0x81,0x78,0xD6,0xFB,0x22,0xB4,0xDD,0x89,0x13,0x3A,
		0x9E,0xDB,0xE9,0xD1,0x42,0x7F,0x9,0x90,0x38,0x23,0x84,0xFB,0x56,0xC0,0x97,0xCA
};

int round_up_to_next_erase_size(BlockDevice* bd, int in) {
    int es = bd->get_erase_size();
    return ((in + es - 1)/es)*es;
}

/** Global test buffer */
uint8_t test_buffer[1024];

/** Random test buffer */
//uint8_t random_buffer[1024];

void test_device_characteristics(void)
{
	bd_size_t expected, actual;

	// Verify proper read size
	expected = 1;
	actual = flash.get_read_size();
	TEST_ASSERT_EQUAL(expected, actual);

	// Verify proper program size
	expected = 1;
	actual = flash.get_program_size();
	TEST_ASSERT_EQUAL(expected, actual);

	// Verify proper erase size
	expected = AT25DF041B_PAGE_BYTE_SIZE;
	actual = flash.get_erase_size();
	TEST_ASSERT_EQUAL_HEX64(expected, actual);

	// Verify proper erased value
	expected = AT25DF041B_ERASE_VALUE;
	actual = flash.get_erase_value();
	TEST_ASSERT_EQUAL_HEX8(expected, actual);

	// Verify proper size
	expected = AT25DF041B_PAGE_BYTE_SIZE * AT25DF041B_PAGE_COUNT;
	actual = flash.size();
	TEST_ASSERT_EQUAL_HEX64(expected, actual);

}

void test_is_valid_operation(void)
{
	bd_addr_t addr;
	bd_size_t size;
	int type;
	bool expected, actual;

	/** Test erasure validation **/
	type = AT25DF041B_OPERATION_TYPE_ERASE;

	// Test unaligned size erase
	addr = 2 * AT25DF041B_PAGE_BYTE_SIZE;
	size = AT25DF041B_PAGE_BYTE_SIZE + 2;
	expected = false;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Unaligned size erase");

	// Test erase past bounds of memory
	addr = AT25DF041B_PAGE_BYTE_SIZE * (AT25DF041B_PAGE_COUNT - 1);
	size = AT25DF041B_PAGE_COUNT * 2;
	expected = false;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Out of bounds erase");

	// Test program beyond bounds of memory
	addr = AT25DF041B_PAGE_BYTE_SIZE * AT25DF041B_PAGE_COUNT;
	size = AT25DF041B_PAGE_BYTE_SIZE;
	expected = false;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Out of bounds erase 2");

	// Test 0 size erase
	size = 0;
	expected = false;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Zero size erase");

	// Test aligned erasure of multiple sectors
	addr = AT25DF041B_PAGE_BYTE_SIZE * 2;
	size = AT25DF041B_PAGE_BYTE_SIZE * 3;
	expected = true;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Aligned erasure, multi-sector");

	/** Test program validation **/
	type = AT25DF041B_OPERATION_TYPE_PROGRAM;

	// Test program past bounds of memory
	addr = AT25DF041B_PAGE_BYTE_SIZE * (AT25DF041B_PAGE_COUNT - 1);
	size = AT25DF041B_PAGE_BYTE_SIZE * 2;
	expected = false;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Out of bounds program");

	// Test program beyond bounds of memory
	addr = AT25DF041B_PAGE_BYTE_SIZE * AT25DF041B_PAGE_COUNT;
	size = 128;
	expected = false;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Out of bounds program 2");

	// Test 0 size program
	size = 0;
	expected = false;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Zero size program");

	// Test single page program
	addr = 0;
	size = 128;
	expected = true;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Single page program");

	// Test multi-page program unaligned
	addr = 128;
	size = 743;
	expected = true;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Multi-page program unaligned");

	/** Test read validation **/
	type = AT25DF041B_OPERATION_TYPE_READ;

	// Test read past bounds of memory
	addr = AT25DF041B_PAGE_BYTE_SIZE * (AT25DF041B_PAGE_COUNT - 1);
	size = AT25DF041B_PAGE_BYTE_SIZE * 2;
	expected = false;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Out of bounds read");

	// Test read beyond bounds of memory
	addr = AT25DF041B_PAGE_BYTE_SIZE * AT25DF041B_PAGE_COUNT;
	size = 128;
	expected = false;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Out of bounds read 2");

	// Test 0 size read
	size = 0;
	expected = false;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Zero size read");

	// Test single page read
	addr = 0;
	size = 128;
	expected = true;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Single page read");

	// Test multi-page read unaligned
	addr = 128;
	size = 743;
	expected = true;
	actual = flash.is_valid_operation_wrapper(addr, size, type);
	TEST_ASSERT_EQUAL_MESSAGE(expected, actual, "Multi-page read unaligned");
}

void test_page_boundary_crossings(void)
{
	bd_addr_t addr;
	bd_size_t size;
	int expected, actual;

	/** Test boundary_crossings */

	// No boundary crossing, aligned
	addr = 0;
	size = (AT25DF041B_PAGE_BYTE_SIZE / 2);
	expected = 0;
	actual = flash.boundary_crossings_wrapper(addr, size);
	TEST_ASSERT_EQUAL(expected, actual);

	// One page boundary crossing, aligned
	addr = 0;
	size = ((AT25DF041B_PAGE_BYTE_SIZE / 2) * 3);
	expected = 1;
	actual = flash.boundary_crossings_wrapper(addr, size);
	TEST_ASSERT_EQUAL(expected, actual);

	// Single page boundary (does not count as crossing), exact
	addr = 0;
	size = AT25DF041B_PAGE_BYTE_SIZE;
	expected = 0;
	actual = flash.boundary_crossings_wrapper(addr, size);
	TEST_ASSERT_EQUAL(expected, actual);

	// One page boundary crossing, exact
	addr = AT25DF041B_PAGE_BYTE_SIZE;
	size = (AT25DF041B_PAGE_BYTE_SIZE * 2);
	expected = 1;
	actual = flash.boundary_crossings_wrapper(addr, size);
	TEST_ASSERT_EQUAL(expected, actual);

	// No boundary crossing, unaligned
	addr = 123;
	size = 54;
	expected = 0;
	actual = flash.boundary_crossings_wrapper(addr, size);
	TEST_ASSERT_EQUAL(expected, actual);

	// One page boundary crossing, unaligned
	addr = (AT25DF041B_PAGE_BYTE_SIZE - 6);
	size = 7;
	expected = 1;
	actual = flash.boundary_crossings_wrapper(addr, size);
	TEST_ASSERT_EQUAL(expected, actual);

	// No boundary cross, unaligned, exact
	addr = AT25DF041B_PAGE_BYTE_SIZE - 43;
	size = 43;
	expected = 0;
	actual = flash.boundary_crossings_wrapper(addr, size);
	TEST_ASSERT_EQUAL(expected, actual);

	// One boundary cross, unaligned, exact
	addr = ((AT25DF041B_PAGE_BYTE_SIZE * 3) - 37);
	size = ((AT25DF041B_PAGE_BYTE_SIZE) + 37);
	expected = 1;
	actual = flash.boundary_crossings_wrapper(addr, size);
	TEST_ASSERT_EQUAL(expected, actual);

	// Several boundary crossings, unaligned
	addr = ((AT25DF041B_PAGE_BYTE_SIZE * 7) + 59);
	size = ((AT25DF041B_PAGE_BYTE_SIZE + 3) * 3);
	expected = 3;
	actual = flash.boundary_crossings_wrapper(addr, size);
	TEST_ASSERT_EQUAL(expected, actual);
}

void test_page_boundary_rounding(void)
{
	bd_addr_t expected, actual;

	/** Test rounding up to page boundaries */

	// Just below page boundary
	expected = AT25DF041B_PAGE_BYTE_SIZE * 4;
	actual = (AT25DF041B_PAGE_BYTE_SIZE * 4) - 1;
	actual = flash.page_boundary_round_wrapper(actual);
	TEST_ASSERT_EQUAL_HEX64(expected, actual);

	// Special case, input address is a page boundary
	// In this case, it should round up to the next page boundary
	expected = AT25DF041B_PAGE_BYTE_SIZE * 3;
	actual = AT25DF041B_PAGE_BYTE_SIZE * 2;
	actual = flash.page_boundary_round_wrapper(actual);
	TEST_ASSERT_EQUAL_HEX64(expected, actual);
	TEST_ASSERT_EQUAL((expected - (AT25DF041B_PAGE_BYTE_SIZE * 2)),
			AT25DF041B_PAGE_BYTE_SIZE);

	// General case
	expected = AT25DF041B_PAGE_BYTE_SIZE * 15;
	actual = (AT25DF041B_PAGE_BYTE_SIZE * 14) + 34;
	actual = flash.page_boundary_round_wrapper(actual);
	TEST_ASSERT_EQUAL_HEX64(expected, actual);

}

// Also tests the wakeup command
status_t test_setup_check_device_id(const Case *const source, const size_t index_of_case)
{
	TEST_ASSERT_EQUAL(0, flash.init());
	return greentea_case_setup_handler(source, index_of_case);
}

void test_check_device_id(void)
{
	TEST_ASSERT_EQUAL(0, flash.check_device_id_wrapper());
}

// Sets up flash read/program/erase tests
// Erases the whole chip
// Resets the test buffer
status_t test_setup_flash(const Case *const source, const size_t index_of_case)
{
	// Zero out the test buffer
	memset(test_buffer, 0, 512);

	// Erase chip - blocking call
	//flash.perform_chip_erase(AT25DF041B_CHIP_ERASE_MAGIC_WORD);

	//wait_ms(10);

	// Make sure AT25DF041B is awake
	//flash.exit_standby();

	return greentea_case_setup_handler(source, index_of_case);
}

int find_first_diff(const uint8_t* lhs, const uint8_t* rhs, bd_size_t size) {
    for(int i = 0; i < size; i++) {
        uint8_t lhv = lhs[i], rhv = rhs[i];
        if(lhv != rhv) {
            return i;
        }
    }
    return -1;
}

bool is_all_erased(BlockDevice* bd, const uint8_t* in, bd_size_t size) {
    for(int i = 0; i < size; i++) {
        if(in[i] != bd->get_erase_value()) {
            return false;
        }
    }
    return true;
}

// Constant data flash test
void test_constant_read_program_erase(void)
{
	bd_addr_t addr;
	bd_size_t size;

	/** Test under 1 page boundary operations, aligned*/
	addr = 0;
	size = 83;
	TEST_ASSERT_EQUAL(0, flash.erase(addr, round_up_to_next_erase_size(&flash, size)));
    TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
    TEST_ASSERT_EQUAL(true, is_all_erased(&flash, test_buffer, size));
	TEST_ASSERT_EQUAL(0, flash.program(static_bytes, addr, size));
	TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
	TEST_ASSERT_EQUAL(0, memcmp(static_bytes, test_buffer, size));

	/** Test under 1 page boundary operations, unaligned*/
	addr = 8257;
	size = 83;
    TEST_ASSERT_EQUAL(0, flash.erase(addr, round_up_to_next_erase_size(&flash, size)));
    TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
    TEST_ASSERT_EQUAL(true, is_all_erased(&flash, test_buffer, size));

    TEST_ASSERT_EQUAL(0, flash.program(static_bytes, addr, size));
	TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
	TEST_ASSERT_EQUAL(0, memcmp(static_bytes, test_buffer, size));

	/** Test exactly 1 page boundary operations, aligned*/
	addr = 0;
	size = AT25DF041B_PAGE_BYTE_SIZE;
    TEST_ASSERT_EQUAL(0, flash.erase(addr, round_up_to_next_erase_size(&flash, size)));
    TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
    TEST_ASSERT_EQUAL(true, is_all_erased(&flash, test_buffer, size));
    TEST_ASSERT_EQUAL(0, flash.program(static_bytes, addr, size));
	TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
	TEST_ASSERT_EQUAL(0, memcmp(static_bytes, test_buffer, size));

	/** Test exactly 1 page boundary operation, unaligned */
	addr = 8257;
	size = 191;
	TEST_ASSERT_EQUAL(0, flash.boundary_crossings_wrapper(addr, size));
    TEST_ASSERT_EQUAL(0, flash.erase(addr, round_up_to_next_erase_size(&flash, size)));
    TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
    TEST_ASSERT_EQUAL(true, is_all_erased(&flash, test_buffer, size));
    TEST_ASSERT_EQUAL(0, flash.program(static_bytes, addr, size));
	TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
	TEST_ASSERT_EQUAL(0, memcmp(static_bytes, test_buffer, size));

	/** Test multiple page boundary operations, aligned */
	addr = 0;
	size = 568;
    TEST_ASSERT_EQUAL(0, flash.erase(addr, round_up_to_next_erase_size(&flash, size)));
    TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
    TEST_ASSERT_EQUAL(true, is_all_erased(&flash, test_buffer, size));
    TEST_ASSERT_EQUAL(0, flash.program(static_bytes, addr, size));
	TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
	find_first_diff(static_bytes, test_buffer, size);
	TEST_ASSERT_EQUAL(0, memcmp(static_bytes, test_buffer, size));

	/** Test multiple page boundary operations, unaligned */
	addr = 234;
	size = 568;
    TEST_ASSERT_EQUAL(0, flash.erase(addr, round_up_to_next_erase_size(&flash, addr + size)));
    TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
    TEST_ASSERT_EQUAL(true, is_all_erased(&flash, test_buffer, size));
    TEST_ASSERT_EQUAL(0, flash.program(static_bytes, addr, size));
	TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
	TEST_ASSERT_EQUAL(0, memcmp(static_bytes, test_buffer, size));

	/** Test exact multiple page boundary operations, aligned */
	addr = 0;
	size = 768;
    TEST_ASSERT_EQUAL(0, flash.erase(addr, round_up_to_next_erase_size(&flash, size)));
    TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
    TEST_ASSERT_EQUAL(true, is_all_erased(&flash, test_buffer, size));
    TEST_ASSERT_EQUAL(0, flash.program(static_bytes, addr, size));
	TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
	TEST_ASSERT_EQUAL(0, memcmp(static_bytes, test_buffer, size));

	/** Test exact multiple page boundary operations, unaligned */
	addr = 20;
	size = 748;
    TEST_ASSERT_EQUAL(0, flash.erase(addr, round_up_to_next_erase_size(&flash, addr + size)));
    TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
    TEST_ASSERT_EQUAL(true, is_all_erased(&flash, test_buffer, size));
    TEST_ASSERT_EQUAL(0, flash.program(static_bytes, addr, size));
	TEST_ASSERT_EQUAL(0, flash.read(test_buffer, addr, size));
	TEST_ASSERT_EQUAL(0, memcmp(static_bytes, test_buffer, size));
}

// Random data flash test
void test_random_read_program_erase(void)
{
	// Random number generator contexts
	/*mbedtls_entropy_context ent;
	mbedtls_ctr_drbg_context rng;

	// Initialize the random number generator
	mbedtls_entropy_init(&ent);
	mbedtls_ctr_drbg_init(&rng);

	TEST_ASSERT_EQUAL_MESSAGE(0,
			mbedtls_ctr_drbg_seed(&rng, mbedtls_entropy_func, &ent, NULL, 0),
			"Could not initialize the random number generator");

	// Generate a size (up to 1024)
	bd_size_t random_size = 0;
	mbedtls_ctr_drbg_random(&rng, (uint8_t*) &random_size, 2);
	if(random_size > 1024)
		random_size = 1024;

	// Generate an address (up to flash.size() - size)
	bd_addr_t random_address = 0;
	mbedtls_ctr_drbg_random(&rng, (uint8_t*) &random_address, 4);
	if(random_address >= (flash.size() - random_size))
		random_address = (flash.size() - random_size) - 1;

	// Generate random data
	mbedtls_ctr_drbg_random(&rng, (uint8_t*) &random_buffer, random_size);

	greentea_send_kv("Random Addr", random_address);
	greentea_send_kv("Random Size", random_size);

	// Run the test
	TEST_ASSERT_EQUAL(0, flash.erase(random_address, flash.get_erase_size()));
	TEST_ASSERT_EQUAL(0, flash.program(static_bytes, random_address, random_size));
	TEST_ASSERT_EQUAL(0, flash.read(test_buffer, random_address, random_size));
	TEST_ASSERT_EQUAL(0, memcmp(static_bytes, test_buffer, random_size));*/

	// TODO actually get this working

	// TODO have greentea automatically repeat this a number of times
}

// Custom setup handler required for proper Greentea support
status_t greentea_setup(const size_t number_of_cases) {
    GREENTEA_SETUP(20, "default_auto");


    // Call the default reporting function
    return greentea_test_setup_handler(number_of_cases);
}

// Specify all your test cases here
Case cases[] = {
	Case("Device Characteristics", test_device_characteristics),
	Case("Operation Validation", test_is_valid_operation),
	Case("Page Boundary Crossing Formula", test_page_boundary_crossings),
	Case("Page Boundary Rounding Formula", test_page_boundary_rounding),
	Case("Check Device ID", test_setup_check_device_id, test_check_device_id),
	Case("Constant Data Read/Program/Erase", test_setup_flash, test_constant_read_program_erase),
	//Case("Random Data Read/Program/Erase", test_setup_flash, test_random_read_program_erase)
};

// Declare your test specification with a custom setup handler
Specification specification(greentea_setup, cases);

int main(void)
{
  Harness::run(specification);
  return 0;
}
