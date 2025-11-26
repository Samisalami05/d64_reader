#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SECTOR_SIZE 256

static const int sectors_per_track[36] = {
    0,  // track 0 (unused)
    21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21, // 1–17
    19,19,19,19,19,19,19,								// 18–24
    18,18,18,18,18,18,									// 25–30
    17,17,17,17,17										// 31–35
};

// Has set size of 17
void petscii_to_ascii(char *out, uint8_t *in) {
    for (int i = 0; i < 16; i++) {
        if (in[i] == 0xA0) out[i] = ' ';
        else if (in[i] >= 32 && in[i] <= 126) out[i] = in[i];
        else out[i] = '?';
    }
    out[16] = '\0';

	// Remove unnecesary spaces
	for (int i = 15; i >= 0; i--) {
		if (out[i] != ' ') {
			out[i + 1] = '\0';
			break;
		}
	}
}

void petscii_to_ascii_file(d64file* file, char *out) {
    for (int i = 0; i < file->size; i++) {
        uint8_t b = file->data[i];

        if (b == 0x0D) out[i] = '\n';    // PETSCII CR → ASCII LF
        else if (b == 0xA0) out[i] = ' '; // PETSCII space
        else if (b >= 32 && b <= 126) out[i] = b; // printable ASCII
        else out[i] = '?';  // other unprintable bytes
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

d64file* d64_read_file(d64image* image, int track, int sector) {
	int t = track;
	int s = sector;

	int capacity = 4096;

	d64file* file = malloc(sizeof(d64file));
	file->start_track = t;
	file->start_sector = s;
	file->size = 0;
	file->data = malloc(capacity);

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
	

static void list_directory(d64image* image) {
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
			
			int start_track = entry[1];
			int start_sector = entry[2];

			char name[17];
			petscii_to_ascii(name, entry + 3);

			d64file* file = d64_read_file(image, start_track, start_sector);

			char out[file->size];
			petscii_to_ascii_file(file, out);

			FILE* f = fopen(name, "wb");
			fwrite(out, 1, file->size, f);
			fclose(f);
	
			char *ftype = d64_type_str(type);

			int blocks = entry[30] + (entry[31] << 8);
			int approx_bytes = blocks * 256;
			printf("blocks: %d\n", blocks);

			printf("%s - %s: start_sector = %d, start_track = %d\n", name, ftype, start_sector, start_track);

		}
	}
}

d64image* parse_d64(char* filePath) {
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

	int num_sectors = fileSize / SECTOR_SIZE;
	printf("Sector count: %d\n", num_sectors);

	d64image* image = malloc(sizeof(d64image));
	image->num_sectors = num_sectors;
	image->sectors = malloc(sizeof(uint8_t*) * num_sectors);

	for (int i = 0; i < num_sectors; i++) {
		image->sectors[i] = malloc(SECTOR_SIZE);
		fread(image->sectors[i], 1, SECTOR_SIZE, f);
	}

	parse_bam(image);

	list_directory(image);

	fclose(f);
	return image;
}

void d64image_free(d64image* image) {
	for (int i = 0; i < image->num_sectors; i++) {
		free(image->sectors[i]);
	}
	free(image->sectors);
	free(image);
}
