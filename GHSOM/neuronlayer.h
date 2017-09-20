/*********************************************************************
					neuronlayer.h - kuvaus
					-------------------
aloitus				: 22. Tammikuuta 2015
copyright			: (C) 2015 Jokke Ruokolainen
email				: jruokola@student.uef.fi
**********************************************************************/

#ifndef NEURONLAYER_H
#define NEURONLAYER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include "neuron.h"
#include "vector.h"
#include "globals.h"
typedef Vector<DataItem> Data_Vector;
typedef Vector<char> String_Vector;
class Neuron;

class NeuronLayer {
public:
	int gid;
	float superMQE;

	NeuronLayer(Neuron *sn, Data_Vector *dataItems, float superMQE, int level, int initialSizeX, int initialSizeY, int posX, int posY, float *ULweight, float *URweight, float *LLweight, float *LR weight);
	~NeuronLayer();
	/**/
	NeuronLayer* getLayer1Map();
	/**/
	void calcLayer0MQE();
	/**/
	void calcMQE();
	/**/
	void insertColumn(int pos);
	/**/
	void train();
	/**/
	void saveAs(int what);
private:
	Data_Vector *dataItems;
	int *superPos;
	Neuron *superNeuron;
	float MQE;
	int *MQENeuron;
	float stupidity;
	float ini_learnrate;
	float learnrate;
	float min_learnrate;
	float ini_neighbourhood;
	float neighbourhood;
	float min_neighbourhood;
	float neigbhourhood_falloff;
	int level;
	int dataLength;
	int x;
	int y;
	float STRETCH_PARAM_LEARN;
	float STRETCH_PARAM_NEIGHB;
	Neuron ***neuronMap;
	int currentCycle;
private: //Private methods
	/**/
	void adaptWeights(int *winner, DataItem *di);
	/**/
	void testDataItems();
	/**/	
	int *getMaxDissNeighbour(int *n);
	/**/
	void saveAsHTML();
	/**/
	void insertRow(int pos);
	/**/
	void saveAsSOMLib();
	/**/
	void getNewWeights(int xPos, int yPos, float *UL, float *UR, float *LL, float *LR);
};

#endif