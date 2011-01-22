#include <stdio.h>
#include <stdlib.h>
#include "hu01_decompressor.h"

int main(int argc, char *argv[]) {
	FILE* infile;
	FILE* outfile;
	int input_size;
	int output_size;
	int decompressed_size;
	unsigned char* compressed_data;
	unsigned char* decompressed_data;
	
	if(argc==3) {
		infile = fopen(argv[1], "rb");
		if(infile) {
			
			fseek(infile, 0, SEEK_END);
			input_size = ftell(infile);
			fseek(infile, 0, SEEK_SET);
			
			compressed_data = malloc(input_size);
			fread(compressed_data, input_size, 1, infile);
			fclose(infile);
			
			output_size = decompress_hu01_data(compressed_data, input_size, NULL);
			
			if(output_size>0) {
				decompressed_data = malloc(output_size);
				
				decompressed_size = decompress_hu01_data(compressed_data, input_size, decompressed_data);
				
				if(output_size == decompressed_size) {
					outfile = fopen(argv[2], "wb");
					if(outfile) {
						fwrite(decompressed_data, decompressed_size, 1, outfile);
						fclose(outfile);
						
						printf("SUCCESS: decompressed %d bytes\n", decompressed_size);
						return 0;
					} else {
						printf("ERROR: could not open output file '%s'\n", argv[2]);
						return 1;
					}
				} else {
					printf("ERROR: decompression failed.  Expected %d decompressed bytes, got %d\n", output_size, decompressed_size);
					return 1;
				}
			} else {
				printf("ERROR: decompression failed - %d\n", output_size);
				return 1;
			}
		} else {
			printf("ERROR: could not open input file '%s'\n", argv[1]);
			return 1;
		}
	} else {
		printf("USAGE: %s infile outfile\n", argv[0]);
		return 1;
	}
	
	return 0;
}
