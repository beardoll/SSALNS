#ifndef _SSLANS_H
#define _SSALNS_H
#include "Car.h"
#include "Matrix.h"
#include<vector>

class SSALNS{  // SSALNS�㷨
public:
	SSALNS(vector<Customer*> PR2, vector<Customer*> PR3, vector<Car*> originPlan, Customer depot, float capacity):
		PR2(PR2), PR3(PR3), originPlan(originPlan), depot(depot), capacity(capacity){
	};
	~SSALNS(){};
	void shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float maxd, float maxt, float maxquantity);
	void randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
	void worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, vector<float> DTpara);  
	void greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd, vector<float> DTpara);
	void regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd, vector<float> DTpara);
	void run(vector<Car*> &finalCarSet, float &finalCost);  // �����㷨���൱���㷨��main()����
private:
	vector<Customer*> PR2;     // �ڶ����ȼ��˿�
	vector<Customer*> PR3;     // �������ȼ��˿�
	vector<Car*> originPlan;   // ��ʼ�ƻ�
	Customer depot;
	float capacity;
};

#endif