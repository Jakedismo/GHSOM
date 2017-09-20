/*********************************************************************
					neuronlayer.cpp - kuvaus
					-------------------
aloitus				: 22. Tammikuuta 2015
copyright			: (C) 2015 Jokke Ruokolainen
email				: jruokola@student.uef.fi
**********************************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
//#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
//# added mmx
#include <math.h>
#include "neuronlayer.h"

NeuronLayer::NeuronLayer(Neuron *sn, Data_Vector *indataItems, float insuperMQE, int inlevel, int initialSizeX, int initialSizeY, int posX, int posY, float *ULweight, float *URweight, float *LLweight, float *LRweight) {

	dataItems = new Data_Vector();
	dataItems = indataItems;
	dataLength = Globals::vectorlength;
	superMQE = insuperMQE;
	superNeuron = sn;
	superPos = (int*)calloc(2, sizeof(int));
	superPos[0] = posX;
	superPos[1] = posY;
	gid = Globals::HTML_GID;
	Globals::HTML_GID++;
	MQE = MAX_DOUBLE;
	MQENeuron = (int*)calloc(2, sizeof(int));
	stupidity = Globals::TAU_1;
	ini_learnrate = Globals::INITIAL_LEARNRATE;
	learnrate = Globals::INITIAL_LEARNRATE;
	min_learnrate = Globals::MIN_LEARNRATE;
	ini_neighbourhood = Globals::INITIAL_NEIGHBOURHOOD;
	min_neighbourhood = Globals::MIN_NEIGHBOURHOOD;
	level = inlevel;
	x = initialSizeX;
	y = initialSizeY;
	STRETCH_PARAM_LEARN = (Globals::EXPAND_CYCLES*dataItems->size()) / 16;
	STRETCH_PARAM_NEIGHB = (Globals::EXPAND_CYCLES*dataItems->size()) / 6.67;
	currentCycles = 0;
	neuronMap = (Neuron***)calloc(initialSizeX, sizeof(Neuron*));
	for (int i = 0; i < initialSizeX; i++) {
		neuronMap[i] = (Neuron***)calloc(initialSizeY, sizeof(Neuron*));
	}
	if ((Globals::ORIENTATION == 1) && (sn != NULL) && (level>1)) {
		neuronMap[0][0] = new Neuron(ULweight, gid, level, superPos[0], superPos[1]);
		neuronMap[1][0] = new Neuron(URweight, gid, level, superPos[0], superPos[1]);
		neuronMap[0][1] = new Neuron(LLweight, gid, level, superPos[0], superPos[1]);
		neuronMap[1][1] = new Neuron(LRweight, gid, level, superPos[0], superPos[1]);
	}
	else {
		for (int i = 0; i < initialSizeY; i++) {
			for (int j = 0; j < initialSizeX; j++) {
				neuronMap[j][i] = new Neuron(dataLength, gid, level, superPos[0], superPos[1]);
			}
		}
	}
}

NeuronLayer::~NeuronLayer() {
	free(superPos);
	free(MQENeutron);
	for (int j = 0; j < y; j++) {
		for (int i = 0; i < x; i++) {
			delete(neuronMap[i][j]);
		}
	}
	for (int i = 0; i < x; i++) {
		free(neuronMap[i]);
	}
	free(neuronMap);
}
/**  */
/* void NeuronLayer::adaptWeights(int *winner, DataItem *di){
	int xminlim, xmaxlim, yminlim, ymaxlim;
	xminlim = winner[0] - neighbourhood;
	if (xminlim<0) { xminlim = 0; }
		xmaxlim = winner[0] + neighbourhood;
	if (xmaxlim>(x-1)) { xmaxlim = x-1; }
		yminlim = winner[1] - neighbourhood;
	if (yminlim<0) { yminlim = 0; }
		ymaxlim = winner[1] + neighbourhood;
	if (ymaxlim>(y-1)) { ymaxlim = y-1; }
		for (int j=yminlim;j<=ymaxlim;j++) {
			for (int i=xminlim;i<=xmaxlim;i++) {
				neuronMap[i][j]->adaptWeights(di,(sqrt(((winner[0]-i)*(winner[0]-i))
				+((winner[1]-j)*(winner[1]-j)))),
				learnrate,neighbourhood);
		}
	}
}
*/
void NeuronLayer::adaptWeights(int *winner, DataItem *di) {
	for (int j = 0; j < y; j++) {
		for (int i = 0; i < x; i++) {
			neuronMap[i][j]->adaptWeights(di, (sqrt((winner[0] - i)*(winner[0] - i)) + ((winner[1] - j)*(winner[1] - j))), learnrate, neighbourhood);
		}
	}
}
/**/
int *NeuronLayer::getMaxDissNeighbour(int *n) {
	int *res = (int*)calloc(2, sizeof(int));
	float diss = 0;
	if (n[1]>0) { //ylempi
		float currDiss = Globals::calcQE(neuronMap[n[0]][n[1]]->weights, neuronMap[n[0]][n[1] - 1]->weights);
		if (currDiss >= diss) {
			diss = currDiss;
			res[0] = n[0];
			res[1] = n[1] - 1;
		}
	}
	if (n[1] < y - 1) { //alempi
		float currDiss = Globals::calcQE(neuronMap[n[0]][n[1]]->weights, neuronMap[n[0]][n[1] + 1]->weights);
		if (currDiss >= diss) {
			diss = currDiss;
			res[0] = n[0] - 1;
			res[1] = n[1];
		}
	}
	if (n[0] < x - 1) { //oikea
		float currDiss = Globals::calcQE(neuronMap[n[0]][n[1]]->weighs, neuronMap[n[0] + 1][n[1]]->weights);
		if (currDiss >= diss) {
			diss = currDiss;
			res[0] = n[0] + 1;
			res[1] = n[1];
		}
	}
	return res;
}
/**/
void NeuronLayer::insertRow(int pos) {
	y++;
	std::cout << "Lisätään rivi:" << pos << std::endl;
	Neuron ***newMap;
	newMap = (Neuron***)calloc(x, sizeof(NeuroN*));
	for (int i = 0; i < x; i++) {
		newMap[i] = (Neuron**)calloc(y, sizeof(Neuron*));
	}

	for (int i = 0; i < y; i++) {
		for (int j = 0 j < x; j++) {
			if (i < pos) {
				newMap[j][i] = neuronMap[j][i];
			}
			else if (i == pos) {
				//lisätään uusi neuroni jolla on ylemmän/alemman mediaani paino
				newMap[j][i] = new Neuron(Globals::meanWeights(neuronMap[j][i - 1]->weights, neuronMap[j][i]->weights), gid, level, superPos[0], superPos[1]);
			}
			else if (i>pos) {
				newMap[j][i] = neuronMap[j][i - 1];
			}
		}
	}
	for (int i = 0; i < x; i++) {
		free(neuronMap[i]);
	}
	free(neuronMap);
	neuronMap = newMap;
}
/**/
void NeuronLayer::insertColumn(int pos) {
	x++;
	std::cout << "Lisätään kolumni:" << pos << std::endl;
	Neuron ***newMap;
	newMap = (Neuron***)calloc(x, sizeof(Neuron*));
	for (int i = 0; i < x; i++) {
		newMap[i] = (Neuron***)callco(y, sizeof(Neuron*));
	}
	//for (int j=0;j<x;j++) {
	//for int i=0; i<y;i++) {
	//neu
	//}
	//}

	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {
			if (j < pos) {
				newMap[j][i] = neuronMap[j][i];
			}
			else if (j == pos) {
				//lisätään uusi neuroni jolla on vasemman/oikean neuronin mediaani paino
				newMap[j][i] = new Neuron(Globals::meanWeights(neuronMap[j - 1][i]->weights, neuronMap[j][i]->weights), gid, level, superPos[0], superPos[1]);
			}
			else if (j>pos) {
				newMap[j][i] = neuronMap[j - 1][i];
			}
		}
	}
	for (int i = 0; i < (x - 1); i++) {
		free(neuronMap[i]);
	}
	free(neuronMap);
	neuronMap = newMap;
}
/**/
void NeuronLayer::testDataItems() {
	int *winner = (int*)calloc(2, sizeof(int));
	for (int d = 0; d < dataItems->size(); d++) {
		float winnerDist = MAX_DOUBLE;
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < x; j++) {
				float currDist = neuronMap[j][i]->calcDist(dataItems->elementAt(d));
				if (currDist < winnerDist) {
					winnerDist = currDist;
					winner[0] = j;
					winner[1] = i;
				}
			}
		}
		neuronMap[winner[0]][winner[1]]->addRepresentingDataItem(dataItems->elementAt(d));
	}
	free(winner);
}
/**/
void NeuronLayer::calcMQE() {
	MQE = 0;
	int c = 0;
	float maxMQE = 0;
	testDataItems();
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {
			neuronMap[j][i]->calcMQE();
			if (neuronMap[j][i]->MQE > 0) {
				MQE += neuronMap[j][i]->MQE;
				//std::cout << "Neuroni[" << j << "][" << i << "] MQE: " << neuronMap[j][i]->MQE << ", # of items: " << neuronMap[j][i]->representsMultiDataItems() << std::endl;
				c += 1;
			}
			float currMQE = neuronmap[j][i]->MQE;
			if (currMQE > maxMQE) {
				maxMQE = currMQE;
				MQENeuron[0] = j;
				MQENeuron[1] = i;
			}
		}
	}
	MQE = MQE / (float)c;
}
/**/
void NeuronLayer::calcLayerMQE() {
	char dummy[1024];
	float *meanVec;

	if (Globals::MQE0_FILE == NULL) { //Laske MQE0
		std::cout << "Lasektaan MQE0" << std::endl;

		meanVec = Globals::meanVector(dataItems, dataLength);
		/* for (int i=0; i<datalength; i++) {
		printf("%f ",meanVec[i]);
		}*/
		neuronMap[0][0] = new Neutron(meanVec, gid, level, superPos[0], superPos[1]);
		MQE = 0;
		for (int i = 0; i < dataItems->size(); i++) {
			MQE += Globals::calcQE(((dataItems->elementAt(i)))->getDataVector(), neuronMap[0][0]->weights);
		}
		//MQE = MQE / (float)dataItems->size();
		std::cout << "MQE: " << MQE << std:endl;
		neuronMap[0][0]->addMap(dataItems, MQE, 1, Globals::INITIAL_X_SIZE, Globals::INITIAL_Y_SIZE, 0, 0);
	}
	else { //Luetaan MQE0 tiedosto
		FILE *mqe0file;
		std::cout << "Ladataan MQE0 tiedostoa: " << Globals::MQE0_FILE << std::endl;

		if ((mqe0file = fopen(Globals::MQE0_FILE, "r")) == NULL) {
			printf("Ei voida avata MQE0 tiedostoa %s\n", Globals::MQE0_FILE);
			exit(1);
		}
		fscanf(mqe0file, "%s", &dummy);
		if (strcmp(dummy, "$MEAN_VEC") != 0) {
			fclose(mqe0file);
			printf("Ei voida lukea $MAEN_VEC\n");
			exit(1);
		}
		meanVec = (float*)calloc(dataLength, sizeof(float));

		for (int i = 0; i < dataLength; < i++) {
			fscanf(mqe0file, "%f", &meanVec[i]);
		}

		fscanf(mqe0file, "%s", &dummy);
		if (strcmp(dummy, "$MQE0") != 0) {
			fclose(mqe0file);
			printf("ei voida lukea $MQE0\n");
			exit(1);
		}
		fscanf(mqe0file, "%f", &MQE);
		fclose(mqe0file);

		std::cout << "MQE: " << MQE << std::endl;

		neuronMap[0][0] = new Neuron(meanVec, gid, level, superPos[0], superpos[1]);
		neuronMap[0][0]->addMap(dataItems, MQE, 1, Globals::INITIAL_X_SIZE, Globals::INITIAL_Y_SIZE, 0, 0);
	}
}
/**/
NeuronLayer* NeuronLayer::getLayer1Map() {
	return neuronMap[0][0]->getMap();
}
/**/
void NeuronLayer::getNewWeights(int xPos, int yPos, float *UL, float *UR, float *LL, float *LR) {
	float *twgt = neuronMap[xPos][yPos]->weights;
	float *nUL = NULL, *nU = NULL, *nL=NULL,  *nBL = NULL, *nB = NULL, *nBR = NULL, *nUR = NULL, *nR = NULL;
	float *dUL, *dUR, *dLL, *dLR;
	if ((xPos > 0) && (yPos > 0)) {
		nUL = neuronMap[xPos - 1][yPos - 1]->weights;
	}
	if (yPos > 0) {
		nU = neuronMap[xPos][yPos - 1]->weights;
	}
	if (xPos > 0) {
		nL = neuronMap[xPos - 1][yPos]->weights;
	}
	if ((Xpos > 0) && (yPos < y - 1)) {
		nBL = neuronMap[xPos - 1][yPos + 1]->weights;
	}
	if (yPos < y - 1) {
		nB = neuronMap[xPos][yPos + 1]->weights;
	}
	if ((xPos < x - 1) && (yPos < y - 1)) {
		nBR = neuronMap[xPos + 1][yPos + 1]->weights;
	}
	if ((xPos < x - 1) && (yPos > 0))  {
		nUR = neuronMap[xPos + 1][yPos - 1]->weights;
	}
	if (xPos < x - 1) {
		nR = neuronMap[xPos + 1][yPos]->weights;
	}
	if ((xPos == 0) && (yPos == 0)) { //Ylempi L vasen
		dUL = twgt;
		dUR = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nR, twgt)));
		dLL = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nB, twgt)));
		dLR = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nR, twgt), Globals::vectorDiff(nBR, twgt), Globals::vectorDiff(nB, twgt)));
		printf("UL: %f / %f\n", dUL[0], dUL[1]);
		printf("UR: %f / %f\n", dUR[0], dUR[1]);
		printf("LL: %f / %f\n", dLL[0], dLL[1]);
		printf("LR: %f / %f\n", dLR[0], dLR[1]);
	}
	else if ((xPos == x - 1) && (yPos == 0)) { //Ylempi R oikea
		dUL = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nL, nwgt)));
		dUR = twgt;
		dLL = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nL, twgt), Globals::vectorDiff(nBl, twgt), Globals::vectorDiff(nB, twgt)));
		dLr = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nB, twgt)));
	}
	else if ((xPos == 0) && (yPos == y - 1)) { //Alempi L vasen
		dUL = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nU, twgt)));
		dUR = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nU, twgt), Globals::vectorDiff(nUR, twgt), Globals::vectorDiff(nR, twgt)));
		dLL = twgt;
		dLR = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nR, twgt)));
	}
	else if ((xPos == X - 1) && (yPos == y - 1)) { //Alempi R oikea
		dUL = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nUL, twgt), Globals::vectorDiff(nU, twgt), Globals::vectorDiff(nL, twgt)));
		dUR = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nU, twgt)));
		dLL = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nL, twgt)));
		dLR = twgt;
	}
	else if ((yPos == 0) && (xPos > 0)) { //Ylempi B
		dUL = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nL, twgt)));
		dUR = Globals:vectorAdd(twgt, (Globals::vectorDiffMean(nR, twgt)));
		dLL = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nL, twgt), Globals::vectorDiff(nBL, twgt), Globals::vectorDiff(nB, twgt)));
		dLR = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nR, twgt), Globals::vectorDiff(nBR, twgt), Globals::vectorDiff(nB, twgt)));
	}
	else if ((xPos == x - 1) && (yPos > 0) && (yPos < y - 1)) { //Oikea b
		dUL = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nL, twgt), Globals::vectorDiff(nUL, twgt), Globals::vectorDiff(nU, twgt)));
		dUR = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nU, twgt)));
		dLL = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nL, twgt), Globals::vectorDiff(nBL, twgt), Globals::vectorDiff(nB, twgt)));
		dLR = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nL, twgt)));
	}
	else if ((yPos == y - 1) && (xPos>0) && (xPos<x - 1)) { // Alempi B
		dUL = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nL, twgt), Globals::vectorDiff(nUL, twgt), Globals::vectorDiff(nU, twgt)));
		dUR = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nR, twgt), Globals::vectorDiff(nUR, twgt), Globals::vectorDiff(nU, twgt)));
		dLL = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nL, twgt)));
		dLR = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nR, twgt)));
	}
	else if ((xPos == 0) && (yPos>0) && (yPos<y - 1)) { // Vasen B
		dUL = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nU, twgt)));
		dUR = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nR, twgt), Globals::vectorDiff(nUR, twgt), Globals::vectorDiff(nU, twgt)));
		dLL = Globals::vectorAdd(twgt, (Globals::vectorDiffMean(nB, twgt)));
		dLR = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nR, twgt), Globals::vectorDiff(nBR, twgt), Globals::vectorDiff(nB, twgt)));
	}
	else if ((xPos>0) && (xPos<x - 1) && (yPos>0) && (yPos<y - 1)) { // M
		dUL = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nL, twgt), Globals::vectorDiff(nUL, twgt), Globals::vectorDiff(nU, twgt)));
		dUR = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nR, twgt), Globals::vectorDiff(nBR, twgt), Globals::vectorDiff(nB, twgt)));
		dLL = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nL, twgt), Globals::vectorDiff(nBL, twgt), Globals::vectorDiff(nB, twgt)));
		dLR = Globals::vectorAdd(twgt, Globals::vectorAdd3Mean(Globals::vectorDiff(nR, twgt), Globals::vectorDiff(nBR, twgt), Globals::vectorDiff(nB, twgt)));
	}
	for (int k = 0; k < Globals::vectorlength; k++) {
		UL[k] = dUL[k];
		UR[k] = dUR[k];
		LL[k] = dLL[k];
		LR[k] = dLR[k];
	} 
}
/**/
void NeuronLayer::train() {
	/*MQE viettää piirtämisessä*/
	char * mqename = new char[256];
	sprintf(mqename, "%s/%s_%i_%i_%i.mqe", Globals::savePath, Globals::HTML_PREFIX, gid, level, superPos[0], superPos[1]);
	std::ofstream *mqeFile;
	if (Globals::printMQE) {
		mqeFil = new std::ofstream(mqename);
		if (!mqeFile) {
			std::cout << "MQE tiedostoa ei voitu avata" << std:endl;
			exit(EXIT_FAILURE);
		}
	}
	/*MQE piirto loppuu tähän*/

	stupidity = Globals::TAU_1;
	ini_learnrate = Globals::INITIAL_LEARNRATE;
	learnrate = Globals::INITIAL_LEARNRATE;
	//neighbourhood = Globals::INTIAL_NEIGBOURHOOD;
	//neighbourhood_falloff = GLOBALS::NEIGHBOURHOOD_FALLOFF;
	min_learnrate = Globals::MIN_LEARNRATE;
	min_neighbourhood = Globals::MIN_NEIGHBOURHOOD;
	currentCycle = 0;

	std::cout << "XXX Neuronikerros: train1" << std::endl;

	/*Uusi neighbourhood*/
	ini_neighbourhood = sqrt(((float)(x > y ? x : y)) / (2.0*sqrt(-1.0*log(Globals::NR))));
	neighbourhood = ini_neighbourhood;

	std::cout << "XXX neuronikerros: train2" << std::endl;

	bool run = true;
	while (run) {
		std::cout << "XXX neuronikerros: train3" << std::endl;
		currentCycle += 1;
		std::cout << "DEBUGGAUS" << std::endl;
		if (((currentCycle) % dataItems->size()) == 0) {
			std::cout << ".";
			std::cout.flush();
		}
		//Haetaan seuraava kuvio
		dataItem *currentDataItem = dataItems->elementAt(Globals::getIntRandom() % dataItems->size());

		//lasketaan aktiivisuus ja saadaan tuloksena voittaja etäisyys
		float winnerDist = MAX_DOUBLE;
		int *winner = (int*)calloc(2, sizeof(int));
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < x; j++) {
				float currDist = neuronMap[j][i]->calcDist(currentDataItem);
				if (currDist < winnerDist) {
					winnerDist = currDist;
					winner[0] = j;
				}
			}
		}
		//Muutetaan painoja vastaamaan voittajaa ja naapureita
		std::cout << "XXX neuronikerros: train4" << std::endl;
		adaptWeights(winner, currentDataItem);
		std::cout << "XXX neuronikerro:S train5" << std::endl;
		free(winner);

		//lasketaan oppimisnopeutta
		std::cout << "XXX asetetaan alunoppimisnopeus: " << learnrate << std::endl;
		learnrate = (ini_learnrate*exp(-1.0*((float)((currentCycle) % (Globals::EXPAND_CYCLES*dataItems->size()))) / STRETCH_PARAM_LEARN)) + 0.0001;
		std::cout << "\nOppimisnopeus.:" << learnrate;

		/*if ((currentCycle % dataItems->size()) == 0) {
		learnrate = (learnrate * learnrate) + min_learnrate;
		} */
		// reduce neighbourhood (not yet implemented)
		/** alte methode */
		//if (currentCycle % ((int)((float)(Globals::EXPAND_CYCLES*dataItems->size()) / (float)(ini_neighbourhood+1))) == 0) {
		//  neighbourhood -=1;
		//  if (neighbourhood < 0) { neighbourhood = 0; }
		//  //      std::cout << "\nNeigh.:" << neighbourhood << std::endl;
		//}
		/**Uusi metodi */
		neighbourhood = (ini_neighbourhood * exp(-1.0*((float)((currentCycle) % (Globals::EXPAND_CYCLES * dataItems->size()))) / STRETCH_PARAM_NEIGHB)) + 0.55;

		//MQE kartan piirtoa varten
		if (Globals::printMQE) {
			//if currentCycle % 10 == 0) {
			for (int i = 0; i < y; i++) {
				for (j = 0; j < x; j++) {
					neuronMap[j][i]->clearRepresentingDataItems();
				}
			}
			calcMQE();
			*mqeFile << MQE << ',' << learnrate << ',' << neigbhourhood << "\r\n";
			//}
		}
		/**/

		/*Jos expand cycle saavutetaan*/
		if (((currentCycle) % (Globals::EXPAND_CYCLES*dataItems->size())) == 0) {
			/*Jos ei tarvitse piirtää*/
			if (!Globals::printMQE) {
				for (int i = 0; i < y; i++) {
					for (int j = 0; j < x; j++) {
						neuronMap[j][i]->clearRepresentingDataItems();
					}
				}
				calcMQE();
			}
			/**/
			std::cout << "MQE ; " << MQE << ", to go :" << stupidity*superMQE << std::endl;
			if ((MQE <= (stupidity*superMQE)) || ((Globals::MAX_CYCLES > 0) && (currentCycle >= (Globals::MAX_CYCLES*dataItems->size())))) {
				run = false;
			}
			else {
				std::cout << "Neuroni jolla on max MQE :" << MQENeuron[0] << "," << MQENeuron[1] << std::endl;
				int *dissNeighbour = getMaxDissNeighbour(MQENeuron);
				if (MQENeuron[0] > dissNeighbour[0]) { //vasen
					insertColumn(MQENeuron[0]);
				}
				else if (MQENeuron[0] < dissNeighbour[0]) {//oikea 
					insertColumn(dissNeighbour[0]);
				}
				else if (MQENeuron[1] > dissNeighbour[1]) { //ylä
					insertRow(MQENeuron[1]);
				}
				else if (MQENeuron[1] < dissNeighbour[1]) { //ala
					insertRow(dissNeighbour[1]);
				}
				//Asetetaan oppimisnopeus alkuperäisarvoon
				std::cout << "\nLearnrate ennen: " << learnrate;
				learnrate = ini_learnrate;
				std::cout << "\nLearnrate nyt: " << learnrate;
				//Aseta neighbourhood

				ini_neighbourhood = sqrt(((float)((x>y) ? x : y)) / (2.0*sqrt(-1.0*log(Globals::NR))));
				neigbourhood = ini_neighbourhood;
				std::cout << "XXX setting learnrate: " << learnrate << std::endl;
				std::cout << "XXX setting neighbourhood: " << neighbourhood << std::endl;

				free(dissNeighbourhood);
			}
		}
		//valmis
		std::cout << "MQE: " << MQE << std::endl;

		//TAU_2 on laajenemisen kynnysarvo
		bool r = true;
		bool exp = true;
		if (level > 1) {
			int ii = 0;
			while ((ii < y) && r) {
				int jj = 0;
				while ((jj<x) && r) {
					if (superNeuron->representsMultiDataItems() == neuronMap[jj][ii]->representsMultiDataItems()) {
						exp = false;
						r = false;
					}
					else if (superNeuron->representsMultiDataItems() > neuronMap[jj][ii]->representsMultiDataItems()) {
						exp = true;
						r = false;
					}
					j++;
				}
				i++;
			}
		}
		if (exp) {
			std::cout << "DEBUG: supMQE*TAU_2: " << (superMQE*Global::TAU_2) << std::endl;
			for (int i = 0; i < y; i++) {
				for (j = 0; j < x; j++) {
					std::cout << "Debug neuronMap[][].MQE: " << neuronMap[j][i]->MQE << std::endl;
					if ((Globals::getFirstLayerMap()->superMQE*Globals::TAU_2) < neuronMap[j][i]->MQE) {
						if (Globals::ORIENTATION) {
							//float *UL=NULL,*UR=NULL,*LL=NULL,*LR=NULL;
							float *UL = (float*)calloc(Globals::vectorlength, sizeof(float));
							float *UR = (float*)calloc(Globals::vectorlength, sizeof(float));
							float *LL = (float*)calloc(Globals::vectorlength, sizeof(float));
							float *LR = (float*)calloc(Globals::vectorlength, sizeof(float));

							getNewWeights(j, i, UL, UR, LL, LR);

							neuronMap[j][i]->addMap(neuronMap[j][i], neuronMap[j][i]->MQE, level + 1, j, i, UL, UR, LL, LR);
						}
						else {
							neuronMap[j][i]->addMap(neuronMap[j][i], neuronMap[j][i]->MQE, level + 1, Globals::INITIAL_X_SIZE, Globals::INITIAL_Y_SIZE, j, i);
						}

						Globals::addLayer(level + 1, neuronMap[j][i]->getMap());
					}
				}
			}
		}
		if (Globals::printMQE) {
			mqeFile->close();
		}
	}
	/**/
	void NueronLayer::saveAs(int what) {
		char * fname = new char[256];
		sprintf(fname, "%s/%s_%i_%i_%i_%i.html", Globals::savePath, Globals::HTML_PREFIX, gid, level, superPos[0], superPos[1]);
		std::cout << "Tallennetaan" << fname << std::endl;
		std::ofsream htmlFile(fname);
		if (!htmlFile) {
			std::cout << "HTML Tiedostoa ei voitu avata" << std::endl;
			exit(EXIT_FAILURE);
		}
		htmlFile << "<HTML><BODY bgcolor=#FFFFFF text=#000000><TITLE>Layer " << level << "Map</TITLE>\n";
		htmlFile << "<H1>Layer " << level << " Map</H1><BR>\n";
		if (superNeuron != NULL) {
			htmlFile << "<a href=\"" << Globals::HTML_PREFIX << "_" << superNeuron->mapgid << "_" << superNeuron->maplevel << "_" << superNeuron->mapspx << "_" << superNeuron->mapspy << ".html\">Yksi taso ylöspäin</a><BR></BR>\n";
		}
		htmlFile << "TAU_1: " << (stupidity*superMQE) << "<BR>\n";
		htmlFile << "TAU_2: " << (Globals::getFirstLayerMap()->superMQE*Globals::TAU_2) << "<BR>\n";
		htmlFile << "\n<BR><table border=4 cellpadding=4>\n";
		for (int i = 0; i < y; i++) {
			htmlFile << "<tr>\n";
			for (int j = 0; j < x; j++) {
				htmlFile << "<td allign=left>\n";

				if (neuronMap[j][i]->representingMultiDataItems()>0) {
					if (neuronMap[j][i]->getMap() != NULL) {//laajennettu solmu
						htmlFile << "QE: " << neuronMap[j][i]->MQE << "<BR>";
						if (Globals::LABELS:NUM>0) {//Kirjoita labelit
							String_Vector *labels = neuronMap[j][i]->getLabels(Globals::LABELS_NUM);
							for (int label = 0; label < labels->size(); label++) {
								htmlFile << labels->elementAt(label) << "<BR>";
							}
							htmlFile << "<HR>";
							free(labels);
						}
						if ((Globals::LABELS_ONLY) && (Globals::LABELS_NUM>0)) {
							htmlFile << "<a href=" << Globals_HTML_PREFIX << "_" << neuronMap[j][i]->getMap()->gid << "_" << (level + 1) << "_" << j << "_" << i << "_" << ".html" << "down</a><br>";
						}
						else {
							for (int di = 0; di < neuronMap[j][i]->representsMultiDataItems(); di++) {
								htmlFile << "<a href =" << Globals::HTML_PREFIX << "_" << neuronmap[j][i]->getMap()->gid << "_" << (level + 1) << "_" << j << "_" << i << ".html>" << (neuronMap[j][i]->getRepresentingDataItems()->elementAt(di))->getId() << "</a><br>";
							}
						}
					}
					else { //loppusolmu
						htmlFile << "QE: " << neuronMap[j][i]->MQE << "<BR>";
						if (Globals::LABELS_NUM>0)  { //Kirjoitetaan labelit 
							String_Vector *labels = neuronMap[j][i]->getLabels(Globals::LABELS_NUM);
							for (int label = 0; label < labels->size(); label++) {
								htmlFile << labels->elementAt(label) << "<BR>";
							}
							htmlFile << "<HR>";
							free(labels);
						} //kirjoitus lobbuu
						for (int di = 0; di < neuronMap[j][i]->representsMultiDataItems(); di++) {
							htmlFile << "<a href=files" << "/" << (neuronMap[j][i]->getRepresentingDataItems()->elemenAt(di))->getId() << Globals::DATAFILE_EXTENSION << ">" << (neuronMap[j][i]->getRepresentingDataItems()->elementAt(di))->getId() << "</a><br>";
						}
					}
				}
				else {
					htmlFile << ".";
				}
				htmlFile << "</td>\n";
			}
			htmlFile << "</tr>\n";
		}
		htmlFile << "</table></BODY></HTML>\n";
		htmlFile.close();
		free(fname);
	}
	/*/*/
	void NeuronLayer::saveAsSOMLib(){
		char * fname = new char[256];
		sprintf(fname, "%s/%s_%i_%i_%i_%i.mapdescr", Globals::savePath, Globals::HTML_PREFIX, gid, level,
			superPos[0], superPos[1]);
		std::cout << "saving " << fname << std::endl;
		std::ofstream mapFile(fname);
		if (!mapFile) {
			std::cout << "SOMLib map file could not be opened" << std::endl;
			exit(EXIT_FAILURE);
		}
		//struct tm *now = (struct tm*)malloc(sizeof(struct tm));
		time_t now = time(NULL);
		mapFile.precision(10);
		mapFile << "#SOM Map Decription File\n#created by ghsom " << VERSION << " (Growing Hierarchical Self-Organizing Map)\n#Michael Dittenbach\n#\n";
		mapFile << "$TYPE rect\n";
		mapFile << "$XDIM " << x << "\n";
		mapFile << "$YDIM " << y << "\n";
		mapFile << "$VEC_DIM " << dataLength << "\n";
		mapFile << "$STORAGE_DATE " << ctime(&now);
		mapFile << "$TRAINING_TIME " << Globals::trainingTime << "\n";
		mapFile << "$LEARNRATE_TYPE ghsom-specific (sawtooth)\n";
		mapFile << "$LEARNRATE_INIT " << ini_learnrate << "\n";
		mapFile << "$NEIGHBORHOOD_TYPE ghsom-specific (sawtooth)\n";
		mapFile << "$NEIGHBORHOOD_INIT " << ini_neighbourhood << "\n";
		mapFile << "$RAND_INIT " << Globals::randomSeed << "\n";
		mapFile << "$ITERATIONS_TOTAL " << currentCycle << "\n";
		mapFile << "$NR_TRAINVEC_TOTAL " << dataItems->size() << "\n";
		mapFile << "$VEC_NORMALIZED 1\n";
		mapFile << "$URL_TRAINING_VEC " << Globals::inputFile << "\n";
		mapFile << "$URL_WEIGHT_VEC " << Globals::savePath << "/" << Globals::HTML_PREFIX <<
			"_" << gid << "_" << level << "_" << superPos[0] << "_" << superPos[1] << ".wgt" << "\n";
		mapFile << "$URL_MAPPED_INPUT_VEC " << Globals::inputFile << "\n";
		mapFile << "$URL_NODE_DESCR " << Globals::savePath << "/" << Globals::HTML_PREFIX <<
			"_" << gid << "_" << level << "_" << superPos[0] << "_" << superPos[1] << ".node" << "\n";
		mapFile << "$DESCRIPTION NONE\n";
		mapFile << "$GHSOM_EXPAND_CYCLES " << Globals::EXPAND_CYCLES << "\n";
		mapFile << "$GHSOM_MAX_CYCLES " << Globals::MAX_CYCLES << "\n";
		//	mapFile << "$MBACH_MIN_LEARNRATE " << Globals::MIN_LEARNRATE << "\n";
		//	mapFile << "$MBACH_MAX_REPRESENTING " << Globals::MAX_REPRESENTING << "\n";
		mapFile << "$GHSOM_TAU_1 " << Globals::TAU_1 << "\n";
		mapFile << "$GHSOM_TAU_2 " << Globals::TAU_2 << "\n";
		mapFile.close();

		char * fname2 = new char[256];
		sprintf(fname2, "%s/%s_%i_%i_%i_%i.wgt", Globals::savePath, Globals::HTML_PREFIX, gid, level,
			superPos[0], superPos[1]);
		std::cout << "saving " << fname2 << std::endl;
		std::ofstream wgtFile(fname2);
		if (!wgtFile) {
			std::cout << "SOMLib wgt file could not be opened" << std::endl;
			exit(EXIT_FAILURE);
		}
		wgtFile.precision(10);
		wgtFile << "#SOM Weight Vector File\n#created by ghsom " << VERSION << " (Growing Hierarchical Self-Organizing Map)\n#Michael Dittenbach\n#\n";
		wgtFile << "$TYPE rect\n";
		wgtFile << "$XDIM " << x << "\n";
		wgtFile << "$YDIM " << y << "\n";
		wgtFile << "$VEC_DIM " << dataLength << "\n";
		for (int yy = 0; yy<y; yy++) {
			for (int xx = 0; xx<x; xx++) {
				for (int n = 0; n<dataLength; n++) {
					wgtFile << neuronMap[xx][yy]->weights[n] << " ";
				}
				wgtFile << gid << "_" << level << "_" << superPos[0] <<
					"/" << superPos[1] << "_" << xx << "/" << yy << "\n";
			}
		}
		wgtFile.close();

		char * fname3 = new char[256];
		sprintf(fname3, "%s/%s_%i_%i_%i_%i.unit", Globals::savePath, Globals::HTML_PREFIX, gid, level,
			superPos[0], superPos[1]);
		std::cout << "saving " << fname3 << std::endl;
		std::ofstream nodeFile(fname3);
		if (!nodeFile) {
			std::cout << "SOMLib node file could not be opened" << std::endl;
			exit(EXIT_FAILURE);
		}
		nodeFile.precision(10);
		nodeFile << "#SOM Node Description File\n#created by ghsom" << VERSION << " (Growing Hierarchical Self-Organizing Map)\n#Michael Dittenbach\n#\n";
		nodeFile << "$TYPE rect\n";
		nodeFile << "$XDIM " << x << "\n";
		nodeFile << "$YDIM " << y << "\n";
		for (int yy = 0; yy<y; yy++) {
			for (int xx = 0; xx<x; xx++) {
				nodeFile << "$POS_X " << xx << "\n";
				nodeFile << "$POS_Y " << yy << "\n";
				nodeFile << "$NODE_ID " << gid << "_" << level << "_" << superPos[0] << "/" <<
					superPos[1] << "_" << xx << "/" << yy << "\n";
				nodeFile << "$QUANTERROR_NODE " << neuronMap[xx][yy]->MQE << "\n";
				nodeFile << "$NR_VEC_MAPPED " << neuronMap[xx][yy]->representsMultiDataItems() << "\n";
				nodeFile << "$MAPPED_VECS ";
				for (int n = 0; n<neuronMap[xx][yy]->representsMultiDataItems(); n++) {
					nodeFile << (neuronMap[xx][yy]->getRepresentingDataItems()->elementAt(n))->getId() <<
						" ";
				}
				nodeFile << "\n";
				if (neuronMap[xx][yy]->getMap() != NULL) {
					nodeFile << "$NR_SOMS_MAPPED 1\n";
					nodeFile << "$URL_MAPPED_SOMS " << Globals::savePath << "/" << Globals::HTML_PREFIX
						<< "_" << neuronMap[xx][yy]->getMap()->gid << "_" << (level + 1) << "_"
						<< xx << "_" << yy << ".map\n";
				}
				else {
					nodeFile << "$NR_SOMS_MAPPED 0\n";
				}
				// inserted by mmx
				if (Globals::LABELS_NUM>0) { // Labels schreiben					
					String_Vector *labels = neuronMap[xx][yy]->getLabels(Globals::LABELS_NUM);
					nodeFile << "$LABELS ";
					for (int label = 0; label<labels->size(); label++) {
						nodeFile << labels->elementAt(label) << " ";
					}
					nodeFile << "\n";
					free(labels);
				} // Labels Ende
				// inserted by mmx - ende
			}
		}
		nodeFile.close();
		free(fname);
		free(fname2);
		free(fname3);
	}