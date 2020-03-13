#define MAX_BYTE 256
#define SECTOR_SIZE 512 
#define MAX_FILES 16 
#define MAX_FILENAME 12 
#define MAX_SECTORS 20 
#define DIR_ENTRY_LENGTH 32 
#define MAP_SECTOR 256 
#define INSUFFICIENT_SECTORS 0 
#define INSUFFICIENT_DIR_ENTRIES -1 
#define FILES_SECTOR 258
#define DIR_SECTOR 257
#define SECTORS_SECTOR 259

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
  if (p==0){
      return 0;
  } else {
    int x = 0;
    while (x*q<=p){
        x ++;
    }
    
    return x-1;
    } 
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

void readFile(char *buffer, char *path, int *result, char parentIndex){
    //read file with readSector
    //SECTOR_ENTRY_LENGTH = 16
    //SECTOR_SIZE = 512

    int letakpath = 0;
    int idx = 0;
    int name_offset = 0;
    int dir_offset = 0;
    int i = 0;
    int letakpath = 0;
    int sec_offset = 0;
    //mencari index filename pertama kali
    while (path[i] != '\0'){
      if (path[i]=='/') {
        idx = i + 1; //index char pertama di depan /
      }
      ++i;
    }
    i = 0;
    if (idx == 0) { //tidak ketemu
      dir_offset = parentIndex;
    } else {
      while ( i < 512 ){ //cari dir offset
        if (path[letakpath] ==parentIndex){
          i = 513;
          dir_offset = letakpath;
        }
      }
      if (dir_offset == 0xFF){
        *result = -1; //File tidak ketemu
      }
    }
    i = 0;
    char sector[512];
    while(sector[i*16 + sec_offset]!='\0' && sec_offset<16){
      readSector(buffer+sec_offset*512,sector[i*16+sec_offset]);
      ++i;
      ++sec_offset;
    }
}

void clear(char *buffer, int length){
  int i;
  for(i=0;i<length;i++){
    buffer[i] = 0x0;
  }
} 
//Fungsi untuk mengisi buffer dengan 0
void writeFile(char *buffer, char *path,int *sectors, char parentIndex){
  //inisialisasi sector dan masing masing offset untuk filesystem
  char sectors[512];
  char files[512];
  char map[512];
  int map_offset = 0;
  int files_offset = 0;
  int sectors_offset = 0;
  int i;
  //Sectors
  readSector(sectors,259);
  i = 0;
  //Files
  readSector(files,258);
  i = 0;
  while (i<32){
    if (files[i] == '\0'){
      files_offset = i;
      i = 32;
    }
    i++;
  }
  if (i==32){ // not found
    *result = -3;
    return;
  }

  //Map
  readSector(map,256);
  i = 0;
  while (i<256) {
    if (map[i] == '\0'){
      map_offset = i;
      i = 256;
    } 
    i++;
  }
  if (i==256){ // not found
    *result = -3; // sektor tidak cukup
    return;
  }

  writeSector(map,MAP_SECTOR);
  writeSector(files,FILES_SECTOR);
  writeSector(sectors,SECTORS_SECTOR);
  *result = 0;
}

void executeProgram(char *filename, int segment, int *success){
// Mengalokasikan sebuah buffer (ukuran lihat bagian file system)
// Buka file dengan fungsi readFile
// Jika sukses, gunakan loop untuk menyalin tiap byte kode dengan fungsi putInMemory(segment, i, buffer[i])​
// Panggil fungsi launchProgram(segment) (fungsi ini telah didefinisikan di kernel.asm) untuk menjalankan program di segment yang telah terisi
  int buf_size = 10240;
  char buf[buf_size];
  int i;
  readFile(buf, filename, success);
  if(*success == 1) {
    while (i<buf_size){
      putInMemory(segment,i,buf[i])
    }
    launchProgram(segment);
  }
}
