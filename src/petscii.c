#include "petscii.h"
#include <stdio.h>

static const char* petscii_chars = " !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

char petscii_to_ascii(uint8_t in) {
	switch (in) {
		case 0x0D: return '\n';
		case 0x5C: return '\\';
		case 0xDB: return '{';
		case 0xDD: return '}';
		case 0x5B: return '[';
		case 0x5D: return ']';
		case 0xa0: return ' ';
		case 0x00: return ' ';
		default: 
			if ((in >= 32 && in <= 90)) {
				////printf("converting %d to %c\n", in, petscii_chars[in - 32]);
				return petscii_chars[in - 32];
			}
			else if (in >= 193 && in <= 218) {
				return petscii_chars[in - 134];
			}

	}
	printf(" %d ", in);
	return '?';
}

void petscii_to_ascii_str(uint8_t* in, int size, char* out) {
    for (int i = 0; i < size; i++) {
         out[i] = petscii_to_ascii(in[i]);
	}
    out[size] = '\0';
}
