#define MAX_BYTE 256
#define SECTOR_SIZE 512 
#define MAX_FILES 16 
#define MAX_FILENAME 12 
#define MAX_SECTORS 20 
#define DIR_ENTRY_LENGTH 32 
#define MAP_SECTOR 1 
#define DIR_SECTOR 2 
#define INSUFFICIENT_SECTORS 0 
#define INSUFFICIENT_DIR_ENTRIES -1 

/* Ini deklarasi fungsi */
// void handleInterrupt21(int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *filename, int *success);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void writeFile(char *buffer, char *filename, int *sectors);
void executeProgram(char *filename, int segment, int *success);
int div(int p,int q);
int mod(int p,int q);

int main () {


  // while(1);
  char buf[SECTOR_SIZE * MAX_SECTORS];
  int succ;
  makeInterrupt21();
  printString("Halo !!, Selamat Datang Di Operating System");

  executeProgram(&"milestone1", 0x2000, &succ);
  
    // interrupt(0x21, 0x4, &buf, "key.txt", &succ);
    // if(succ){
    //   interrupt(0x21, 0x0, &buf, 0, 0);
    // }else{
    //   interrupt(0x21, 0x6, "milestone1", 0x2000, &succ);
    // }
  

  while(1){

  
    // readString(&buf);
    // printString(&buf);
    // clear(&buf,SECTOR_SIZE * MAX_SECTORS);
  }
}


void handleInterrupt21 (int AX, int BX, int CX, int DX) {
   char AL, AH;
   AL = (char) (AX);
   AH = (char) (AX >> 8);
   switch (AL) {
      case 0x00:
         printString(BX);
         break;
      case 0x01:
         readString(BX);
         break;
      case 0x02:
         readSector(BX, CX);
         break;
      case 0x03:
         writeSector(BX, CX);
         break;
      case 0x04:
         readFile(BX, CX, DX, AH);
         break;
      case 0x05:
         writeFile(BX, CX, DX, AH);
         break;
      case 0x06:
         executeProgram(BX, CX, DX, AH);
         Break;
      default:
         printString("Invalid interrupt");
   }
}

void printString(char *string) {
  int i = 0;
  char c = string[i];
  while(c != '\0') {
    interrupt(0x10, 0xE*256 + c, 0, 0, 0);
    c = string[i++];
  }
  interrupt(0x10, 0xE*256 + '\n', 0, 0, 0);
  interrupt(0x10, 0xE*256 + '\r', 0, 0, 0);
}

void readString(char *string) {
  int i = 0;
  char c;
  do{
    c = interrupt(0x16, 0, 0, 0, 0);
    if(c == '\b') {
      interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
      interrupt(0x10, 0xE00 + '\0', 0, 0, 0);
      interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
      i--;
    } else if(c != '\r') {
      string[i] = c;
      interrupt(0x10, 0xE00 + c, 0, 0, 0);
      i++;
  }
} while(c != '\r');

  string[i] = '\0';
  interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
  interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
}

void readSector(char *buffer, int sector){
  interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

int div(int p,int q){
  int hasil;
  hasil = 0;
  while (hasil*q<=p){
    hasil ++;
  }
  return hasil-1;
}

int mod(int p, int q){
  while(p>=q){
    p = p-q;
  }
  return p;
}

void writeSector(char *buffer, int sector){
  interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector,18),2) * 0x100);
}

void readFile(char *buffer, char *filename, int *success){
  char dir[SECTOR_SIZE];
  int i, found, j;


  readSector(dir, DIR_SECTOR);
  for(i = 0; i < SECTOR_SIZE; i += DIR_ENTRY_LENGTH) {
    found = 1;
    for(j = 0; j < MAX_FILENAME; j++) {
      if(dir[i + j] == '\0' && filename[j] == '\0') {
        break;
      }
      if(dir[i + j] != filename[j]) {
        found = 0;
        break;
      }
    }

    if(found) {
      j = i + MAX_FILENAME;
      i = 0;
      while(i < MAX_SECTORS && dir[i + j] != 0) {
        readSector(buffer + i * SECTOR_SIZE, dir[j + i]);
        i++;
      }
      *success = 1;
      return; 
    }
  }
  *success = found;
}

void clear(char *buffer, int length){
  int i;
  for(i=0;i<length;i++){
    buffer[i] = 0x0;
  }
} 
//Fungsi untuk mengisi buffer dengan 0
void writeFile(char *buffer, char *filename, int *sectors){
  char map[SECTOR_SIZE];
  char dir[SECTOR_SIZE];
  char sectorBuffer[SECTOR_SIZE]; 
  int dirIndex;

  readSector(map, MAP_SECTOR); 
  readSector(dir, DIR_SECTOR);
  for (dirIndex = 0; dirIndex < MAX_FILES; ++dirIndex) { 
    if (dir[dirIndex * DIR_ENTRY_LENGTH] == '\0') {
      break; 
    }
  }

  if (dirIndex < MAX_FILES) {
    int i, j, sectorCount;
    for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
      if (map[i] == 0x00) {
        ++sectorCount; 
      }
    }

    if (sectorCount < *sectors) {
      *sectors = INSUFFICIENT_SECTORS;
      return; 
    } else {
      clear(dir + dirIndex * DIR_ENTRY_LENGTH, DIR_ENTRY_LENGTH); 
      for (i = 0; i < MAX_FILENAME; ++i) {
        if (filename[i] != '\0') {
          dir[dirIndex * DIR_ENTRY_LENGTH + i] = filename[i];
        } else {
          break; 
        }
      }
      for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
        if (map[i] == 0x00) {
          map[i] = 0xFF;
          dir[dirIndex * DIR_ENTRY_LENGTH + MAX_FILENAME + sectorCount] = i;
          clear(sectorBuffer, SECTOR_SIZE); 
          for (j = 0; j < SECTOR_SIZE; ++j) {
            sectorBuffer[j] = buffer[sectorCount * SECTOR_SIZE + j]; 
          }
          writeSector(sectorBuffer, i);
          ++sectorCount; 
        }
      } 
    }
  } else {
    *sectors = INSUFFICIENT_DIR_ENTRIES;
    return; 
  }
  writeSector(map, MAP_SECTOR); 
  writeSector(dir, DIR_SECTOR);
}

void executeProgram(char *filename, int segment, int *success){
// Mengalokasikan sebuah buffer (ukuran lihat bagian file system)
// Buka file dengan fungsi readFile
// Jika sukses, gunakan loop untuk menyalin tiap byte kode dengan fungsi putInMemory(segment, i, buffer[i])​
// Panggil fungsi launchProgram(segment) (fungsi ini telah didefinisikan di kernel.asm) untuk menjalankan program di segment yang telah terisi
  char buffer[20 * 512];
  int i;

  readFile(buffer, filename, success);

  if(*success == 1) {
    for(i = 0; i < (20 * 512); i++) {
      putInMemory(segment, i, buffer[i]);
    }
    launchProgram(segment);
  }
}
