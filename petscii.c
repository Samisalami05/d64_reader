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
		default: 
			if ((in >= 32 && in <= 90)) {
				////printf("converting %d to %c\n", in, petscii_chars[in - 32]);
				return petscii_chars[in - 32];
			}
			else if (in >= 193 && in <= 218) {
				return petscii_chars[in - 134];
			}

	}
	return '?';
}

void petscii_to_ascii_str(uint8_t* in, int len, char* out) {
    for (int i = 0; i < len - 1; i++) {
         out[i] = petscii_to_ascii(in[i]);
	}
    out[len - 1] = '\0';

	// Remove unnecesary spaces
	for (int i = len - 2; i >= 0; i--) {
		if (out[i] != ' ') {
			out[i + 1] = '\0';
			break;
		}
	}
}
