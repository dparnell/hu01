/*
 *  hu01_decompressor.c
 *  MSNP13Tester
 *
 *  Created by Daniel Parnell on 22/07/08.
 *  Copyright 2008 Automagic Software Pty Ltd. All rights reserved.
 *
 */

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "hu01_decompressor.h"

#define HU_SIZE_OFFSET 0x20

//#define DEBUG
//#define DEBUG_TABLE_GEN
//#define DEBUG_HEADER

#define GET_INT32(buffer, offset) (buffer[offset] | buffer[offset+1]<<8 | buffer[offset+2]<<16 | buffer[offset+3]<<24)
#define GET_INT16(buffer, offset) (buffer[offset] | buffer[offset+1]<<8)

#define __SETO__(x, y) (int)(x-y)<0

static int  build_decompression_table(const unsigned char *source, short int *table)
{
    
	signed int v2; // ecx@1
	signed int v3; // eax@4
	signed int v4; // ecx@4
	signed int v5; // eax@5
	signed int v6; // eax@8
	int v7; // ecx@8
	signed int v8; // eax@10
	int v9; // edi@10
	signed int v10; // eax@11
	unsigned char v11; // ecx@11
	//char *v12; // esi@11
	unsigned char v13; // ecx@12
	signed int v14; // eax@14
	signed int v15; // ecx@14
	signed int i; // edx@15
	int j; // eax@16
	int k; // edx@18
	unsigned int v19; // eax@21
	short int v20; // si@21
	unsigned char v22; // eax@2
	unsigned char v23; // of@17
	char v24; // zf@17
	char v25; // sf@17
	short int v26; // si@26
	int counts[16]; // [sp+40h] [bp-44h]@1
	unsigned int total_counts; // [sp+80h] [bp-4h]@10
	int counts2[16]; // [sp+0h] [bp-84h]@16
   	
	// 1bc52
	memset(counts, 0, sizeof(counts));
	// 1bc62
	v2 = 256;
	// 1bc67
	do
	{
		// 1bc67
		v22 = source[v2 - 1];
		// 1bc6f
        v2--;
		// 1bc72
		counts[v22 & 0xF]++;
		// 1bc7c
		counts[v22 >> 4]++;
		// 1bc84
	} while ( v2 );

#ifdef DEBUG_TABLE_GEN    
	printf("\n\n1: HEX digit counts:\n");
    {
        int debugCounter;
        for(debugCounter=0; debugCounter<16; debugCounter++) {
            printf("\t0x%x = 0x%.8X\n", debugCounter, counts[debugCounter]);
        }
    }
#endif
    
	// 1bc90
	if ( counts[0] >= 511 )
	{
        return 0; // ERROR
    }
    
	
	// Save off a copy of the counts
	// 1bc9f
    memcpy(counts2, counts, sizeof(counts2));
	
	v3 = 0;
	v4 = 15;
	while ( 1 )
	{
		v5 = counts[v4] + v3;
		if ( v5 & 1 ) {
            break;
        }
			
		v3 = v5 >> 1;
		--v4;
		if ( !v4 )
		{
			if ( v3 != 1 ) {
				return 0;
            }
			
            v7 = 0;
			v6 = 1;
			do
			{
				counts[v6] += v7;
				v7 = counts[v6++];
			}
			while ( v6 < 16 );
            
#ifdef DEBUG_TABLE_GEN  
            printf("\n\n2: HEX digit counts:\n");
            {
                int debugCounter;
                for(debugCounter=0; debugCounter<16; debugCounter++) {
                    printf("\t0x%x = 0x%.8X\n", debugCounter, counts[debugCounter]);
                }
            }
#endif  
			v9 = counts[15];
			total_counts = counts[15];
			v8 = 8192;
			
            do
			{
				v10 = v8 - 16;
				v11 = (unsigned char)(source[v10 >> 5]) >> 4;
                
				if ( v11 )
				{
					--counts[v11];
					table[counts[v11]] = v11 |  v10;
					v9 = total_counts;
				}
				v8 = v10 - 16;
				v13 = source[v8 >> 5] & 0xF;
				if ( source[v8 >> 5] & 0xF )
				{
					--counts[v13];
					table[counts[v13]] = v13 | v8;
				}
			}
			while ( v8 );
          
                     
			v15 = 2048;
			v14 = 2048;
			int xsource = 15;
			do
			{
				for ( i = v15; v14 > i; table[v15] = v14 | 0x8000 )
				{
					v14 -= 2;
					--v15;
				}
				for ( j = counts2[xsource] - 1; j >= 0; table[v15] = v26 )
				{
					v26 = table[v9-- - 1];
					--v15;
					--j;
				}
				v14 = i;
				v23 = __SETO__(xsource - 1, 10);
				v24 = xsource == 11;
				v25 = (signed int)xsource-- < 11;
			}
			while ( !((unsigned char)(v25 ^ v23) | v24) );
            
            
			for ( k = 1024; v14 > v15; table[k] = v14 | 0x8000 )
			{
				v14 -= 2;
				--k;
			}

			while ( v9 > 0)
			{
#ifdef DEBUG_TABLE_GEN				
 printf("v9 = %d, k = %d\n", v9, k);
#endif				
				v20 = table[v9-- - 1];
				v19 = k - (1024 >> (v20 & 0xF));
				do
				{
					--k;
					table[k] = v20;
#ifdef DEBUG_TABLE_GEN
					printf("k=%p v19=%p\n", k, v19);
#endif					
				} while (k != v19 );
				
			}
			
#ifdef DEBUG_TABLE_GEN    
            printf("\n\n3: HEX digit table:\n");
            {
                int debugCounter;
                for(debugCounter=0; debugCounter<0x400; debugCounter++) {
                    printf("\t0x%x = 0x%.4X\n", debugCounter, table[debugCounter] & 0xffff);
                }
            }
#endif              
            
			return 1;
		}
	}
	return 0;
}
 
/* 
static char build_decompression_table(const unsigned char* source, unsigned short int* table) {
	int counts[16];  
	int counts2[16]; // ***Unused***
	int i;	
	
	// ---------------------------------------------------------
    // - Build Small Table-1
    // ---------------------------------------------------------
    
    memset(&counts, 0, sizeof(counts));
	
#pragma mark initial counts table	
	for(i=256; i>0; i--) {
        printf("index=%d\n", i-1);
        unsigned int a = source[i-1];
        counts[a & 0x0F]++;
		counts[a >> 4]++;
    }
	
#ifdef DEBUG
	printf("\n\n1: HEX digit counts:\n");
	for(i=0; i<16; i++) {
		printf("\t0x%x = 0x%.8X\n", i, counts[i]);
	}
#endif
    
	// offset: 1BC90
	if(counts[0]>=0x1ff) { // 511
        printf("count[0] should be < 0 but was 0x%x\n", counts[0]);
        return HU_BAD_COMPRESSION_TABLE;
    }
    
    // make a copy of the counts - offset: 1BCAD
    memcpy(counts2, counts, sizeof(counts2));
    
    // sanity check for counts - offset: 1BCAF
#pragma mark mutes counts table
    int counts_check = 0;
    for(i=15; i>0; i--) {
        // 1BCB6
        counts_check += counts[i];
        if(counts_check & 1) {
            printf("ERROR: invalid counts check @%d=%d\n", i, counts[i]);
            return HU_BAD_COMPRESSION_TABLE;
        }
        
        counts_check = counts_check >> 1;
    }
    
    // offset: 1BCC9
    if(counts_check!=1) {
        printf("ERROR: invalid counts_check \n",counts_check);
        return HU_BAD_COMPRESSION_TABLE;
    }
    
    
    // 1BCD2
    int temp = 0;
	//    int temp2;
    
    for(i=1; i<16; i++) {
        counts[i] += temp;
        temp = counts[i];
    }
    
#ifdef DEBUG
	printf("\n\n2: HEX digit counts:\n");
	for(i=0; i<16; i++) {
		printf("\t0x%x = 0x%.8X\n", i, counts[i]);
	}
#endif
    
    // offset: 1BCE9
    int total_counts = counts[15];
    
#ifdef DEBUG
    printf("total_counts = 0x%X\n", total_counts);
#endif
    
    // This is still a work in progress
    // offset: 1BCEF
    int counter = 0x2000; // 8192
    do {
        // offset: 1BCF7
        counter -= 0x10;
        int scratch = counter >> 5;
        unsigned int high = source[scratch];
        high = high >> 4;
        //unsigned int high = (unsigned char)source[counter >> 5] >> 4;
        
        // offset: 1BDC2
        if(high != 0) {
            //printf("\t[0X%.4X] high=0x%.4X\n", counter, high);
            counts[high]--;
            // variables for debugging 
			//            int val = counts[high];
            int scratch2 = counter | high;
            scratch2 = scratch2 & 0xFFFF;
			
            // 1BDCF
            // Write Output
            table[counts[high]] = (counter | high) & 0x0ffff;
            
        }
        
        // offset: 1BD0C
        counter -= 0x10;
        scratch = counter >> 5;
        int low = source[scratch];
        low = low & 0x0F;
        //int low = source[counter >> 5] & 0x0F;
        
        if(low != 0) {
            //printf("\t[0x%.4X] low=0x%.4X\n", counter, low);
            // offset: 1BC31
            counts[low]--;
            table[counts[low]] = (counter | low) & 0x0ffff;
        }
    } while (counter) ;
	
    
#ifdef DEBUG
	printf("\n\n3.1: HEX digit counts:\n");
	for(i=0; i<16; i++) {
		printf("\t0x%x = 0x%.8X\n", i, counts[i]);
	}
#endif
    
#ifdef XDEBUG
	printf("\n\n3: HEX digit table:\n");
	for(i=0; i<0x10000/2; i++) {
		printf("\t[%d] 0x%.4X\n", i, table[i]);
	}
#endif
    
	return 0;
}
*/

/* -- Here is where the magic happens -- */

#define WORD_LENGTH (8*sizeof(value)) 
static inline int rol(unsigned int value, int places) 
{ 
	return (value<<places)|(value>>(WORD_LENGTH-places)); 
} 
							
static inline int ror(unsigned int value, int places) 
{ 
	return (value>>places)|(value<<(WORD_LENGTH-places)); 
} 
#undef WORD_LENGTH 

static int decompress_hu01_block(const unsigned char* source_ptr, const unsigned char* end_of_source, const short int* table, unsigned char* destination_ptr, unsigned char* end_of_destination) {
	int i;
	
#ifdef DEBUG_DECOMPRESSION	
	unsigned char* expected_position = expected_data;
#endif
	
	const unsigned char* source = source_ptr;
	unsigned char* destination = destination_ptr;
	
	// 1BA35
	int esi = (GET_INT16(source, 0)<<16) | GET_INT16(source, 2);
	signed char ch,cl;
	int eax, edx;
	int saved_esi;
	unsigned char* esi_ptr;
	unsigned char* edi_ptr;
	
	source += 4; // 1BA3F
	
	ch = 0x10; // 1BA42
	
	// 1BA44
	if(source<end_of_source && destination<end_of_destination) {
		
	L_1BA56:
		edx = esi; // 1BA56
		cl = 0x0f;
		edx = (unsigned int)edx >> 0x16;
		eax = table[edx & 0x3ff];	// 1BA5D
		
		// 1BA62
		if(eax<0) {
			goto L_1BB55;
		}
		// 1BA6A
		cl = cl & (eax & 0xff);
		eax = (unsigned int)eax >> 4; 
		esi = (unsigned int)esi << cl;
		ch = ch - cl;
		if(ch<0) {
//			printf("\n THERE 3: %p %p %p, %p\n", (void*)eax, (void*)esi, (void*)ch, (void*)cl);
			goto L_1BAFA;
		}
		// 1BA79
		eax = eax - 0x100;
		if(eax<0) {
			goto L_1BB2B;
		}
//		printf("\nHERE X: %d %p\n", ch, (void*)eax);
		
	L_1BA84:
		// XXX
		cl = eax & 0xff;
		edx = esi;
		cl = (unsigned char)cl >> 4;
		edx = edx | 1;
		esi = (unsigned int)esi << cl;
		ch = ch - cl;
		// 1BA92
		edx = ror(edx, 1);
		cl = cl ^ 0x1f;
		eax = eax & 0x0f;
		edx = (unsigned int)edx >> cl;
		// 1BA9C
		saved_esi = esi;
//		printf("*edx = %i  eax = %i*", edx, eax);
		edx = -edx;
		if(eax>5) {
			goto L_1B9E1;
		}
		// 1BAA8
		esi_ptr = destination + edx;
		if(edx<=-3) {
			goto L_36670;
		}
		
		if(esi_ptr < destination_ptr) {
			printf("\nERROR - 1 - esi_ptr < destination_ptr\n");
			goto L_1B966;
		}
		
		// copy 8 bytes interleaved
		destination[0] = esi_ptr[0];
		destination[1] = esi_ptr[1];
		destination[2] = esi_ptr[2];
		destination[3] = esi_ptr[3];
		destination[4] = esi_ptr[4];
		destination[5] = esi_ptr[5];
		destination[6] = esi_ptr[6];
		destination[7] = esi_ptr[7];
#ifdef DEBUG
		for(i=0; i<eax+3; i++) {
			printf("%c", *(destination+i));				   
		}
#endif
		
		esi = saved_esi;
		destination = destination + eax + 3;
		
		// 1BAC8
		if(ch>=0) {
//			printf("\nHERE 1: %d %p\n", ch, (void*)eax);
/*			
			if(ch==1 && eax==1) {
				printf("\nSTOP\n");
				return 0;
			}
*/			
			goto L_1BA56;
		}
		
	L_1BAD0:
		if(source>=end_of_source) {
			goto L_1BE54;
		}
		if(destination>=end_of_destination) {
			goto L_1BE54;
		}
		cl = ch;
		edx = GET_INT16(source, 0);
		cl = -cl;
		source += 2;
		edx = (unsigned int)edx << cl;
		ch = ch + 0x10;
		esi = esi + edx;
		goto L_1BA56;
		
	L_36670:
		if(esi_ptr<destination_ptr) {
			printf("\nERROR - 2 - esi_ptr<destination_ptr - %i\n", edx);
			goto L_1B966;
		}
		
		// copy 8 bytes
//		memcpy(destination, esi_ptr, 8);
		destination[0] = esi_ptr[0];
		destination[1] = esi_ptr[1];
		destination[2] = esi_ptr[2];
		destination[3] = esi_ptr[3];
		destination[4] = esi_ptr[4];
		destination[5] = esi_ptr[5];
		destination[6] = esi_ptr[6];
		destination[7] = esi_ptr[7];
		
#ifdef DEBUG
//		printf("%c[0;32m", 0x1B);
		for(i=0; i<eax+3; i++) {
			printf("%c", *(destination+i));				   
		}
//		printf("%c[0;30m", 0x1B);
#endif
		
		esi = saved_esi;
		destination = destination + eax + 3;
		if(ch>=0) {
			goto L_1BA56;
		}
		
		goto L_1BAD0;
		
	L_1BAFA:
		if(source>=end_of_source || destination>=end_of_destination) {
			goto L_1BDDB;
		}
		// 1BB0C
		cl = ch;
		edx = GET_INT16(source, 0);
		cl = -cl;
		// 1BB15
		source = source + 2;
		edx = (unsigned int)edx << cl;
	L_1BB1A:
		ch = ch + 0x10;
		esi = esi + edx;
		eax = eax - 0x100;
		if(eax<0) {
			goto L_1BB2B;
		}
//		printf("\nHERE 2: %d %p\n", ch, (void*)eax);
		
		goto L_1BA84;
		
	L_1BB2B:
		edx = esi;
		
#ifdef DEBUG_DECOMPRESSION
		if(eax & 0xff!=*expected_position) {
			printf("decompressed data expected %d got %d\n", *expected_position, eax & 0xff);
			return 0;
		}
		
		expected_position++;
#endif
		
#ifdef DEBUG
//		printf("decompressed: %d - %c\n", eax & 0xff, eax & 0xff);
		printf("%c", eax & 0xff);
#endif		
		*destination = eax & 0xff;	// decompressed byte written here
		edx = (unsigned int)edx >> 0x16;
		destination++;	// 1BB32
		
		eax = table[edx & 0x3ff];	// 1BB33
		cl = 0x0f;
		
		// 1BB3A
		if(eax<0) {
			goto L_1BB55;
		}
		
		// 1BB3E
		cl = cl & (eax & 0xff);
		eax = (unsigned int)eax >> 4;
		esi = (unsigned int)esi << cl;		
		// 1BB45
		ch = ch - cl;
		if(ch<0) {
//			printf("\n THERE 1: %p %p %p, %p\n", (void*)eax, (void*)esi, (void*)ch, (void*)cl);
			goto L_1BAFA;
		}
		// 1BB49
		eax = eax - 0x100;
		if(eax<0) {
			goto L_1BB2B;
		}
//		printf("\nHERE 3: %d %p\n", ch, (void*)eax);
		goto L_1BA84;
		
	L_1B966:
		// decompression failed
		printf("\n ** 1B966 **\n");
		return 0;
			
	L_1B9C5:
		eax = (unsigned char)*source;
		source++;
		// 1B9CC
		eax += 0x0f;
//		printf("\n#### %p ####\n", (void*)eax);
		if (eax!=0x10e) {
			goto L_1B9E6;
		} else {
			eax = GET_INT16(source, 0);
			source += 2;
			
			if (eax>0x10e) {
				goto L_1B9E6;
			}
			
			goto L_1B966;
		}
		
	L_1B9E1:
		if(eax==0x0f) {
			goto L_1B9C5;
		}
		
	L_1B9E6:
		// let's get ready to copy some bytes
		esi_ptr = destination + edx;
		eax = eax + 3;
		edi_ptr = destination + eax;
		if(esi_ptr<source_ptr) {
			printf("esi_ptr<source_ptr\n");
			goto L_1B966;
		}
		
		if(edi_ptr>=end_of_destination) {
			goto L_1BE42;
		}
		
//		memcpy(destination, esi_ptr, eax);
		for (i=0; i<eax; i++) {
			destination[i] = esi_ptr[i];
		}
#ifdef DEBUG
		for(i=0; i<eax; i++) {
			printf("%c", *(destination+i));				   
		}
#endif
		destination = edi_ptr;
		
		esi = saved_esi;
		if(destination>=end_of_destination) {
			goto L_1BE50;
		}
		if(ch<0) {
			goto L_1BAD0;
		}
		goto L_1BA56;
		
	L_1BB55:
		// we got a negative decompression index
		
		esi = (unsigned int)esi << 10;
		do {
//			printf("\n--- %d %p ---", eax, (void*)esi);
//			if ((unsigned int)esi&0x80000000) {
//				eax++;
//			}
			eax = eax + ((unsigned int)esi>>31);
			esi = esi + esi;
			// 1BB5D
			eax = table[0x8000+eax];
		} while (eax<0);
		
		// 1BB69
		cl = cl & (eax & 0xff);
		eax = (unsigned int)eax >> 4;
		ch = ch - cl;
		if(ch<0) {
//			printf("\n THERE 2: %p %p %d %d\n", (void*)eax, (void*)esi, ch, cl);
			goto L_1BAFA;
		}
		// 1BB72
		eax = eax - 0x100;
		if(eax>=0) {
//			printf("\nHERE 4: %d %p\n", ch, (void*)eax);
			goto L_1BA84;
		}
		
		goto L_1BB2B;
		   
	L_1BDDB:
		;
//		printf("\n***1BDDB***\n");
	}
	
L_1BE42:
//	printf("\n***1BE42***\n");
	
L_1BE50:
//	printf("\n***1BE50***\n");
	
L_1BE54:
//	printf("\n***1BE54***\n");
	
	// 1BE70
		
	return 1;
}

#pragma mark decompress 
int decompress_hu01_data(const unsigned char* hu01_buffer, int hu01_buffer_size, unsigned char* buffer) {
	int size = 0;
	int to_decompress;
	int header_size;
	const unsigned char* pos;
	short int table[0x8000];
	unsigned char* dest;
	
	if(hu01_buffer[0]=='H' && hu01_buffer[1]=='U') {
		if(hu01_buffer[2]=='0' && hu01_buffer[3]=='1') {
			size = GET_INT32(hu01_buffer, HU_SIZE_OFFSET);
			
			if(buffer==NULL) {
				// do nothing
			} else {
				dest = buffer;
				to_decompress = size;
				header_size = GET_INT32(hu01_buffer, 4);

				if(header_size>=28) {			
#ifdef DEBUG_HEADER
					printf("header_size = %d\n", header_size);
#endif
					hu01_buffer_size -= header_size;
					pos = &hu01_buffer[header_size];
					
					while (hu01_buffer_size>0 && to_decompress>0) {
						int magic = GET_INT32(pos, 0);						
#ifdef DEBUG_HEADER
						printf("block magic header = %p, expecting %p\n", (void*)magic, (void*)0x48424353);
#endif							   
						// SCBH
						if(magic!=0x48424353) {
							return HU_BAD_BLOCK_HEADER;
						}
					
						int block_header_size = GET_INT32(pos, 4);
#ifdef DEBUG_HEADER
						printf("block header size = %d\n", block_header_size);
#endif
						
						int block_decompressed_size = GET_INT32(pos, 8);
#ifdef DEBUG_HEADER
						printf("block decompressed size = %d\n", block_decompressed_size);
#endif
						
#ifdef DEBUG_HEADER
						int block_crc = GET_INT32(pos, 12);
						printf("block CRC = 0x%.8X\n", block_crc);
#endif
						
						int block_size = GET_INT32(pos, 16);
#ifdef DEBUG_HEADER
						printf("block size = %d\n", block_size);
#endif
						
						pos += block_header_size;
						
                        int result = build_decompression_table(pos, &table[0]);
						hu01_buffer_size -= block_header_size;
                        
						if(!result) {
                            printf("ERROR: %d\n", result);
							return HU_BAD_COMPRESSION_TABLE;
						} else {
							const unsigned char* block_end_pos = pos+block_size;
							unsigned char* block_end_dest = dest+block_decompressed_size;
#ifdef DEBUG_HEADER
                            printf("TABLE SUCCESS:\n");
#endif							
							pos += 256;
							
							if(decompress_hu01_block(pos, block_end_pos, &table[0], dest, block_end_dest)==0) {
								return HU_BLOCK_DECOMPRESS_FAILED;
							}
							hu01_buffer_size -= block_size;
							to_decompress -= block_decompressed_size;
							
							pos = block_end_pos;
							dest = block_end_dest;
						}
												  
//						break;  // just for now
					}
				} else {
					return HU_BAD_HEADER_SIZE;
				}
				

			}
			
			return size;
		} else {
			return HU_BAD_VERSION;
		}
	} else {
		return HU_BAD_HEADER;
	}
}
