#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "petscii.h"

#define SECTOR_SIZE 256

static const int sectors_per_track[36] = {
    0,  // track 0 (unused)
    21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21, // 1–17
    19,19,19,19,19,19,19,								// 18–24
    18,18,18,18,18,18,									// 25–30
    17,17,17,17,17										// 31–35
};

void d64_file_to_ascii(d64file* file, char *out) {
    for (int i = 0; i < file->size; i++) {
        uint8_t b = file->data[i];

		out[i] = petscii_to_ascii(b);
	}
    out[file->size] = '\0';
}

static int bam_is_free(d64bam_track* bam_track, int sector) {
	if (sector < 0 || sector >= 21) return 0;

	int byte = sector / 8;
    int bit  = sector % 8;

    return (bam_track->bitmap[byte] & (1 << bit)) != 0;
}

static void parse_bam(d64image* image) {
	int index = d64_track_sector(18, 0);
	uint8_t* sector = image->sectors[index];

	uint8_t* p = sector + 4;

	// Loop through every track
	for (int i = 1; i < 36; i++) {
		image->bam_tracks[i].free_count = p[0];
		image->bam_tracks[i].bitmap[0] = p[1];
		image->bam_tracks[i].bitmap[1] = p[2];
		image->bam_tracks[i].bitmap[2] = p[3];
		p += 4;
	}
}

int d64_track_sector(int track, int sector) {
	if (track < 1 || track > 35) return -1;
	if (sector < 0 || sector >= sectors_per_track[track]) return -1;

	int index = 0;
	for (int i = 0; i < track; i++) {
		index += sectors_per_track[i];
	}

	return index + sector;
}

static char* d64_type_str(uint8_t type) {
    switch ((type >> 5) & 0x07) {
        case 1: return "SEQ";
        case 2: return "PRG";
        case 3: return "USR";
        case 4: return "REL";
		default: return "UNKNOWN";
    }
}

d64file* d64_read_file_ts(d64image* image, int track, int sector) {
	int t = track;
	int s = sector;

	int capacity = 4096;

	d64file* file = malloc(sizeof(d64file));
	file->size = 0;
	file->data = malloc(capacity);
	file->entry = -1;

	while (t != 0) {
		int index = d64_track_sector(t, s);

		if (index < 0 || index >= image->num_sectors) break;

		uint8_t* sec = image->sectors[index];

		t = sec[0];
		s = sec[1];

		int bytes_to_copy = 254;
		if (file->size + bytes_to_copy > capacity) {
			capacity *= 2;
			file->data = realloc(file->data, capacity);
		}

		for (int i = 0; i < bytes_to_copy; i++) {
			file->data[file->size + i] = sec[i + 2];
		}

		file->size += bytes_to_copy;

	}
	return file;
}

d64file* d64_read_file(d64image* image, int entry) {
	d64file_entry fentry = image->file_entries[entry];
	d64file* file = d64_read_file_ts(image, fentry.start_track, fentry.start_sector);
	file->entry = entry;
	return file;
}

static d64image* d64_parse_image(FILE* f, int fileSize) {
	int num_sectors = fileSize / SECTOR_SIZE;
	printf("Sector count: %d\n", num_sectors);

	d64image* image = malloc(sizeof(d64image));
	image->num_sectors = num_sectors;
	image->sectors = malloc(sizeof(uint8_t*) * num_sectors);
	image->file_entries = NULL;
	image->num_file_entry = 0;

	for (int i = 0; i < num_sectors; i++) {
		image->sectors[i] = malloc(SECTOR_SIZE);
		fread(image->sectors[i], 1, SECTOR_SIZE, f);
	}
	return image;
}

static int parse_file_entry(d64image* image, uint8_t* edata) {
	uint8_t type = edata[0];
	if ((type & 0x80) == 0) return -1;

	d64file_entry entry;
	entry.type = type;
	entry.start_track = edata[1];
	entry.start_sector = edata[2];
	entry.blocks = edata[30] + (edata[31] << 8);
	entry.name = malloc(sizeof(char) * 17);
	petscii_to_ascii_str(edata + 3, 17, entry.name);

	image->num_file_entry++;
	image->file_entries = realloc(image->file_entries, sizeof(d64file_entry) * image->num_file_entry);
	image->file_entries[image->num_file_entry - 1] = entry;

	return image->num_file_entry - 1;
}


static void d64_parse_file_entries(d64image* image) {
	int track = 18;
	int sector = 1;
	
	while (track != 0) {
		int index = d64_track_sector(track, sector);
		uint8_t* sec = image->sectors[index];

		track = sec[0];
		sector = sec[1];

		for (int i = 0; i < 8; i++) {
			uint8_t* entry = sec + 2 + (i * 32);

			uint8_t type = entry[0];
			if ((type & 0x80) == 0) continue;   // bit 7 must be set
			
			int fentry = parse_file_entry(image, entry);
		}
	}
}

d64image* d64_read(char* filePath) {
	FILE* f = fopen(filePath, "rb");

	if (f == NULL) {
		perror("fopen");
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	int fileSize = ftell(f);
	
	if (fileSize % SECTOR_SIZE != 0) {
		fprintf(stderr, "Not a valid d64 file: file is not divided into 256 byte sectors\n");
		fclose(f);
		return NULL;
	}

	fseek(f, 0, SEEK_SET);

	d64image* image = d64_parse_image(f, fileSize);
	parse_bam(image);
	d64_parse_file_entries(image);

	fclose(f);
	return image;
}

void d64_print_file_entry(d64file_entry entry) {
	printf("File entry: %s\n", entry.name);
	printf("Blocks: %d\n", entry.blocks);
	printf("Start track: %d\n", entry.start_track);
	printf("Start sector: %d\n", entry.start_sector);
	printf("Type: %s\n\n", d64_type_str(entry.type));
}

void d64_print_file(d64image* image, d64file* file) {
	d64file_entry entry = image->file_entries[file->entry];
	printf("File: %s\n", entry.name);
	printf("Blocks: %d\n", entry.blocks);
	printf("Size: %d\n", file->size);
	printf("Start track: %d\n", entry.start_track);
	printf("Start sector: %d\n", entry.start_sector);
	printf("Type: %s\n\n", d64_type_str(entry.type));
}

void d64_image_free(d64image* image) {
	for (int i = 0; i < image->num_sectors; i++) {
		free(image->sectors[i]);
	}

	for (int i = 0; i < image->num_file_entry; i++) {
		free(image->file_entries[i].name);
	}
	free(image->file_entries);
	free(image->sectors);
	free(image);
}

void d64_file_free(d64file* file) {
	free(file->data);
	free(file);
}
