// filehdr.h 
//	Data structures for managing a disk file header.  
//
//	A file header describes where on disk to find the data in a file,
//	along with other information about the file (for instance, its
//	length, owner, etc.)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#ifndef FILEBLOCK_H
#define FILEBLOCK_H

#include "disk.h"
#include "bitmap.h"
#include "system.h"	
#include "filehdr.h"	

#define NUM_PUNTEROS (SectorSize - sizeof(int))/sizeof(int)

// The following class defines the Nachos "file header" (in UNIX terms,  
// the "i-node"), describing where on disk to find all of the data in the file.
// The file header is organized as a simple table of pointers to
// data blocks. 
//
// The file header data structure can be stored in memory or on disk.
// When it is on disk, it is stored in a single sector -- this means
// that we assume the size of this data structure to be the same
// as one disk sector.  Without indirect addressing, this
// limits the maximum file length to just under 4K bytes.
//
// There is no constructor; rather the file header can be initialized
// by allocating blocks for the file (if it is a new file), or by
// reading it from disk.

class FileBlock {
  public:
	FileBlock();

    void Deallocate(BitMap *bitMap);  		// De-allocate this file's 
						//  data blocks

    void FetchFrom(int sectorNumber); 	// Initialize file header from disk
    void WriteBack(int sectorNumber); 	// Write modifications to file header
					//  back to disk

    int ByteToSector(int offset);	// Convert a byte offset into the file
					// to the disk sector containing
					// the byte

    void Print(int numBytes);			// Print the contents of the file.

	void AsignarSiguiente(int next);
	
	void asignar(int donde, int que);
  private:

    int dataSectors[NUM_PUNTEROS];		// Disk sector numbers for each data  block in the file
	int siguienteBloque;
};

#endif // FILEBLOCK_H
