#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int main(void) {
	d64image* image = parse_d64("WorkDisk.d64");
	if (image == NULL) return 1;

	for (int x = 0; x < image->num_sectors; x++) {
		for (int i = 0; i < 256; i++) {
			if (image->sectors[x][i] != 0) {
				//printf("%c ", image->sectors[x][i]);
			}
		}
	}

	d64file* file = d64_read_file(image, 17, 19);

	char out[file->size];
	petscii_to_ascii_file(file, out);

	FILE* f = fopen("out.txt", "wb");
	//fwrite(out, 1, file->size, f);
	fclose(f);

	d64image_free(image);
	return 0;
}
