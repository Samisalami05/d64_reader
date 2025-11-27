#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s [FILE]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	d64image* image = parse_d64(argv[1]);
	if (image == NULL) return 1;

	for (int i = 0; i < image->num_file_entry; i++) {
		d64file_entry entry = image->file_entries[i];
		d64print_file_entry(entry);
	}

	d64image_free(image);
	return 0;
}
