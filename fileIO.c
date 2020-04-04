void readFile(char* fileName,char* buffer){
    int fileFound;
    int nameCt = 0;
	int index, k,h;
	int sectors[27];
	int j = 0;
	int i;
	int buffAddress = 0;


    readSector(buffer, 2);

	fileFound = strComp(buffer,fileName);

	if (fileFound!=0){
		index = fileFound*32+6;
		for (j=0;j<26;j++){
			sectors[j] = buffer[index+j];

		}

		sectors[26] = 0;
		k = 0;
		while(sectors[k]!=0x0){
			readSector(buffer+buffAddress,sectors[k]);
			buffAddress += 512;
			k++;
		}

	}
	else{
		printString("File Not Found!");
		return;
	}

}

void writeFile(char* name,char* buffer, int numberOfSectors) {
	char map[512];
	char directory[512];
	int directoryLine,j,k, index, diff;
	int nameLen = 0;
	int sectorNum;
	char subBuff[512];
	int iterator = 0;
	int foundFree = 0;
	int nameInts[7];
	int i,h;
	int kVal;


	readSector(map,1);
	readSector(directory,2);

     	for (directoryLine = 0; directoryLine < 16; directoryLine++){
		if (directory[32*directoryLine] == 0x00){
			foundFree = 1;
			break;
		}
	}
	if (foundFree == 0){
		printString("Didn't find empty location for file.");
		return;
	}

	while(name[nameLen] != '\0' && name[nameLen] != 0x0){
		nameLen++;
	}
	for (j=0;j<nameLen;j++){
		directory[32*directoryLine+j] = name[j];
	}
	if (nameLen < 6){
		diff = 6-nameLen;
		for(j=0;j<diff;j++){
			index = j+nameLen;
			directory[32*directoryLine+index] = 0x0;
		}
	}

	for (k = 0; k < numberOfSectors; k++){

		sectorNum = 0;
		while(map[sectorNum] != 0x0){
			sectorNum++;
		}
		if (sectorNum==26)
		{
			printString("Not enough space in directory entry for file\n");
			return;
		}
		map[sectorNum] = 0xFF;
		directory[32*directoryLine+6+k] = sectorNum;
		for(j=0;j<512;j++){
			kVal = k+1;
			subBuff[j] = buffer[j*kVal];
		}
		writeSector(subBuff,sectorNum);
	}
	writeSector(map,1);
	writeSector(directory,2);
}

void deleteFile(char* name){
	char map[512];
	char directory[512];
	int sectors[27];
	int sectorCount = 0;
	int i, j, k, fileFound, index;
	int sectNum;
	int sector;

	readSector(map,1);
	readSector(directory,2);


	fileFound = strComp(directory,name);

	if (fileFound!=0){

		for(i=0;i<6;i++){
			directory[fileFound*32+i] = 0x00;
		}
		index = fileFound*32+6;
		for (j=0;j<26;j++){
			sectors[j] = directory[index+j];
			directory[index+j] = 0x00;
			sectorCount++;

		}
		sectors[26] = 0x0;

		for(k=0;k<sectorCount;k++){
			sector = sectors[k];
			if(sector == 0){
				break;
			}

			map[sector] = 0x00;

		}
	}
	else{
		printString("File not found!");
	}

	writeSector(map,1);
	writeSector(directory,2);
}