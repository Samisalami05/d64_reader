# D64 disk reader
A commodore 1541 disk reader made in c. The file extension for 
these types of disks are `.d64` which is what this reader can
interpret.

## Features
* Reading information from the disk
* Exporting data of files
* Conversion from petscii to ascii (Commodores character table)

## Usage

### Structures

#### d64image

The main structure of the reader is the `d64image` which
represents a image of a `.d64` file. This structure stores
information and data from the disk.

```c
struct d64image {
	char diskname[17];
	int size;
	d64bam_track bam_tracks[36];
	int num_sectors;
	int num_file_entry;
	uint8_t** sectors;
	d64file_entry* file_entries;
}
```

#### d64file_entry

Each file has a correspondind file entry `d64file_entry` that
is stored in the `d64image` structure. This structure stores
information about the file that can then be used for gathering
the data of the file.

```c
typedef struct d64file_entry {
	char name[17];
	uint8_t type;
	int blocks;
	int start_track;
	int start_sector;
} d64file_entry;
```

#### d64file

A file is represented by the `d64file` structure that both stores
a pointer to its file entry but also the data and size of the
file.

```c
typedef struct d64file {
	d64file_entry* entry;
	int size;
	uint8_t* data;
} d64file;
```

### Reading

```c
d64image* d64_read(char* filePath);
```
Reads the given `.d64` file and returns an image. This image can
then be used to get data from the disk.

**Note:** The image needs to be freed after use.

```c
d64file* d64_read_file(d64image* image, d64file_entry* entry);
```

Returns a file of the given file entry inside the image.

**Note:** The file needs to be freed after use.

### Helpers


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
