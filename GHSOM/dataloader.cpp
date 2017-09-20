/******************************************************
						dataloader.cpp - kuvaus
						-----------------------
Aloitus					:10. Tammikuuta 2015
copyright				:(C) 2015 Jokke Ruokolainen
email					:jruokola@student.uef.fi
********************************************************/


#include "dataloader.h"

#include "globals.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>

DataLoader::DataLoader() {
}
DataLoader::~DataLoader() {
}

Data_Vector* DataLoader::readDataItems(char *ifName) {
	std::ifstream inFile(ifName);
	if (!inFile) {
		std::cout << "inputfile" << ifName << "not found (Check propertyfile)" << std::endl;
		exit(EXIT_FAILURE);
	}
	Data_Vector *res = new Data_Vector();
	char veclen[150], numdat[150], element[150], dummy[150];
	inFile.getline(dummy, 150, '\n'); //$TYPE
	inFile.getline(dummy, 150, ' '); //$XDIM
	inFile.getline(numdat, 150, '\n'); //$XDIM value
	inFile.getline(dummy, 150, '\n'); //$YDIM
	inFile.getline(dummy, 150, ' '); //$VEC_DIM
	inFile.getline(dummy, 150, '\n'); //$VEC_DIM_Value

	//inFile.getline(veclen,150,'\n');
	//inFile.getline(numdat,150,'\n');
	dataLength = atoi(veclen);
	Globals::vectorlength = dataLength;
	numVectors = atoi(numdat);
	Globals::numofallvecs = numVectors;
	int debuggy = 0;
	for (int j = 0; j < numVectors; j++) {
		float *data = (float*)calloc(dataLength, sizeof(float)); 
		for (int i = 0; i < dataLength; i++) {
			inFile.getline(element, 150, ' ');
			data[i] = atof(element);
		}
		//inFile.getline(element, 150, '\n');
		//data[dataLength-1] = atof(element);

		if (Globals::normInputVectors == NORM_LENGTH) {
			data = Globals::normVec(data);
		} 
		else if (Globals::normInputVectors=NORM_INTERVAL) {
			data = Globals:normIntervalVector(data);
		}
		char *id;
		inFile.getline(dummy, 150, '\n');
		id = (char*)malloc(strlen(dummy) + 1);
		strcpy(id, dummy);
		DataItem *di = new DataItem(id, data, dataLength);
		res->addElement(id);
		std::cout << "Lisätty" << id << std:endl;
	}
	if (Globals::normInputVectors == NORM_INTERVAL) {
		res = Globals::normIntervalVector(res);
	}
	return res;
}
/**/
bool DataLoader::providesVectorDescription() {
	return true;
}
/**/
char** DataLoader::getVectorDescription() {
	return vectorDescription;
}
/**/
void DataLoader::readVectorDescription(char* dfName) {
	std::ifstream dFile(dfName);
	if (!dfFile) {
		std::cout << "descriptionfile" << dfName << "not found (check propertyfile)" << std:endl;
		exit(EXIT_FAILURE);
	}
	vectorDescription = (char**)calloc(dataLength, sizeof(char*));
	char dummy[150];
	dfFile.getline(dummy, 150, '\n'); //$TYPE
	dfFile.getline(dummy, 150, '\n'); //$XDIM
	dfFile.getline(dummy, 150, '\n');//$YDIM
	dfFile.getline(dummy, 150, '\n');//$VEC_DIM
	for (int i = 0; i < dataLength; i++) {
		dfFile.getline(dummy, 150, ' ') //$INDEX
			//std::cout << "XXX readVectorDescription: index: " << dummy << std::endl;
			dfFile.getline(dummy, 150, ' ');
		//std::cout << "XXX readVectorDescription: name: " << dummy << std::endl;
		vectorDescription[i] = (char*)malloc(strlen(dummy) + 1);
		strcpy(vectorDescription[i], dummy);
		//std::cout << "XXX readVectorDescription: " << vectorDescription[i] << std::endl;
		dfFile.getline(dummy, 150, '\n');//rest
		//std::cout << "XXX readVectorDescription: " << i << std::endl;
	}
	dfFile.close();
	//std::cout << "XXX readVectorDescription: free dummy" << std::endl;
	//free(dummy);
	//std::cout << "XXX readVectorDescription: free dummy" << std::endl;
}
	
