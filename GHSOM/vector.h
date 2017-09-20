/*********************************************************************
					vector.h - kuvaus
					-------------------
aloitus				: 22. Tammikuuta 2015
copyright			: (C) 2015 Jokke Ruokolainen
email				: jruokola@student.uef.fi
**********************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include "dataitem.h"
//#include "neuronlayer.h"

class NeuronLayer;
class DataItem;

template <class T>
class Vector {
public:
	Vector();
	~Vector();
	/**/
	void addElement(T *obj);
public: //public attributes
	/**/
	T *elementAt(int pos);
	/**/
	int size();
	/**/
	void removeAllElements();
private: //Private attributes
	/**/
	int vectorSize;
	/**/
	T** vectorArray;
};

template class Vector<char>;
template class Vector<float>;
template class Vector<DataItem>;
template class Vector<NeuronLayer>;
template class Vector<Vector<NeuronLayer> >;

#endif