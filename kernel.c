

/* Ini deklarasi fungsi */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
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

int main() {
  makeInterrupt21();
  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
      break;
    case 0x2:
      readSector(BX, CX);
      break;
    case 0x3:
      writeSector(BX, CX);
      break;
    case 0x4:
      readFile(BX, CX, DX);
      break;
    case 0x5:
      writeFile(BX, CX, DX);
      break;
    case 0x6:
      executeProgram(BX, CX, DX);
      break;
    default:
      printString("Invalid interrupt");
  }
}

void printString(char *string){
  int i = 0;
  char c = string[i];
  while (c != '\0'){
    interrupt(0x10, 0xE*256 + c, 0, 0, 0);          //interrupt 0x10 untuk menulis string
    c = string[++i];
  }
  interrupt(0x10, 0xE*256 + '\n',0,0,0);
  interrupt(0x10, 0xE*256 + '\r',0,0,0);
}

void readString(char *string){
  int i;
  char c;
  do {
    c = interrupt(0x16,0,0,0,0);
    if (c=='\b'){
      interrupt(0x10,0xE*256 + '\b',0,0,0);
      interrupt(0x10,0xE*256 + '\0',0,0,0);
      interrupt(0x10,0xE*256 + '\b',0,0,0);
      i--;
    } else if (c!='\r'){
      string[i] = c;
      interrupt(0x10, 0xE * 256 + c, 0, 0, 0); 
      i++;
    }
  } while (c!='\r');
  string[i] = '\0';
  interrupt(0x10, 0xE * 256 + '\n', 0, 0, 0);
  interrupt(0x10,0xE*256 + '\r',0,0,0);  
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

}

void clear(char *buffer, int length){
  int i;
  for(i=0;i<length;i++){
    buffer[i] = 0x00;
  }
} 
//Fungsi untuk mengisi buffer dengan 0
void writeFile(char *buffer, char *filename, int *sectors){

}

void executeProgram(char *filename, int segment, int *success){
// Mengalokasikan sebuah buffer (ukuran lihat bagian file system)
// Buka file dengan fungsi readFile
// Jika sukses, gunakan loop untuk menyalin tiap byte kode dengan fungsi putInMemory(segment, i, buffer[i])​
// Panggil fungsi launchProgram(segment) (fungsi ini telah didefinisikan di kernel.asm) untuk menjalankan program di segment yang telah terisi
}
