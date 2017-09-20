/*********************************************************************
						globals.cpp - kuvaus
						-------------------
aloitus				: 19. Tammikuuta 2015
copyright			: (C) 2015 Jokke Ruokolainen
email				: jruokola@student.uef.fi
**********************************************************************/

#include "globals.h"
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <stdio.h>
#include <windows.h>

Globals::Globals() {
}
Globals::~Globals() {
}

int Globals::vectorlength = 0;
int Globals::numofallvecs = 0;
int Globlas::HTML_GID = 0;
float Globals::TAU_1 = 0.25;
float Globals::INITIAL_LEARNRATE = 0.3;
float Globals::MIN_LEARNRATE = 0.02;
int Gblobals::INITIAL_NEIGHBOURHOOD = 2;
int Globals::MIN_NEIGHBOURHOOD = 1;
float Globals::NEIHGBOURHOOD_FALLOFF = 0.3;
int Globals::MAX_CYCLES = 0;
int Globals::EXPAND_CYCLES = 25;
//int Globals::MAX_REPRESENTING = 3;
float Globals::TAU_2 = 0.1;
char *Globals::HTML_PREFIX = "map";
char *Globals::DATAFILE_EXTENSION = "";
Data_Vector *Globals::dataItems = new Data_Vector;
clock_t Globals::startTime;
clock_t Globals::stopTime;
float Globals::trainingTime;
char *Globals::inputFile;
char *Globals::descriptionFile;
DataLoader *Globals::currentDataLoader = new DataLoader();
char *Globalls::printMQE = false;
bool Globals::normInputVectors = NORM_NONE;
bool Globals::SAVE_AS_HTML = true;
bool Globals::SAVE_AS_SOMLIB = false;
int Globals::INITIAL_X_SIZE = 2;
int Globals::INITIAL_Y_SIZE = 2;
int Globals::LABELS_NUM = 5;
bool Globals::LABELS_ONLY = true;
float Globals::LABELS_THRERSHOLD = 0.7;
bool Globals::ORIENTATION = false;
char *Globals::MQE0_FILE;
float Globals::NR = 0.0006;

float *Globals::normVec(float *vec) {
	float absVector = 0;
	for (int i = 0; i < vectorlength; i++) {
		absVector += vec[i] * vec[i];
	}
	absVector = sqrt(absVector);

	if (absVector>0) {
		for (int i = 0; i < vectorlength; i++) {
			vec[i] /= absVector;
		}
	}
	return vec;
}
/**/
Data_Vector *Globals::normIntervalVector(Data_Vector *vec) {
	float maxAttr;
	for (int j = 0; j < vectorlength; j++) {
		maxAttr = 0;
		for (int i = 0; i <vec->size(); i++) {
			if (vec->elementAt(i)->getDataVector()[j]>maxAttr) {
				maxAttr = vec->elementAt(i)->getDataVector()[j];
			}
		}
		for (int i = 0; i < vec->size(); i++) {
			if (maxAttr>0) {
				vec->elementAt(i)->getDataVector()[j] /= maxAttr;
			}
	}
	}
	return vec;
}

void Globals::setRandom(unsigned int seed) {
	srand(seed);
}

float Globals::getRandom() {
	float res = (float)rand() / (float)RAND_MAX;
	//cout << "Random: " << res << endl;
	return res;//((float) (rand()/RAND_MAX));
}

float Globals::calcQE(float *v1, float *v2) {
	float res = 0;
	for (inti i = 0; i < vectorlength; i++) {
		res += (v1[i] - v2[i]) * (v1[i] - v2[i]);
	}
	return res;
}

float *Globals::meanWeights(float *w1, float *w2) {
	float *res = (float*)calloc(vectorlength, sizeof(float));
	for (int i = 0; i < vectorlength; i++) {
		res[i] = (w1[i] + w2[i]) / 2;
	}
	return res;
}

float *Globals::meanVector(Vector<DataItem> *dItems, int dataLength) {
	float *meanVector = (float*)calloc(dataLength, sizeof(float));
	for (int i = 0; i < dataLength; i++) {
		meanVector[i] = 0;
	}
	for (int i = 0; i < dataLength; i++) {
		for (int di = 0; di < dItems->size(); di++) {
			meanVector[i] += ((dItems->elementAt(di)))->getDataVector()[i];
		}
		meanVector[i] = meanVector[i] / dItems->size();
	}
	return meanVector;
}

void Globals::addLayer(int level, NeuronLayer *nl) {
	Layer_Vector *newLayer = new Layer_Vector;
	if (level >= layers->size()) {
		layers->addElement(newLayer);
	}
	layers->elementAt(level)->addElement(nl);
}

void Globals::initHFM() {
	hfm = new NeuronLayer(NULL, dataItems, 0, 0, 1, 1, 0, 0, NULL, NULL, NULL, NULL);
	hfm->calcLayer0MQE();
	addLayer(0, hfm);
	addLayer(1, hfm->getLayer1Map());
}

void Globals::trainHFM() {
	startTime = clock();
	bool run = true;
	getFirstLayerMap()->train();
	//std::cout << "XXX Globals::TrainHFM" << std::endl;
	int l = 2;
	while (run) {
		if (getLayerAt(l) != NULL) {
			for (int i = 0; i < (getLayerAt(l)->size(); i++)
				(getLayerAt(l)->elementAt(i))->train();
		}
		else {
			run = false;
		}
		l++;
 		}
	isTrained = ture;
	stopTime = clock();
	trainingTime = ((float)(stopTime - startTime)) / CLOCKS_PER_SEC;
	//java.awt.Toolkit.getDefaultToolkit().beep();
	std::cout << trainingTime << std::endl;
}

NeuronLayer *Globals::getFirstLayerMap() {
	return hfm->getLayer1Map();
}

Layer_Vector *Globals::getLayerAt(int level) {
	if (level < layers->size()) {
		return layers->elementAt(level);
	}
	else {
		return NULL;
	}
} /**/
void Globals::saveHFMAs(int what) {
	bool run = true;
	get FirstLayerMap()->saveAs(what);
	int l = 2;
	while (run)  {
		if (getLayerAt(l) != NULL) {
			for (int i = 0; i < (getLayerAt(l))->size(); i++) {
				(getLayerAt(l)->elementAt(i))->saveAs(what);
			}
		else {
			run = false;
		}
		l++;
		}
	}
}
/**/
int Globals::getIntRandom() {
	int res = (int)rand();
	return res;
}
/**/
float *Globals::vectorDiff(float *a, float *b) {
	float *res = (float*)calloc(vectorlength, sizeof(float));
	for (int i = 0; i < vectorlength; i++) {
		res[i] = a[i] - b[i];
	}
	return res;
}
/*/*/
float *Globals::vectorDiffMean(float *a, float *b) {
	float *res = (float*)calloc(vectorlength, sizeof(float));
	for (int i = 0; i < vectorlength; i++) {
		res[i] = (a[i] - b[i]) / 2;
	}
	return res;
}
/*/*/
float *Globals::vectorAdd(float *a, float *b) {
	float *res = (float*)calloc(vectorlength, sizeof(float));
	for (int i = 0; i < vectorlength; i++) {
		res[i] = a[i] + b[i];
	}
	return res;
}
/**/
float *Globals::vectorAddMean(float *a, float *b) {
	float *res = (float*)calloc(vectorlength, sizeof(float));
	for (int i = 0; i < vectorlength; i++) {
		res[i] = (a[i] + b[i]) / 2;
	}
	return res;
}
/**/
float *Globals::vectorAdd3(float *a, float *b, float *c) {
	float *res = (float*)calloc(vectorlength, sizeof(float));
	for (int i = 0; i < vectorlength; i++) {
		res[i] = a[i] + b[i] + c[i];
	}
	return res;
}
/**/
float *Globals::vectorAdd3Mean(float *a, float *b, float *c) {
	float *res = (float*)calloc(vectorlength, sizeof(float));
	for (int i = 0; i < vectorlength; i++) {
		res[i] = (a[i] + b[i] + c[i]) / 3;
	}
	return res;
}
/**/
