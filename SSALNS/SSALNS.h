#ifndef _SSLANS_H
#define _SSALNS_H
#include "Car.h"
#include<vector>

class SSALNS{  // SSALNS算法
public:
	SSALNS(vector<Customer*> PR2, vector<Customer*> PR3, vector<Car*> originPlan, Customer depot, float capacity):
		PR2(PR2), PR3(PR3), originPlan(originPlan), capacity(capacity){};
	~SSALNS(){};
	void shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float maxd, float maxt, float maxquantity);
	void randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
	void worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p);  
	void greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd);
	void regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd);
	void reallocateCarIndex(vector<Car*> &originCarSet); // 重新编号车辆
	void removeNullRoute(vector<Car*> &originCarSet);    // 清除空车辆 
	void generateMatrix(vector<int> &allIndex, vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, Matrix<float> &minInsertPerRoute, 
					Matrix<Customer> &minInsertPos, Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, float noiseAmount, bool noiseAdd);
	// 产生removedCustomer于removedCarSet中的最小插入代价矩阵和次小插入代价矩阵
	void updateMatrix(vector<int> restCustomerIndex, Matrix<float> &minInsertPerRoute, Matrix<Customer> &minInsertPos, 
				  Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, int selectedCarPos, vector<Car*> &removedCarSet,
				  vector<Customer*>removedCustomer, float noiseAmount, bool noiseAdd);
	// 插入新节点后，更新其余未插入节点到该路径的最小/次小插入代价
	void computeReducedCost(vector<Car*> originCarSet, vector<int> indexsetInRoute, vector<int> removedIndexset, 
						vector<pair<float, int>> &reducedCost);  // 计算各个节点的移除代价
	float getCost(vector<Car*> originCarSet);  // 得到originCarSet的路径代价
	void run(vector<Car*> &finalCarSet, float &finalCost);  // 运行算法，相当于算法的main()函数
private:
	vector<Customer*> PR2;     // 第二优先级顾客
	vector<Customer*> PR3;     // 第三优先级顾客
	vector<Car*> originPlan;   // 初始计划
	Customer depot;
	float eta0, eta1, eta2, eta3;    // 四类顾客的惩罚系数
	float lambda;              // 计算artificial路径代价时的惩罚系数
	float capacity;
};

#endif