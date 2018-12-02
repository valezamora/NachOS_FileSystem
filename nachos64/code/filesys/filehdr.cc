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
    /* = fileSize;
    numSectors  = divRoundUp(fileSize, SectorSize);	//cantidad de sectores requeridos para almacenar el contenido del archivo
printf("cantidad de sectores: %d\n", numSectors);		
	
    int numSectorsExtra = numSectors - NumDirect;	//cantidad de punteros por almacenar sin contar los que estan en el bloque del fileheader
    int totalSectors = numSectors;		//total de sectores requeridos
printf("Bloques extra: %d\n", numSectorsExtra);		
	
    
    if(numSectorsExtra > 0){	//se requiere de mas bloques de punteros
    	numSectorsExtra = divRoundUp(numSectorsExtra,NumDirect2);	//cantidad de bloques requeridos para almacenar punteros
    	totalSectors += numSectorsExtra;	// agrega a la cantidad de bloques requerida la cantidad de bloques para punteros
    }
    
    if (freeMap->NumClear() < totalSectors){
		return false;		// not enough space
	}
	
	int i = 0;

printf("Bloques extra: %d\n", numSectorsExtra);		
	

	if(numSectorsExtra <= 0){
		//solo hay que llenar los punteros requeridos dentro del vector del fileheader
			//asignar punteros a bloques dentro del fileheader
		for (i = 0; i < numSectors; i++){
			dataSectors[i] = freeMap->Find();
		}		
		printf("Cabe en un solo bloque\n");
	}else{
		printf("No cabe en un solo bloque\n");
		//asignar punteros a bloques en el vector dentro del fileheader
		for (i = 0; i < NumDirect; i++){
			dataSectors[i] = freeMap->Find();
		}
		printf("Asigno el principal\n");
		siguienteBloque = freeMap->Find(); 	//asigna siguiente bloque del header

		// loop para asignar los demas bloques
		int sigTemp = siguienteBloque;
		int siguienteTemporal[NumDirect2+1];	//crear vector temporal para guardar a las posiciones del disco del bloque
		int contBloque = 0;
		int temporal = 0;
		int bloquesAsignados = 0;
		int bloquesRestantes = (totalSectors-numSectorsExtra-NumDirect);
	
		printf("Restantes: %d\n", bloquesRestantes);		
	
		
		for(int j = 0; j < numSectorsExtra; ++j){	//cantidad de sectores requeridos para almacenar punteros
			printf("Siguiente bloque: %d\n", siguienteBloque);		
	
			while(contBloque < NumDirect2 && bloquesAsignados < bloquesRestantes){
				siguienteTemporal[contBloque] = freeMap->Find();				//guardar siguiente en siguiente bloque el puntero
				++contBloque;
				++bloquesAsignados;
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
	printf("Termino allocate\n" );		

*/

	numSectors = 0;
	numBytes = 0;
	siguienteBloque = -1;
    return true;

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
printf("inico: deallocate\n" );
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
	    	++contadorSectores;
	    	freeMap->Clear(numSector);
	    	numSector = sector[NumDirect2];  	//siguiente sector
    	}
	}	
    printf("final: de allocate\n" );
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
    printf("write back\n" );		
	
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
	
	if( sectorNum < NumDirect ){
    	return(dataSectors[sectorNum]);
    }else{
    //buscar sector donde esta almacenado en otro bloque
    	int sigSec = siguienteBloque;					// Posición del siguiente sector de índices
    	int data[NumDirect2];						// Vector para leer sectores
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
	printf("inicio: add length\n" );
	bool result = true;
	int i = 0;
	
   	fileLock->Acquire();
	OpenFile* bm = new 	OpenFile(0);
    BitMap *freeMap = new BitMap(NumSectors);
    freeMap->FetchFrom(bm);	
	
	int sectoresRequeridosTotal = divRoundUp(numBytes+n, SectorSize);
	if(sectoresRequeridosTotal < NumDirect){
		// Caben en NumDirect sectores
		// hay suficientes disponibles 
		if (freeMap->NumClear() < sectoresRequeridosTotal-numSectors){
			result = false;
		}else{
			for(i = numSectors; i < sectoresRequeridosTotal; ++i){
			// asignar los sectores que faltan
			dataSectors[i] = freeMap->Find();
			}
			numBytes += n;
			numSectors = sectoresRequeridosTotal;
		}
		
	}else{
		// archivo necesita mas sectores para almacenar punteros
   	
	   	// cantidad de sectores para almacenar punteros
	   	int punteros = sectoresRequeridosTotal - NumDirect;
   		int sectoresPunteros = divRoundUp(punteros,SectorSize);
   		
   		if(freeMap->NumClear() < (sectoresRequeridosTotal - numSectors + sectoresPunteros)){
   			//no hay espacio
   			result = false;
   		}else{
   			//SI HAY CAMPO
   			
   			// Asigna los que faltan del vector local
   			for(i = numSectors; i < NumDirect; ++i){
				dataSectors[i] = freeMap->Find();
			}
			
			//asigna siguiente bloque de datos
			siguienteBloque = freeMap->Find();
			
			FileBlock * bloquePunteros = new FileBlock();
			
			int bloqueTemp = siguienteBloque;
			int contadorAsignados = 0;
			int contadorInterno = 0;
			int temporal = -1;
			
			for(i=0; i<sectoresPunteros; ++i){
				bloquePunteros->FetchFrom(bloqueTemp);
				contadorInterno= 0;
				temporal = -1;
				while(contadorAsignados < punteros && contadorInterno < NUM_PUNTEROS){
					//faltan bloques por asignar
					bloquePunteros->asignar(contadorInterno, freeMap->Find());
					++contadorInterno;
					++contadorAsignados;					
				}
				//siguiente bloque
				if(i+1 < sectoresPunteros){
					temporal = freeMap->Find();
					//necesito otro
				}
				bloquePunteros->AsignarSiguiente(temporal);
				bloquePunteros->WriteBack(bloqueTemp);
				bloqueTemp = temporal;
			}
			numBytes += n;
			numSectors = sectoresRequeridosTotal;
   			delete bloquePunteros;
   		}

	}
   	delete bm;
	delete freeMap;  

	fileLock->Release();  	
     
    printf("final: add length\n" );
    return result; 
}
