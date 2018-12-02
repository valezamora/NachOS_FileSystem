
#include "copyright.h"

#include "system.h"
#include "fileblock.h"

//----------------------------------------------------------------------
// FileBlock::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return false if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

FileBlock::FileBlock(){ 
	siguienteBloque = -1;
    for (int i = 0; i < NUM_PUNTEROS; i++)
		dataSectors[i] = -1;

}

//----------------------------------------------------------------------
// FileBlock::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void 
FileBlock::Deallocate(BitMap *freeMap)
{
    for (int i = 0; i < NUM_PUNTEROS; i++) {
		freeMap->Clear((int) dataSectors[i]);
    }
}

//----------------------------------------------------------------------
// FileBlock::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileBlock::FetchFrom(int sector)
{
    synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileBlock::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileBlock::WriteBack(int sector)
{
    synchDisk->WriteSector(sector, (char *)this); 
}

//----------------------------------------------------------------------
// FileBlock::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int
FileBlock::ByteToSector(int offset)
{
    return(dataSectors[offset / SectorSize]);
}


//----------------------------------------------------------------------
// FileBlock::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileBlock::Print(int numBytes)
{
    int i, j, k;
    char *data = new char[SectorSize];

    printf("FileBlock contents.\n");
    for (i = 0; i < NUM_PUNTEROS; i++)
		printf("%d ", dataSectors[i]);
		printf("\nFile contents:\n");
		for (i = k = 0; i < NUM_PUNTEROS; i++) {
			if(dataSectors[i] > 0){
				synchDisk->ReadSector(dataSectors[i], data);
				for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
				if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
				printf("%c", data[j]);
				    else
				printf("\\%x", (unsigned char)data[j]);
			}
		}
        printf("\n"); 
    }
    delete [] data;
}


void FileBlock::AsignarSiguiente(int next){
	siguienteBloque = next;
}


void FileBlock::asignar(int donde, int que){
	dataSectors[donde] = que;
}
