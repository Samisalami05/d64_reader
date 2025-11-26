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

	for (int x = 0; x < image->num_sectors; x++) {
		for (int i = 0; i < 256; i++) {
			if (image->sectors[x][i] != 0) {
				//printf("%c ", image->sectors[x][i]);
			}
		}
	}

	d64file* file = d64_read_file(image, 17, 19);

	d64image_free(image);
	return 0;
}
