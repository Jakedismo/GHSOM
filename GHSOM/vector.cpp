/*********************************************************************
					vector.cpp kuvaus
					-------------------
aloitus				: 22. Tammikuuta 2015
copyright			: (C) 2015 Jokke Ruokolainen
email				: jruokola@student.uef.fi
**********************************************************************/

#include <cstdlib>
#include "vector.h"
#include "dataitem.h"
#include "neuronlayer.h"

class DataItem;
class NeuronLayer;

template <class T>
Vector<T>::Vector() {
	vectorSize = 0;
	vectorArray = NULL;
}

template <class T>
Vector<T>::~Vector() {
	removeAllElements();
}
/**/
template <class T>
void Vector<T>::addElement(T *obj) {
	vectorSize++;
	T **newArray = (T**)realloc(vectorArray, vectorSize*sizeof(T*));
	newArray[vectorSize - 1] = obj;
	vectorArray = newArray;
	//delete obj;
}

template <class T>
T *Vector<T>::elementAt(int pos) {
	return vectorArray[pos]
}
/**/
template <class T>
int vector<T>::size() {
	return vectorSize();
}
/**/
template <class T>
void vector<T>::removeAllElements() {
	free(vectorArray);
	vectorArray = NULL;
	vectorSize = 0;
}
