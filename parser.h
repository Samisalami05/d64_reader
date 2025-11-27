#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

typedef struct d64bam_track {
	uint8_t free_count;
	uint8_t bitmap[3];
} d64bam_track;

typedef struct d64file_entry {
	char* name;
	uint8_t type;
	int blocks;
	int start_track;
	int start_sector;
} d64file_entry;

typedef struct d64file {
	int entry;
	int size;
	uint8_t* data;
} d64file;

typedef struct d64image {
	int num_sectors;
	uint8_t** sectors;
	d64bam_track bam_tracks[36];
	int num_file_entry;
	d64file_entry* file_entries;
} d64image;

void d64_file_to_ascii(d64file* file, char *out);

int d64_track_sector(int track, int sector);
d64file* d64_read_file(d64image* image, int entry);
d64file* d64_read_file_ts(d64image* image, int track, int sector);
d64image* d64_read(char* filePath);
void d64_print_file_entry(d64file_entry entry);
void d64_print_file(d64image* image, d64file* file);
void d64_image_free(d64image* image);
void d64_file_free(d64file* file);

#endif
