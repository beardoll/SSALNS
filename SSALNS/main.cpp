#include<iostream>
#include<set>
#include<algorithm> 
#include<map>
#include<vector>
#include "LoadFile.h"
#include "SSALNS.h"
#include "OutputFile.h"

using namespace std;

static const string FILE_PATH = "C:/Users/cfinsbear/Documents/SSALNS/SSALNS/solomon-1987-rc1/";
static const string FILE_NAME = "RC105_100.xml";
static const string FILE_PATH2 = "C:/Users/cfinsbear/Documents/SSALNS/SSALNS/ALNS_Result/";
static const string FILE_NAME2 = FILE_NAME;
static const int NUM_OF_CUSTOMER = 100;

Car* constructCar(vector<int> idSet, vector<Customer*> allCustomer, Customer startPoint,  Customer endPoint, int index, float capacity) {
	// 利用idSet指定的顾客（按顺序）构造路径
	Car* newCar = new Car(startPoint, endPoint, capacity, index, false);
	vector<int>::iterator intIter;
	for(intIter = idSet.begin(); intIter < idSet.end(); intIter++) {
		allCustomer[*intIter - 1]->priority = 1;
		newCar->getRoute().insertAtRear(*allCustomer[*intIter - 1]);	
	}
	return newCar;
}

vector<Customer*> constructCustomerSet(vector<int> idSet, vector<Customer*> allCustomer, int priority){
	// 利用idSet指定的顾客生成顾客集
	vector<Customer*> customerSet;
	customerSet.reserve(idSet.size());
	vector<int>::iterator intIter;
	for(intIter = idSet.begin(); intIter < idSet.end(); intIter++) {
		Customer* newCustomer = new Customer;
		*newCustomer = *allCustomer[*intIter - 1];
		newCustomer->priority = priority;
		customerSet.push_back(newCustomer);
	}
	return customerSet;
}

void constructScenario(vector<Customer*> allCustomer, Customer depot, float capacity, vector<Car*> &initialPlan, vector<Customer*> &PR2, vector<Customer*> &PR3) {
	//int a1[4] = {61, 39, 37, 70};
	//int a2[5] = {54, 44, 41, 71, 93};
	//int a3[4] = {92, 62, 67, 80};
	//int a4[5] = {2, 8, 6, 4, 100};
	//int b[8] = {57, 23, 18, 22, 74, 82, 75, 58};
	//int c[8] = {65, 87, 84, 33, 27, 28, 26, 34};
	int a1[5] = {39, 36, 38, 40, 70};
	int a2[5] = {92, 62, 28, 50, 93};
	int a3[4] = {82, 10, 13, 17};
	int a4[3] = {71, 84, 66};
	int b[7] = {45, 3, 8, 7, 46, 8, 83};
	int c[12] = {23, 49, 20, 53, 12, 14, 15, 16, 47, 78, 64, 86};
	initialPlan.reserve(4);
	vector<int> temp1(a1, a1+5);
	Car* c1 = constructCar(temp1, allCustomer, depot,  depot, 1, capacity);
	vector<int> temp2(a2, a2+5);
	Car* c2 = constructCar(temp2, allCustomer, depot,  depot, 2, capacity);
	vector<int> temp3(a3, a3+4);
	Car* c3 = constructCar(temp3, allCustomer, depot,  depot, 3, capacity);
	vector<int> temp4(a4, a4+3);
	Car* c4 = constructCar(temp4, allCustomer, depot,  depot, 4, capacity);
	initialPlan.push_back(c1);
	initialPlan.push_back(c2);
	initialPlan.push_back(c3);
	initialPlan.push_back(c4);
	PR2.reserve(7);
	vector<int> temp5(b, b+7);
	PR2 = constructCustomerSet(temp5, allCustomer, 2);
	PR3.reserve(12);
	vector<int> temp6(c, c+12);
	PR3 = constructCustomerSet(temp6, allCustomer, 3);
}

int main(){
	LoadFile lf(FILE_PATH, FILE_NAME, NUM_OF_CUSTOMER);
	vector<Customer*> allCustomer(0);
	Customer depot;
	float capacity = 0;
	lf.getData(allCustomer, depot, capacity);

	// 初始化
	vector<Car*> initialPlan;
	vector<Customer*> PR2;
	vector<Customer*> PR3;
	constructScenario(allCustomer, depot, capacity, initialPlan, PR2, PR3);
	SSALNS alg = SSALNS(PR2, PR3, initialPlan, depot, capacity);
	vector<Car*> finalCarSet;
	float finalCost=0;
	alg.run(finalCarSet, finalCost);
	OutputFile of(finalCarSet, FILE_PATH2, FILE_NAME2, depot,finalCost);
	of.exportData();
	system("pause");
	return 0;
}