/*
 *  hu01_decompressor.h
 *  MSNP13Tester
 *
 *  Created by Daniel Parnell on 22/07/08.
 *  Copyright 2008 Automagic Software Pty Ltd. All rights reserved.
 *
 */

#define HU_BAD_HEADER -1
#define HU_BAD_VERSION -2
#define HU_BAD_HEADER_SIZE -3
#define HU_BAD_BLOCK_HEADER -4
#define HU_BAD_COMPRESSION_TABLE -5
#define HU_BLOCK_DECOMPRESS_FAILED -6

#define XDEBUG_DECOMPRESSION

#ifdef DEBUG_DECOMPRESSION
extern unsigned char* expected_data;
#endif

extern int decompress_hu01_data(const unsigned char* hu01_buffer, int hu01_buffer_size, unsigned char* buffer);
