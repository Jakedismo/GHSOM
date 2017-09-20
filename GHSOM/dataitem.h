/*********************************************************************
					dataitem.h - kuvaus
					-------------------
aloitus				: 10. Tammikuuta 2015
copyright			: (C) 2015 Jokke Ruokolainen
email				: jruokola@student.uef.fi
**********************************************************************/



#ifndef DATAITEM_H
#define DATAITEM_H

class DataItem {
public:
	DataItem(char *inid, float *indataVector, int inlength);
	~DataItem();
	float *getDataVector();
	char *getId();
private:
	char *id;
	float *dataVector;
	int length;
};

#endif