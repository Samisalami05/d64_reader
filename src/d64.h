#ifndef D64_H
#define D64_H

#include <stdint.h>

typedef struct d64bam_track {
	uint8_t free_count;
	uint8_t bitmap[3];
} d64bam_track;

typedef struct d64file_entry {
	char name[17];
	uint8_t type;
	int blocks;
	int start_track;
	int start_sector;
} d64file_entry;

typedef struct d64file {
	d64file_entry* entry;
	int size;
	uint8_t* data;
} d64file;

typedef struct d64image {
	char* filepath;
	char diskname[17];
	int size;
	d64bam_track bam_tracks[36];
	int num_sectors;
	int num_file_entry;
	uint8_t** sectors;
	d64file_entry* file_entries;
} d64image;

void d64_file_to_ascii(d64file* file, char *out);
int d64_ts_offset(int track, int sector);

d64file_entry* d64_find_file(d64image* image, const char* name);
d64file* d64_read_file_name(d64image* image, const char* name);
d64file* d64_read_file(d64image* image, d64file_entry* entry);
d64file* d64_read_file_index(d64image* image, int entry);
d64file* d64_read_file_ts(d64image* image, int track, int sector);

void d64_image_write(d64image* image, int entry, uint8_t* data, int size);

d64image* d64_read(char* filePath);

void d64_print_image(d64image* image);
void d64_print_file_entry(d64file_entry entry);
void d64_print_file(d64image* image, d64file* file);

void d64_image_free(d64image* image);
void d64_file_free(d64file* file);

#endif
