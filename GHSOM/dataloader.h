/*********************************************************************
					dataloader.h - kuvaus
					-------------------
aloitus				: 19. Tammikuuta 2015
copyright			: (C) 2015 Jokke Ruokolainen
email				: jruokola@student.uef.fi
**********************************************************************/

#ifndef DATALOADER_H
#define DATALOADER_H

#include "dataitem.h"
#include "vector.h"
typedef Vector<DataItem> Data_Vector;

Class DataLoader {
public:
	DataLoader();
	~DataLoader();
	/**/
	void readVectorDescription(char* dfName);
	/**/
	char** getVectorDescription();
	/**/
	bool providesVectorDescription();
	/**/
	Data_Vector* readDataItems(char *ifName);
private:
	int dataLength;
	int numVectors;
	char **vectorDescription;
};


#endif