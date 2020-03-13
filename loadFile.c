// loadFile.c
// Michael Black, 2007
//
// Loads a file into the file system
// This should be compiled with gcc and run outside of the OS

#include <stdio.h>

void main(int argc, char* argv[]) {
  int i;

  if (argc < 2) {
    printf("Specify file name to load\n");
    return;
  }

  // open the source file
  FILE* loadFil;
  loadFil = fopen(argv[1], "r");
  if (loadFil == 0) {
    printf("File not found\n");
    return;
  }

  // open the floppy image
  FILE* floppy;
  floppy = fopen("system.img", "r+");
  if (floppy == 0) {
    printf("system.img not found\n");
    return;
  }

  // load the disk map
  char map[512];
  fseek(floppy, 512 * 256, SEEK_SET); // OFFSET 0x100
  for (i = 0; i < 512; i++) map[i] = fgetc(floppy);

  // load the directory
  char dir[1024];
  fseek(floppy, 512 * 257, SEEK_SET); // OFFSET 0x101 dan 0x 102
  for (i = 0; i < 512; i++) dir[i] = fgetc(floppy);

  // load the disk sectors
  char sec[512];
  fseek(floppy, 512 * 259, SEEK_SET); // OFFSET 0x103
  for (i = 0; i < 512; i++) sec[i] = fgetc(floppy);

  // find a free entry in the directory
  for (i = 0; i < 1024; i = i + 0x10)
    if (dir[i] == 0) {
      break;
    }
  if (i == 1024) {
    printf("Not enough room in directory\n");
    return;
  }
  int dirindex = i;

  // fill the name field with 00s first
  for (i = 0; i < 14; i++) dir[2 + dirindex + i] = 0;
  // copy the name over
  for (i = 0; i < 14; i++) {
    if (argv[1][i] == 0) {
      break;
    }
    dir[2 + dirindex + i] = argv[1][i];
  }

  dirindex = dirindex + 12;

  // locate file on root directory and search unoccupied entry
  for (i = 0; i < 32; i++) {
    if (sec[i*16] == 0) {
      break;
    }
  }
  if (i == 32) {
    printf("There are no available entry\n");
    return;
  }
  int entryindex = i;
  dir[dirindex] = 0xFF;
  dir[dirindex + 1] = entryindex;  

  // find free sectors and add them to the file
  int sectcount = 0;
  while (!feof(loadFil)) {
    if (sectcount == 20) {
      printf("Not enough space in directory entry for file\n");
      return;
    }

    // find a free map entry
    for (i = 0; i < 256; i++)
      if (map[i] == 0) break;
    if (i == 256) {
      printf("Not enough room for file\n");
      return;
    }

    // mark the map entry as taken
    map[i] = 0xFF;

    // mark the sector in the directory entry
    sec[entryindex*16+sectcount] = i;
    sectcount++;

    printf("Loaded %s to sector %d\n", argv[1], i);

    // move to the sector and write to it
    fseek(floppy, i * 512, SEEK_SET);
    for (i = 0; i < 512; i++) {
      if (feof(loadFil)) {
        fputc(0x0, floppy);
        break;
      } else {
        char c = fgetc(loadFil);
        fputc(c, floppy);
      }
    }
  }

  // write the map and directory back to the floppy image
  fseek(floppy, 512, SEEK_SET);
  for (i = 0; i < 512; i++) fputc(map[i], floppy);

  fseek(floppy, 512 * 2, SEEK_SET);
  for (i = 0; i < 512; i++) fputc(dir[i], floppy);

  fclose(floppy);
  fclose(loadFil);
}
