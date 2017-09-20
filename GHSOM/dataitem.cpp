/*******************************************************************************
							dataitem.cpp - kuvaus
							---------------------
 aloitus					: 10. Tammikuuta 2015
 copyright					: (C) 2015 Jokke Ruokolainen
 email						: jruokola@student.uef.fi
********************************************************************************/

#include <cstdlib>
#include "dataitem.h"

DataItem::DataItem(char *inid, float *indataVector, int inlength) {
	id = inid;
	dataVector = indataVector;
	length = inlength;
}
DataItem::~DataItem() {
	free(id);
	free(dataVector);
}
/** */
float *DataItem::getDataVector() {
	return dataVector;
}
/** */
float *DataItem::getId() {
	return id;
}