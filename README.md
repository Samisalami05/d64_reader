# D64 disk reader
A commodore 1541 disk reader made in c. The file extension for 
these types of disks are `.d64` which is what this reader can
interpret.

## Features
* Reading information from the disk
* Exporting data of files
* Conversion from petscii to ascii (Commodores character table)

## Usage

### Reading

#### d64_read()

```c
d64image* d64_read(char* filePath);
```
Reads the given `.d64` file and returns an image. This image
can then be used to get data from the disk.

#### d64_read_file()
```c
d64image* d64_read(char* filePath);
```

```c
void d64_file_to_ascii(d64file* file, char *out);
int d64_ts_offset(int track, int sector);

d64file_entry* d64_find_file(d64image* image, const char* name);
d64file* d64_read_file_name(d64image* image, const char* name);
d64file* d64_read_file(d64image* image, d64file_entry* entry);
d64file* d64_read_file_index(d64image* image, int entry);
d64file* d64_read_file_ts(d64image* image, int track, int sector);
d64image* d64_read(char* filePath);

void d64_print_image(d64image* image);
void d64_print_file_entry(d64file_entry entry);
void d64_print_file(d64image* image, d64file* file);

void d64_image_free(d64image* image);
void d64_file_free(d64file* file);
```
