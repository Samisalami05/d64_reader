#ifndef PETSCII_H
#define PETSCII_H

#include <stdint.h>

char petscii_to_ascii(uint8_t in);

void petscii_to_ascii_str(uint8_t* in, int size, char* out);

#endif
