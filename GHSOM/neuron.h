/*********************************************************************
						neuron.h - kuvaus
						-------------------
aloitus				: 22. Tammikuuta 2015
copyright			: (C) 2015 Jokke Ruokolainen
email				: jruokola@student.uef.fi
**********************************************************************/

#ifndef NEURON_H
#define NEURON_H

#include "neuronlayer.h"

#include "dataitem.h"
#include "globals.h"
#include "vector.h"
typedef Vector<DataItem> Data_Vector;
typedef Vector<char> String_Vector;
typedef Vector<float> Double_Vector;
class NeuronLayer;

class Neuron {
public:
	Neuron(int ws, int ingid, int inlevel, int spx, int spy);
	Neuron(float *w, int ingid, int inlevel, int spx, int spy);
	~Neuron();
	/**/
	void adaptWeights(DataItem *di, float dist, float learnrate, float neighbourhood);
	/**/
	void calcDist(DataItem *di);
	/**/
	void calcMQE();
	/**/
	void addRepresentingDataItem(DataItem *di);
	/**/
	int representsMultiDataItems();
	/**/
	NeuronLayer *getMAP();
	/**/
	void addMap(Neuron *sn, float MQE, int level, int sizeX, int sizeY, int posX, int posY);
	/**/
	void addMap(Data_Vector *dataItems, float MQE, int level, int sizeX, int sizeY, int posX, int posY);
	/**/
	void addMap(Neuron *sn, float MQE, int level, int posX, int posY, float *ULweight, float *URweight, float *LLweight, float *LRweight);
	/**/
	Data_Vector *getRepresentingDataItems();
	/**/
	void clearRepresentingDataItems();
	/**/
	String_Vector *getLabels(int num);

	int mapgid, maplevel, mapspx, mapspy;
	float MQE;
	float *weights;
private:
	int weightsize;
	NeuronLayer *nl;
	Data_Vector *representingDataItems;
};