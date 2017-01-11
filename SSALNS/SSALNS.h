#ifndef _SSLANS_H
#define _SSALNS_H
#include "Car.h"
#include<vector>

class SSALNS{  // SSALNS�㷨
public:
	SSALNS(vector<Customer*> PR2, vector<Customer*> PR3, vector<Car*> originPlan, Customer depot, float capacity):
		PR2(PR2), PR3(PR3), originPlan(originPlan), capacity(capacity){};
	~SSALNS(){};
	void shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float maxd, float maxt, float maxquantity);
	void randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
	void worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p);  
	void greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd);
	void regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd);
	void reallocateCarIndex(vector<Car*> &originCarSet); // ���±�ų���
	void removeNullRoute(vector<Car*> &originCarSet);    // ����ճ��� 
	void generateMatrix(vector<int> &allIndex, vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, Matrix<float> &minInsertPerRoute, 
					Matrix<Customer> &minInsertPos, Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, float noiseAmount, bool noiseAdd);
	// ����removedCustomer��removedCarSet�е���С������۾���ʹ�С������۾���
	void updateMatrix(vector<int> restCustomerIndex, Matrix<float> &minInsertPerRoute, Matrix<Customer> &minInsertPos, 
				  Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, int selectedCarPos, vector<Car*> &removedCarSet,
				  vector<Customer*>removedCustomer, float noiseAmount, bool noiseAdd);
	// �����½ڵ�󣬸�������δ����ڵ㵽��·������С/��С�������
	void computeReducedCost(vector<Car*> originCarSet, vector<int> indexsetInRoute, vector<int> removedIndexset, 
						vector<pair<float, int>> &reducedCost);  // ��������ڵ���Ƴ�����
	float getCost(vector<Car*> originCarSet);  // �õ�originCarSet��·������
	void run(vector<Car*> &finalCarSet, float &finalCost);  // �����㷨���൱���㷨��main()����
private:
	vector<Customer*> PR2;     // �ڶ����ȼ��˿�
	vector<Customer*> PR3;     // �������ȼ��˿�
	vector<Car*> originPlan;   // ��ʼ�ƻ�
	Customer depot;
	float eta0, eta1, eta2, eta3;    // ����˿͵ĳͷ�ϵ��
	float lambda;              // ����artificial·������ʱ�ĳͷ�ϵ��
	float capacity;
};

#endif