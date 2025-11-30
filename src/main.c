#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s [FILE] [OUT]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	d64image* image = d64_read(argv[1]);
	if (image == NULL) return 1;

	for (int i = 0; i < image->num_file_entry; i++) {
		d64file* file = d64_read_file_index(image, i);
		d64_print_file(image, file);

		d64file_entry* entry = file->entry;

		char chars[file->size + 1]; // +1 for \0
		d64_file_to_ascii(file, chars);

		char path[strlen(entry->name) + strlen(argv[2]) + 1];
		sprintf(path, "%s%s", argv[2], entry->name);

		FILE* f = fopen(path, "wb");
		if (f == NULL) {
			perror("fopen");
			continue;
		}
		fwrite(chars, 1, file->size, f);
		fclose(f);

		d64_file_free(file);
	}

	d64_image_free(image);
	return 0;
}
