#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

typedef struct d64bam_track {
	uint8_t free_count;
	uint8_t bitmap[3];
} d64bam_track;

typedef struct d64file {
	char* name;
	uint8_t type;
	int blocks;
	int size;
	int start_track;
	int start_sector;
	uint8_t* data;
} d64file;

typedef struct d64image {
	int num_sectors;
	uint8_t** sectors;
	d64bam_track bam_tracks[36];
} d64image;

void petscii_to_ascii(char *out, uint8_t *in);
void petscii_to_ascii_file(d64file* file, char *out);

int d64_track_sector(int track, int sector);
d64file* d64_read_file(d64image* image, int track, int sector);
d64image* parse_d64(char* filePath);
void d64image_free(d64image* image);

#endif
