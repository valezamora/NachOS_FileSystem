// filehdr.cc 
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the 
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the 
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect 
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector, 
//
//      Unlike in a real system, we do not keep track of file permissions, 
//	ownership, last modification date, etc., in the file header. 
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "filehdr.h"

//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return false if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

bool
FileHeader::Allocate(BitMap *freeMap, int fileSize)
{ 
    /*numBytes = fileSize;
    numSectors  = divRoundUp(fileSize, SectorSize);	//cantidad de sectores requeridos para almacenar el contenido del archivo
    int numSectorsExtra = numSectors - NumDirect;	//cantidad de punteros por almacenar sin contar los que estan en el bloque del fileheader
    int totalSectors = numSectors;		//total de sectores requeridos
    
    if(numSectorsExtra > 0){	//se requiere de mas bloques de punteros
    	numSectorsExtra = divRoundUp(numSectorsExtra,NumDirect2);	//cantidad de bloques requeridos para almacenar punteros
    	totalSectors += numSectorsExtra;	// agrega a la cantidad de bloques requerida la cantidad de bloques para punteros
    }
    
    if (freeMap->NumClear() < totalSectors){
		return false;		// not enough space
	}
	
	int i = 0;

	if(numSectorsExtra == 0){
		//solo hay que llenar los punteros requeridos dentro del vector del fileheader
			//asignar punteros a bloques dentro del fileheader
		for (i = 0; i < numSectors; i++){
			dataSectors[i] = freeMap->Find();
		}		
	}else{
		//asignar punteros a bloques en el vector dentro del fileheader
		for (i = 0; i < NumDirect; i++){
			dataSectors[i] = freeMap->Find();
		}
		
		siguienteBloque = freeMap->Find(); 	//asigna siguiente bloque del header
		
		// loop para asignar los demas bloques
		int sigTemp = siguienteBloque;
		int siguienteTemporal[NumDirect2+1];	//crear vector temporal para guardar a las posiciones del disco del bloque
		int contBloque = 0;
		int temporal = 0;
		
		for(int j = 0; j < numSectorsExtra; ++j){
			while(contBloque < NumDirect2){
				siguienteTemporal[contBloque] = freeMap->Find();				//guardar siguiente en siguiente bloque el puntero
				++contBloque;
			}
			
			contBloque = 0;
			
			//se necesita otro sector mas
		 	if((j+1) < numSectorsExtra){
		 		temporal = freeMap->Find();	
		 		siguienteTemporal[NumDirect2] = temporal;		//asignar al ultimo del siguiente sector un nuevo sector
		 	}else{
		 		siguienteTemporal[NumDirect2] = -1;		//asignar al ultimo del siguiente sector un nuevo sector
		 	}
			
			synchDisk->WriteSector(sigTemp, (char *)siguienteTemporal);				//escribe el sector en disco
			sigTemp = temporal;
		}
		
	}
	
    return true;*/
    
    numBytes = 0;
    numSectors  = 0;
    siguienteBloque = -1;
}

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void 
FileHeader::Deallocate(BitMap *freeMap)
{
	int i = 0;
	int contadorSectores = 0;
	if(numSectors < NumDirect){
		//no hay bloques extra de punteros
		for (i = 0; i < numSectors; i++) {
			ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
			freeMap->Clear((int) dataSectors[i]);
    	}
	}else{
		//hay otros bloques
		//borrar los que estan almacenados en el vector del fileheader
		for (i = 0; i < NumDirect; i++) {
			ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
			freeMap->Clear((int) dataSectors[i]);
			++contadorSectores;
    	}
    	
    	//recorrer los siguientes bloques
    	int sector[NumDirect2+1];
    	int numSector = siguienteBloque;
    	int contadorBloque = 0;
    	
    	//cabe en un solo bloque mas?
    	int bloquesExtra = divRoundUp((numSectors-NumDirect), NumDirect2);
    	
    	for(i = 0; i<bloquesExtra; ++i){
    		// obtiene siguiente sector
	    	synchDisk->ReadSector(numSector, (char *)sector);	//guarda en sector el contenido del siguiente bloque  
	    	//recorre sector para desocupar cada espacio.
	    	while(contadorSectores < numSectors && contadorBloque < NumDirect2){
	    		ASSERT(freeMap->Test((int) sector[contadorBloque]));  // ought to be marked!
				freeMap->Clear((int) sector[contadorBloque]);
				
	    		++contadorSectores;
	    		++contadorBloque;
	    	}
	    	contadorBloque = 0;
	    	numSector = sector[NumDirect2];  	//siguiente sector
    	}
    	
	}	
    
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileHeader::FetchFrom(int sector)
{
    synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileHeader::WriteBack(int sector)
{
    synchDisk->WriteSector(sector, (char *)this); 
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int
FileHeader::ByteToSector(int offset)
{
	int sectorNum = offset / SectorSize;
	
	if( sectorNum <= NumDirect )
    	return(dataSectors[offset / SectorSize]);
    else{
    	int sigSec = sigPosSector;					// Posición del siguiente sector de índices
    	int data[SectorSize/sizeof(int)];						// Vector para leer sectores
    	int secBuscado = sectorNum % NumDirect2;	// El sector deseado
    	
    	sectorNum -= NumDirect;						// Contador para saber si falta o no recorrer otro sector de índices
    	
    	do{
    		synchDisk->ReadSector( sigSec, (char *)data );	// Se lee el sector
    		sectorNum -= NumDirect2;				// Se resta la cantidad de índices de este sector
    		
    		// Se calcula el índice del siguiente sector de índices en caso de ser necesario
    		sigSec = data[ SectorSize - sizeof(int) ];	
    	}
    	while( sectorNum > 0 && sigSec != -1 ); // Asumiendo que el -1 sea el valor de un final lógico
    	
    	// Se retorn el índice del sector deseado
    	return data[secBuscado];
    }
}

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
FileHeader::FileLength()
{
    return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileHeader::Print()
{
    int i, j, k;
    char *data = new char[SectorSize];

    printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes, numSectors);
    for (i = 0; i < numSectors; i++)
    	if(i< NumDirect){
			printf("%d ", dataSectors[i]);
			printf("\nFile contents:\n");
			for (i = k = 0; i < numSectors; i++) {
				synchDisk->ReadSector(dataSectors[i], data);
					for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
					if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
					printf("%c", data[j]);
						else
					printf("\\%x", (unsigned char)data[j]);
				}
				
					printf("\n"); 
			}
    	}else{
    		//imprime de otro sector
    		s
    		
    	}
    delete [] data;
}

//-------------------------------------------------------------------------
// FUNCION AGREGADA
/*
* Agrega el espacio requerido para que el tamano del archivo sea variable.	
*/

bool FileHeader::AddLength(int n)
{
	fileLock->Acquire();
	//add number of bytes
    numBytes += n;

	int bytesUltimoSector = (numBytes - n) % SectorSize; // Cantidad bytes utilizados en el último índice

	if( (bytesUltimoSector + n) > SectorSize ){ // Voy a ocupar un nuevo sector
    	int nuevosSectores = divRoundUp(numBytes, SectorSize) - numSectors; // Cantidad de nuevos sectores requeridos
    	int capacidadTotSec = NumDirect; // Capacidad total de sectores que tengo para indízan
    	int indBloque = siguienteBloque; // Nos indica el índice del nuevo bloque de índices
    	int data[NumDirect2 + 1]; // Bloque de índices
    	
    	// Recorre toda la lista de índices contando cuantos puede almacenar
    	while( indBloque != -1 ){
    		ReadAt( (char *)data, SectorSize, indBloque * SectorSize );
    		capacidadTotSec += NumDirect2;
    		indBloque = data[NumDirect2];
    	}
    
    	if( (numSectors + nuevosSectores) > capacidadTotSec){ // Ocupo un nuevo bloque para almacenar índices
    		// ¿Es posible obtener la cantidad de sectores requeridos?; Si no, retorne falso
    		// nuevos sectores más la cantidad de almacenes necesarios para ellos
    		if( freeMap->NumClear() < nuevosSectores + divRoundUp( (numSectors + nuevosSectores) - capacidadTotSec , NumDirect2) )
				return false;		// not enough space
    	}
    	else{
    		// ¿Es posible obtener la cantidad de sectores requeridos?; Si no, retorne falso
    		if( freeMap->NumClear() < nuevosSectores )
				return false;		// not enough space
    	}
    	
    	indBloque = -1; // "Inicializar" el índice para saber si se creo un nuevo sector
    	
    	for(int i = 0; i < nuevosSectores; i++){
    		if( numSectors < NumDirect ){
    			// Los nuevos sectores caben en el primer bloque
    			if( (numSectors + 1) < NumDirect ){
    				dataSectors[numSectors + i] = freeMap->Find();
    			}
    			else{ // Necesitamos guardarlo en el siguiente bloque
    				// Busca un nuevo bloque para índices y guarda un nuevo índice
    				siguienteBloque = freeMap->Find();
    				ReadAt( (char *)data, SectorSize, siguienteBloque * SectorSize );
    				data[0] = freeMap->Find();
    			}
    		}
    		else{
    			// El nuevo bloque se almacena en los "almacenes" que tenemos
    			if( (numSectors + 1) < (NumDirect + NumDirect2) ){
    				data[numSectors - NumDirect] = freeMap->Find();
    			}
    			else{
    				// Busca un nuevo bloque para índices y guarda un nuevo índice
    				data[NumDirect2] = freeMap->Find();
    				ReadAt( (char *)data, SectorSize, data[NumDirect2] * SectorSize );
    				data[0] = freeMap->Find();
    			}
    		}
    		++numSectors;
    	}
    }
    
    fileLock->Release();
    return true;
}
