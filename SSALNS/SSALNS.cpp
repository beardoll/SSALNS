#include "SSALNS.h"
#include "Matrix.h"
#include "Car.h"
#include<iostream>
#include<limits>
#include<map>
#include<cmath>
#include<ctime>
#include<cstdlib>
#include<algorithm>
#include<sstream>
#include<string>
#include<functional>
#include<numeric>
#include<cstddef>

using namespace std;

const float MAX_FLOAT = numeric_limits<float>::max();
const float LARGE_FLOAT = 10000.0f;

double random(double start, double end){
	return start+(end-start)*rand()/(RAND_MAX+1.0);
}

bool ascendSort(pair<float, int> x, pair<float, int> y){   // ��������
	return x.first < y.first;
}

bool ascendSort2(pair<float, pair<int, int>> x, pair<float, pair<int, int>> y){  // ��������
	return x.first < y.first;
}

bool descendSort2(pair<float, pair<int, int>> x, pair<float, pair<int, int>> y){  // �ݼ�����
	return x.first > y.first;
}

void getAllCustomerInOrder(vector<Car*> originCarSet, vector<int> &customerNum, vector<Customer*> &allCustomerInOrder){
	// ��ȡCar���������еĹ˿�
	// customerNum: ����������Ĺ˿�����
	// allCustomer: ���еĹ˿ͽڵ�
	int i=0;
	for(vector<Car*>::iterator it1 = originCarSet.begin(); it1 < originCarSet.end(); it1++){
		Route tempRoute = (*it1)->getRoute();
		if(i==0){  // ���customerNum��û��Ԫ�أ�����Ҫ�ۼ�
			customerNum.push_back(tempRoute.getSize()); 
		}else{     // ���������Ҫ�ۼ�
			customerNum.push_back(tempRoute.getSize()+customerNum[i-1]);
		}
		i++;
		vector<Customer*> customerSet = tempRoute.getAllCustomer(); // ÿ������������Ĺ˿�
		for(vector<Customer*>::iterator it2=customerSet.begin(); it2<customerSet.end(); it2++){
			allCustomerInOrder.push_back(*it2);   // ����˿ͽڵ����
			//???? �Ƿ�Ḵ��customer?���������Ҫdelete��customerSet
		}
	}
}

void deleteCustomer(vector<int> removedIndexset, vector<int> customerNum, vector<Customer*> allCustomerInOrder, 
					vector<Car*> &originCarSet, vector<Customer*> &removedCustomer){
	// ���ڽ�removedIndexset��Ӧ��customer���ó���
	// ͬʱ������car��·�����и��ģ��Ƴ��ڵ㣩
	int indexRemovedLen = removedIndexset.end() - removedIndexset.begin();
	for(int i=0; i<indexRemovedLen; i++){
		int carIndex;
		int currentIndex = removedIndexset[i];
		vector<int>::iterator iter;
		iter = upper_bound(customerNum.begin(), customerNum.end(), currentIndex);  // ��һ����currentIndex���Ԫ��
		carIndex = iter - customerNum.begin();
		originCarSet[carIndex]->getRoute().deleteNode(*allCustomerInOrder[currentIndex]);
		Customer *temp = new Customer;
		*temp = *allCustomerInOrder[currentIndex];
		removedCustomer.push_back(temp);
	}
}

void computeReducedCost(vector<Car*> originCarSet, vector<int> indexsetInRoute, vector<int> removedIndexset, 
						vector<pair<float, int>> &reducedCost, vector<float> DTpara){
	// indexsetInRoute: ����·���еĽڵ���
	// removedIndexSet: �ѱ��Ƴ��Ľڵ���
	int i;
	int carNum = originCarSet.end() - originCarSet.begin();
	vector<float> reducedCostInRoute(0); // ����·���еĸ����ڵ���Ƴ�����
	for(i=0; i<carNum; i++){
		vector<float> tempReducedCost = originCarSet[i]->getRoute().computeReducedCost(DTpara, originCarSet[i]->judgeArtificial());
		reducedCostInRoute.insert(reducedCostInRoute.end(), tempReducedCost.begin(), tempReducedCost.end());
	}
	for(i=0; i<indexsetInRoute.end()-indexsetInRoute.begin(); i++){
		int index = indexsetInRoute[i];
		reducedCost[index].first = reducedCostInRoute[i];
		reducedCost[index].second = index;
	}
	for(i=0; i<removedIndexset.end() - removedIndexset.begin(); i++){
		int index = removedIndexset[i];
		reducedCost[index].first = MAX_FLOAT;  // �Ѿ��Ƴ����Ľڵ㣬��������
		reducedCost[index].second = index;	
	}
}

void SSALNS::shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer,
							int q, int p, float maxd, float maxt, float maxquantity){
	// originCarSet: δִ��remove����ǰ�Ļ�������
	// removedCarSet: ִ��remove������Ļ�������
	// removedCustomer: ���Ƴ��Ĺ˿ͽڵ�
	// q: ������Ҫremove�Ĺ˿�����
	// p: ����remove�����������
	// maxd, maxt, maxquantity: ��һ��ֵ
	// ÿ��ѭ���Ƴ� y^p*|L|���˿ͣ�LΪ·����ʣ��ڵ㣬y��0-1֮��������

	int phi = 9;
	int kai = 3;
	int psi = 2;
	int carAmount = originCarSet.end()-originCarSet.begin();  // ��������
	vector<int> customerNum(0);       // ������������Ĺ˿ͽڵ���Ŀ
	vector<Customer*> allCustomerInOrder(0);
	int i,j;
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);   
	// ��ȡ���е�Customer, customerNum��Ÿ�������ӵ�еĹ˿ͽڵ���Ŀ�� allCustomer������й˿ͽڵ�
	int customerAmount = allCustomerInOrder.end() - allCustomerInOrder.begin();  // �˿�����
	vector<pair<float, int>> R(customerAmount*customerAmount);     // ���ƾ���
	float temp1;
	vector<int> allIndex(customerAmount);  // 0~customerAmount-1
	for(i=0; i<(int)originCarSet.size(); i++){
		originCarSet[i]->getRoute().refreshArrivedTime();
	}
	for(i=0; i<customerAmount; i++){
		allIndex[i] = i;
		for(j=0; j<customerAmount; j++){
			if(i==j) { 
				R[i*customerAmount+j].first = MAX_FLOAT;
				R[i*customerAmount+j].second = j;
			}
			else{
				temp1 = phi*sqrt(pow(allCustomerInOrder[i]->x-allCustomerInOrder[j]->x,2) + pow(allCustomerInOrder[i]->y-allCustomerInOrder[j]->y, 2))/maxd +
					kai * abs(allCustomerInOrder[i]->arrivedTime - allCustomerInOrder[j]->arrivedTime)/maxt +
					psi * abs(allCustomerInOrder[i]->quantity - allCustomerInOrder[j]->quantity)/maxquantity;
				R[i*customerAmount+j].first = temp1;   // i��j��
				R[i*customerAmount+j].second = j;
				R[j*customerAmount+i].first = temp1;
				R[j*customerAmount+i].second = i;      // j��i��
			}
		}
	}
	int selectedIndex;           // ��ѡ�еĽڵ���allCustomer�е��±�
	vector<int> removedIndexset; // ���б��Ƴ��Ľڵ���±꼯��
	selectedIndex = int(random(0,customerAmount));   // ���ѡȡһ���ڵ�
	removedIndexset.push_back(selectedIndex);
	vector<int> indexsetInRoute(customerAmount-1);     // ��·���еĽڵ���±꼯��
	set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
	while((int)removedIndexset.size() < q){  // Ҫ�Ƴ���һ��q���ڵ�
		vector<pair<float, int>> currentR(0);        // ��ǰҪ������������ƾ�����������ֻ��������·���еĽڵ�
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin(); // ��·���еĽڵ�ĸ���
		vector<pair<float, int>>::iterator iter1;
		for(i=0; i<indexInRouteLen; i++){
			int index = indexsetInRoute[i];
			currentR.push_back(R[selectedIndex*customerAmount + index]);
		}
		sort(currentR.begin(), currentR.end(), ascendSort);  // �����԰�С�����������
		float y = rand()/(RAND_MAX+1.0f);  // ����0-1֮��������
		int indexsetInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();  // indexsetInRoute�ĳ���
		int removeNum = max((int)floor(pow(y,p)*indexsetInRouteLen), 1);             // �����Ƴ��Ľڵ���Ŀ
		for(i=0; i<removeNum ; i++){
			removedIndexset.push_back(currentR[i].second);
		}
		int indexRemovedLen = removedIndexset.end() - removedIndexset.begin();  // ��ǰremovedIndexset�ĳ���
		int randint = (int)random(0,indexRemovedLen);  // ����һ��0-indexRemovedLen�������
		selectedIndex = removedIndexset[randint];
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	for(i=0; i<(int)allCustomerInOrder.size(); i++){
		delete allCustomerInOrder[i];
	}
}

void SSALNS::randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q){
	// originCarSet: δִ��remove����ǰ�Ļ�������
	// removedCarSet: ִ��remove������Ļ�������
	// removedCustomer: ���Ƴ��Ĺ˿ͽڵ�
	// q: ������Ҫremove�Ĺ˿�����
	int i;
	vector<int> customerNum(0);       // ���������˿ͽڵ���Ŀ 
	vector<Customer*> allCustomerInOrder(0);  // ���й˿ͽڵ�
	vector<int> allIndex;             // 0~customerAmount-1
	vector<int> indexsetInRoute(0);
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	int customerAmount = allCustomerInOrder.end() - allCustomerInOrder.begin();
	vector<int> removedIndexset(0); 
	for(i=0; i<customerAmount; i++){
		allIndex.push_back(i);
	}
	indexsetInRoute = allIndex;
	for(i=0; i<q; i++){
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();  // ����·���еĽڵ����
		int selectedIndex = int(random(0, indexInRouteLen));  // ��indexsetInRoute�е�����
		removedIndexset.push_back(indexsetInRoute[selectedIndex]);
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	for(i=0; i<(int)allCustomerInOrder.size(); i++){
		delete allCustomerInOrder[i];
	}
}

void SSALNS::worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, vector<float> DTpara){
	// originCarSet: δִ��remove����ǰ�Ļ�������
	// removedCarSet: ִ��remove������Ļ�������
	// removedCustomer: ���Ƴ��Ĺ˿ͽڵ�
	// q: ������Ҫremove�Ĺ˿�����
	// p: ����remove�����������
	int i;
	vector<int> customerNum(0);       // ���������˿ͽڵ���Ŀ 
	vector<Customer*> allCustomerInOrder(0);  // ���й˿ͽڵ�
	vector<int> allIndex(0);             // 0~customerAmount-1��Ϊ���е�customer���λ��
	vector<int> indexsetInRoute(0);	  // ����·���еĽڵ��±�
	vector<int> removedIndexset(0);    // ���Ƴ��Ľڵ��±�
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	int customerAmount = allCustomerInOrder.end() - allCustomerInOrder.begin();  // originCarSet�еĹ˿�����
	for(i=0; i<customerAmount; i++){
		allIndex.push_back(i);
	}
	indexsetInRoute = allIndex;
	while((int)removedIndexset.size() < q){
		vector<pair<float, int>> reducedCost(customerAmount);  // ���ڵ���Ƴ�����	
		computeReducedCost(originCarSet, indexsetInRoute, removedIndexset, reducedCost, DTpara);
		sort(reducedCost.begin(), reducedCost.end(), ascendSort);   // ��������
		float y = rand()/(RAND_MAX+1.0f);  // ����0-1֮��������
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();
		int removedNum = static_cast<int>(max(floor(pow(y,p)*indexInRouteLen), 1.0f));
		assert(removedNum <= indexInRouteLen);
		for(i=0; i<removedNum; i++){
			removedIndexset.push_back(reducedCost[i].second);
		}
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;   // ��������������
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	for(i=0; i<(int)allCustomerInOrder.size(); i++){
		delete allCustomerInOrder[i];
	}
}

void generateMatrix(vector<int> &allIndex, vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, Matrix<float> &minInsertPerRoute, 
					Matrix<Customer> &minInsertPos, Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, float noiseAmount, bool noiseAdd,
					vector<float> DTpara){
	float DT11, DT12, DT21, DT22, DT31, DT32;
	vector<float>::iterator DTIter = DTpara.begin();
	DT11 = *(DTIter++);
	DT12 = *(DTIter++);
	DT21 = *(DTIter++);
	DT22 = *(DTIter++);
	DT31 = *(DTIter++);
	DT32 = *(DTIter++);
	int removedCustomerNum = removedCustomer.size();
	int carNum = removedCarSet.size();
	for(int i=0; i<carNum; i++){
		removedCarSet[i]->getRoute().refreshArrivedTime();  // �ȸ���һ�¸���·����arrivedTime
		for(int j=0; j<removedCustomerNum; j++){
			if(i==0){
				allIndex.push_back(j);
			}
			float minValue, secondValue;
			Customer customer1, customer2;
			float penaltyPara;
			if(removedCarSet[i]->judgeArtificial() == false){  // ����������⳵
				switch(removedCustomer[j]->priority) {  // ��������⳵
				case 1:
					penaltyPara = -DT11;
					break;
				case 2:
					penaltyPara = -DT21;
					break;
				case 3:
					penaltyPara = -DT31;
					break;
				}
			} else {   // ��������⳵������Ҫ���гͷ�
				switch(removedCustomer[j]->priority) {  // ��������⳵
				case 1:
					penaltyPara = DT12;
					break;
				case 2:
					penaltyPara = DT22;
					break;
				case 3:
					penaltyPara = DT32;
					break;
				}
			}
			removedCarSet[i]->getRoute().computeInsertCost(*removedCustomer[j], minValue, customer1, secondValue, customer2, noiseAmount, noiseAdd, penaltyPara);
			minInsertPerRoute.setValue(i, j, minValue);
			minInsertPos.setValue(i, j, customer1);
			secondInsertPerRoute.setValue(i, j, secondValue);
			secondInsertPos.setValue(i, j, customer2);
		}
	}
}

void updateMatrix(vector<int> restCustomerIndex, Matrix<float> &minInsertPerRoute, Matrix<Customer> &minInsertPos, 
				  Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, int selectedCarPos, vector<Car*> &removedCarSet,
				  vector<Customer*>removedCustomer, float noiseAmount, bool noiseAdd, vector<float> DTpara){
	// ��ȡDTpara�е�Ԫ��
	float DT11, DT12, DT21, DT22, DT31, DT32;
	vector<float>::iterator DTIter = DTpara.begin();
	DT11 = *(DTIter++);
	DT12 = *(DTIter++);
	DT21 = *(DTIter++);
	DT22 = *(DTIter++);
	DT31 = *(DTIter++);
	DT32 = *(DTIter++);

	// �����ĸ�����
	removedCarSet[selectedCarPos]->getRoute().refreshArrivedTime();
	for(int i=0; i<(int)restCustomerIndex.size();i++) {
		int index = restCustomerIndex[i];   // �˿��±�
		float minValue, secondValue;
		Customer customer1, customer2;
		float penaltyPara;
		if(removedCarSet[selectedCarPos]->judgeArtificial() == false) { // ��������鹹�ĳ���
			switch(removedCustomer[index]->priority) {  // ���ݲ�ͬ�Ĺ˿����ȼ������費ͬ�ĳͷ�ϵ���������뵽artificial vehicleʱ��
			case 1:
				penaltyPara = -DT11;
				break;
			case 2:
				penaltyPara = -DT21;
				break;
			case 3:
				penaltyPara = -DT31;
				break;
			}		
		} else {   // ���鹹�ĳ���
			switch(removedCustomer[index]->priority) {  // ���ݲ�ͬ�Ĺ˿����ȼ������費ͬ�ĳͷ�ϵ���������뵽artificial vehicleʱ��
			case 1:
				penaltyPara = DT12;
				break;
			case 2:
				penaltyPara = DT22;
				break;
			case 3:
				penaltyPara = DT32;
				break;
			}		
		}
		removedCarSet[selectedCarPos]->getRoute().computeInsertCost(*removedCustomer[index], minValue, customer1, secondValue, customer2, noiseAmount, noiseAdd, penaltyPara);
		minInsertPerRoute.setValue(selectedCarPos, index, minValue);
		minInsertPos.setValue(selectedCarPos, index, customer1);
		secondInsertPerRoute.setValue(selectedCarPos, index, secondValue);
		secondInsertPos.setValue(selectedCarPos, index, customer2);
	}
}

void SSALNS::greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer,
							 float noiseAmount, bool noiseAdd, vector<float> DTpara){
	// ��removedCustomer���뵽removedCarSet��
	// ����ǰ�����޷��������е�removedCustomer�����½�artificial car
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // ��Ҫ���뵽·���еĽڵ���Ŀ
	int carNum = removedCarSet.end() - removedCarSet.begin();    // ������Ŀ
	int newCarIndex = removedCarSet[carNum-1]->getCarIndex()+1;  // �³�����ʼ���
	int i;
	vector<int> alreadyInsertIndex(0);		   // �Ѿ����뵽·���еĽڵ��±꣬�����allIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // ��ÿ��·���е���С������۾��������꣺�����������꣺�˿ͣ�
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // ��ÿ��·���е���С�����������Ӧ�Ľڵ�
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // ��ÿ��·���еĴ�С������۾���
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // ��ÿ��·���еĴ�С�����������Ӧ�Ľڵ�
	vector<int> allIndex(0);   // ��removedCustomer���б��,1,2,3,...
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, 
		noiseAmount, noiseAdd, DTpara);
	vector<int> restCustomerIndex = allIndex;  // ʣ��û�в��뵽·���еĽڵ��±꣬�����removedCustomer
	vector<pair<float, pair<int,int>>> minInsertPerRestCust(0);  // ����removedcustomer����С�������
	                                                             // ֻ����û�в��뵽·���еĽڵ�
	                                                             // ��һ�������ǽڵ��±꣬�ڶ����ڵ��ǳ���λ��
	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		minInsertPerRestCust.clear();  // ÿ��ʹ��֮ǰ�����
		for(i=0; i<(int)restCustomerIndex.size(); i++){               // ֻ��������·���еĽڵ�
			int index = restCustomerIndex[i];
			int pos;
			float minValue;
			minValue = minInsertPerRoute.getMinAtCol(index, pos);
			minInsertPerRestCust.push_back(make_pair(minValue, make_pair(index, pos)));
		}	
		sort(minInsertPerRestCust.begin(), minInsertPerRestCust.end(), ascendSort2);
		int selectedCustIndex = minInsertPerRestCust[0].second.first;  // ��ѡ�еĹ˿ͽڵ���
		if(minInsertPerRestCust[0].first != MAX_FLOAT){  // ����ҵ��˿��в���λ��
			int selectedCarPos = minInsertPerRestCust[0].second.second;  // ��ѡ�еĳ���λ��
			removedCarSet[selectedCarPos]->getRoute().insertAfter(minInsertPos.getElement(selectedCarPos, selectedCustIndex), *removedCustomer[selectedCustIndex]);
			alreadyInsertIndex.push_back(selectedCustIndex);
			vector<int>::iterator iterINT;
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_differenceҪ��������
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // ����restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara);
		} else {  // û�п��в���λ�ã������¿�һ������
			int selectedCarPos = carNum++;  // ��ѡ�еĳ���λ��
			Car *newCar = new Car(depot, depot, capacity, newCarIndex++, true);
			newCar->getRoute().insertAtFront(*removedCustomer[selectedCustIndex]);
			removedCarSet.push_back(newCar);  // ��ӵ�����������
			alreadyInsertIndex.push_back(selectedCustIndex); // ����selectedCustIndex
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_differenceҪ��������
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // ����restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			minInsertPerRoute.addOneRow();   // ����һ��
			minInsertPos.addOneRow();
			secondInsertPerRoute.addOneRow();
			secondInsertPos.addOneRow();
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara);
		}
	}
}

void SSALNS::regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer,
							 float noiseAmount, bool noiseAdd, vector<float> DTpara){
	// ��removedCustomer���뵽removedCarSet��
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // ��Ҫ���뵽·���еĽڵ���Ŀ
	int carNum = removedCarSet.end() - removedCarSet.begin();    // ������Ŀ
	int newCarIndex = removedCarSet[carNum - 1]->getCarIndex();  // �³����
	int i;
	vector<int> alreadyInsertIndex(0);		   // �Ѿ����뵽·���еĽڵ��±꣬�����allIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // ��ÿ��·���е���С������۾��������꣺�����������꣺�˿ͣ�
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // ��ÿ��·���е���С�����������Ӧ�Ľڵ�
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // ��ÿ��·���еĴ�С������۾���
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // ��ÿ��·���еĴ�С�����������Ӧ�Ľڵ�
	vector<int> allIndex(0);   // ��removedCustomer���б��
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, 
		noiseAmount, noiseAdd, DTpara);
	vector<int> restCustomerIndex = allIndex;  // ʣ��û�в��뵽·���еĽڵ��±꣬�����removedCustomer
	vector<pair<float, pair<int,int>>> regretdiffPerRestCust(0);  // ����removedcustomer����С����������С�������֮��
	                                                              // ֻ����û�в��뵽·���еĽڵ�
	                                                              // ��һ�������ǽڵ��±꣬�ڶ����ڵ��ǳ����±�
	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		int selectedCustIndex;   // ѡ�еĹ˿ͽڵ���
		int selectedCarPos;      // ѡ�еĻ���λ��
		regretdiffPerRestCust.clear();
		for(i=0; i<(int)restCustomerIndex.size(); i++){
			int index = restCustomerIndex[i];        // �˿ͽڵ��±�
			float minValue, secondValue1, secondValue2;
			int pos1, pos2, pos3;
			minValue = minInsertPerRoute.getMinAtCol(index, pos1);          // ��С�������
			minInsertPerRoute.setValue(pos1, index, MAX_FLOAT);
			secondValue1 = minInsertPerRoute.getMinAtCol(index, pos2);      // ��ѡ��С�������
			minInsertPerRoute.setValue(pos1, index, minValue);              // �ָ�����
			secondValue2 = secondInsertPerRoute.getMinAtCol(index, pos3);   // ��ѡ��С�������
			if(minValue == MAX_FLOAT){  
				// �������ĳ���ڵ��Ѿ�û�п��в���㣬�����Ȱ���֮
				regretdiffPerRestCust.push_back(make_pair(MAX_FLOAT, make_pair(index, pos1)));
			} else if(minValue != MAX_FLOAT && secondValue1==MAX_FLOAT && secondValue2==MAX_FLOAT){
				// ���ֻ��һ�����в���㣬��Ӧ�����Ȱ���
				// ����minValue��ֵ����С��Ӧ�����ȱ�����
				// ���diff = LARGE_FLOAT - minValue
				regretdiffPerRestCust.push_back(make_pair(LARGE_FLOAT-minValue, make_pair(index, pos1)));
			} else{
				if(secondValue1 <= secondValue2){
					regretdiffPerRestCust.push_back(make_pair(abs(minValue-secondValue1), make_pair(index, pos1)));
				} else{
					regretdiffPerRestCust.push_back(make_pair(abs(minValue-secondValue2), make_pair(index, pos1)));
				}
			}
		}
		sort(regretdiffPerRestCust.begin(), regretdiffPerRestCust.end(), descendSort2);  // Ӧ���ɴ�С��������
		if(regretdiffPerRestCust[0].first == MAX_FLOAT) {
			// ������еĽڵ㶼û�п��в���㣬�򿪱��³�
			selectedCarPos= carNum++;
			selectedCustIndex = regretdiffPerRestCust[0].second.first;
			Car *newCar = new Car(depot, depot, capacity, newCarIndex++, true);
			newCar->getRoute().insertAtFront(*removedCustomer[selectedCustIndex]);
			removedCarSet.push_back(newCar);  // ��ӵ�����������
			alreadyInsertIndex.push_back(selectedCustIndex); // ����selectedCustIndex
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // ����restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			minInsertPerRoute.addOneRow();   // ����һ��
			minInsertPos.addOneRow();
			secondInsertPerRoute.addOneRow();
			secondInsertPos.addOneRow();
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara);	
		} else {
			// ���򣬲���Ҫ�����³�
			selectedCarPos = regretdiffPerRestCust[0].second.second;
			selectedCustIndex = regretdiffPerRestCust[0].second.first;
			alreadyInsertIndex.push_back(selectedCustIndex);
			removedCarSet[selectedCarPos]->getRoute().insertAfter(minInsertPos.getElement(selectedCarPos, selectedCustIndex), *removedCustomer[selectedCustIndex]);
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin());
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara);
		}
	}
}


void reallocateCarIndex(vector<Car*> &originCarSet){  // ����Ϊ�������
	for(int i=0; i<(int)originCarSet.size(); i++){
		originCarSet[i]->changeCarIndex(i);
	}
}

void removeNullRoute(vector<Car*> &originCarSet){    
	// ����ճ���
	// ֻ�����������Ŀճ�
	vector<Car*>::iterator iter;
	int count = 0;
	for(iter=originCarSet.begin(); iter<originCarSet.end();){
		if ((*iter)->getRoute().getSize() == 0 && (*iter)->judgeArtificial() == true) { // ����ǿճ�����������ĳ�
			iter = originCarSet.erase(iter);
		} else {
			(*iter)->changeCarIndex(count++);
			++iter;
		}
	}
}

size_t codeForSolution(vector<Car*> originCarSet){  // ��ÿ���⣨����·��������hash����
	vector<int> customerNum;
	vector<Customer*> allCustomerInOrder;
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	stringstream ss;
	string allStr = "";
	for(int i=0; i<(int)allCustomerInOrder.size(); i++){
		ss.str("");  // ÿ��ʹ��֮ǰ�����ss
		int a = allCustomerInOrder[i]->id;
		ss << a;
		allStr += ss.str();
	}
	hash<string> str_hash;
	return str_hash(allStr);
}

void computeMax(vector<Customer*> allCustomer, float &maxd, float &mind, float &maxquantity){
	// �������й˿�֮����������Լ��˿͵�������������
	int customerAmount = (int)allCustomer.size();
	Matrix<float> D(customerAmount, customerAmount);
	float tempmax = -MAX_FLOAT;
	for(int i=0; i<customerAmount; i++){
		if(allCustomer[i]->quantity > tempmax){
			tempmax = allCustomer[i]->quantity;
		}
		D.setValue(i,i, 0.0f);
		for(int j=i+1; j<customerAmount; j++){
			float temp = sqrt(pow(allCustomer[i]->x - allCustomer[j]->x, 2) + pow(allCustomer[i]->y - allCustomer[j]->y, 2));
			D.setValue(i,j, temp);
			D.setValue(j,i, temp);
		}
	}
	int t1, t2;
	maxd = D.getMaxValue(t1, t2);
	mind = D.getMinValue(t1, t2);
	maxquantity = tempmax;
}

float getCost(vector<Car*> originCarSet, vector<float> DTpara){
	// ����originCarSet��·��
	float totalCost = 0;
	for(int i=0; i<(int)originCarSet.size(); i++){

		float temp;
		if(originCarSet[i]->judgeArtificial() == true) {
			temp = originCarSet[i]->getRoute().getLen(DTpara, true);
		} else {
			temp = originCarSet[i]->getRoute().getLen(DTpara, false);
		}
		totalCost += temp;
	}
	return totalCost;
}

template<class T>
inline void setZero(T* p, int size){   // ����������Ԫ�ظ�ֵΪ0
	for(int i=0; i<size; i++){
		*(p++) = 0;
	}
}

inline void setOne(float* p, int size){   // ����������Ԫ�ظ�ֵΪ1
	for(int i=0; i<size; i++){
		*(p++) = 1.0f;
	}
}

inline void updateWeight(int *freq, float *weight, int *score, float r, int num) {  
	// ����Ȩ��
	for(int i=0; i<num; i++){
		if(*freq != 0){
			*weight = *weight *(1-r) + *score / *freq*r;
		} else {    // �������һ��segment��û��ʹ�ù������ӣ�Ȩ��Ӧ���½�
			*weight = *weight*(1-r);
		}
		freq++;
		weight++;
		score++;
	}
}

inline void updateProb(float *removeProb, float *removeWeight, int removeNum){
	// ���¸���
	float accRemoveWeight = 0;  // removeȨ��֮��
	for(int k=0; k<removeNum; k++){
		accRemoveWeight += removeWeight[k];
	}
	for(int i=0; i<removeNum; i++){
		*removeProb = *removeWeight/accRemoveWeight;
		removeProb++;
		removeWeight++;
	}
}

int getCustomerNum(vector<Car*> originCarSet){
	int customerNum = 0;
	for(int i=0; i<(int)originCarSet.size(); i++){
		customerNum += originCarSet[i]->getRoute().getSize();
	}
	return customerNum;
}

bool carSetEqual(vector<Car*> carSet1, vector<Car*> carSet2){
	// �ж�carSet1��carSet2�Ƿ����
	if(carSet1.size() != carSet2.size()) {return false;}
	bool mark = true;
	for(int i=0; i<(int)carSet1.size(); i++){
		vector<Customer*> cust1 = carSet1[i]->getRoute().getAllCustomer();
		vector<Customer*> cust2 = carSet2[i]->getRoute().getAllCustomer();
		if(cust1.size() != cust2.size()) {mark = false; break;}
		for(int j=0; j<(int)cust1.size(); j++) {
			if(cust1[j]->id != cust2[j]->id) {mark = false; break;}
		}
	}
	return mark;
}

bool customerSetEqual(vector<Customer*>c1, vector<Customer*>c2){
	if(c1.size() != c2.size()) {return false;}
	bool mark = true;
	for(int i=0; i<(int)c1.size(); i++) {
		if(c1[i]->id != c2[i]->id) {mark = false; break;}
	}
	return mark;

}

void SSALNS::run(vector<Car*> &finalCarSet, float &finalCost){  // �����㷨���൱���㷨��main()����
	int i;
	int PR1NUM = getCustomerNum(originPlan);
	int PR2NUM = (int)PR2.size();
	int PR3NUM = (int)PR3.size();
	int customerAmount = PR1NUM + PR2NUM + PR3NUM;  // �ܵĹ˿���
	int originCarNum = (int)originPlan.size();   // ��ʼӵ�еĻ�������
	vector<Customer*>::iterator custPtr;
	vector<Car*>::iterator carIter;

	// �ѵ�ǰӵ�е����й˿ͽڵ�ŵ�allCustomer��
	vector<Customer*> allCustomer;         // ���е�customer
	allCustomer.reserve(customerAmount);   // ΪallCustomerԤ���ռ�
	for(carIter = originPlan.begin(); carIter < originPlan.end(); carIter++) {
		vector<Customer*> temp = (*carIter)->getRoute().getAllCustomer();
		vector<Customer*>::iterator tempIter;
		for(tempIter = temp.begin(); tempIter < temp.end(); tempIter++) {
			allCustomer.push_back(*tempIter);
		}
	}
	for(custPtr = PR2.begin(); custPtr < PR2.end(); custPtr++) {
		allCustomer.push_back(*custPtr);
	}
	for(custPtr = PR3.begin(); custPtr < PR3.end(); custPtr++) {
		allCustomer.push_back(*custPtr);
	}

	// �������DT11 ~ DT32�����η���vector DTpara��
	float DT11, DT12, DT21, DT22, DT31, DT32;
	float maxd, mind, maxquantity;    // �ڵ�֮������/��С�����Լ��ڵ��������������
	computeMax(allCustomer, maxd, mind, maxquantity);
	DT32 = 1;
	DT31 = 2*maxd + 1;
	DT22 = 5;
	DT21 = 4*maxd + 1;
	DT12 = 20;
	float tempsigma1 = 2*maxd + DT12;
	float tempsigma2 = 2*(PR1NUM + PR2NUM + PR3NUM) * maxd + PR2NUM * DT22 + PR3NUM * DT32 - 
		(PR1NUM + PR2NUM + PR3NUM) * mind + PR2NUM * DT21 + PR3NUM * DT31 - DT12;
	DT11 = max(tempsigma1, tempsigma2) + 1;
	DT11 = 2000;
	vector<float> DTpara;
	DTpara.push_back(DT11);
	DTpara.push_back(DT12);
	DTpara.push_back(DT21);
	DTpara.push_back(DT22);
	DTpara.push_back(DT31);
	DTpara.push_back(DT32);

	// ��ΪPR2��PR3�˿Ͱ���artificial vehicleȥ����
	vector<Customer*> unscheduledCustomer(0);    // PR2 and PR3
	for(custPtr = PR2.begin(); custPtr < PR2.end(); custPtr++){
		unscheduledCustomer.push_back(*custPtr);
	}
	for(custPtr = PR3.begin(); custPtr < PR3.end(); custPtr++){
		unscheduledCustomer.push_back(*custPtr);
	}
	vector<Car*> artificialCarSet(0);
	Car *initialCar = new Car(depot, depot, capacity, originCarNum+1, true);  // ���½�һ�����⳵
	artificialCarSet.push_back(initialCar);
	greedyInsert(artificialCarSet, unscheduledCustomer, 0, false, DTpara);  // �������⳵��PR2��PR3�˿�װ��ȥ

	// ��current plan��car��artificialcarset�е�car�������
	vector<Car*> currentCarSet(0);  // ��ǰ��
	vector<Car*> globalCarSet(0);   // ȫ�����Ž⣬��ʼ���뵱ǰ����ͬ
	for(carIter = originPlan.begin(); carIter < originPlan.end(); carIter++) {
		Car* newCar1 = new Car(**carIter);
		Car* newCar2 = new Car(**carIter);
		currentCarSet.push_back(newCar1);
		globalCarSet.push_back(newCar2);
	}
	for(carIter = artificialCarSet.begin(); carIter < artificialCarSet.end(); carIter++) {
		Car* newCar1 = new Car(**carIter);
		Car* newCar2 = new Car(**carIter);
		currentCarSet.push_back(newCar1);
		globalCarSet.push_back(newCar2);
	}
	float currentCost = getCost(currentCarSet, DTpara);
	float globalCost = currentCost;

	vector<size_t> hashTable(0);  // ��ϣ��
	hashTable.push_back(codeForSolution(currentCarSet));

	// ���ֻ�����ز������趨
	const int removeNum = 3;    // remove heuristic�ĸ���
	const int insertNum = 2;    // insert heuristic�ĸ���
	float removeProb[removeNum];  // ����remove heuristic�ĸ���
	float insertProb[insertNum];  // ����insert heuristic�ĸ���
	float noiseProb[2] = {0.5, 0.5};        // ����ʹ�õĸ���
	for(i=0; i<removeNum; i++){
		removeProb[i] = 1.0f/removeNum;
	}
	for(i=0; i<insertNum; i++){
		insertProb[i] = 1.0f/insertNum;
	}
	float removeWeight[removeNum];  // ����remove heuristic��Ȩ��
	float insertWeight[insertNum];  // ����insert heuristic��Ȩ��
	float noiseWeight[2];   // ������/�������� �ֱ��Ȩ��
	setOne(removeWeight, removeNum);
	setOne(insertWeight, insertNum);
	setOne(noiseWeight, 2);
	int removeFreq[removeNum];      // ����remove heuristicʹ�õ�Ƶ��
	int insertFreq[insertNum];      // ����insert heuristicʹ�õ�Ƶ��
	int noiseFreq[2];               // ����ʹ�õ�Ƶ�ʣ���һ����with noise���ڶ�����without noise
	setZero<int>(removeFreq, removeNum);
	setZero<int>(insertFreq, insertNum);
	setZero<int>(noiseFreq, 2);
	int removeScore[removeNum];     // ����remove heuristic�ĵ÷�
	int insertScore[insertNum];     // ����insert heuristic�ĵ÷�
	int noiseScore[2];              // �����÷�
	setZero<int>(removeScore, removeNum);
	setZero<int>(insertScore, insertNum);
	setZero<int>(noiseScore, 2);
	// ����÷��趨
	int sigma1 = 33;
	int sigma2 = 9;
	int sigma3 = 13;
	float r = 0.1f;       // weight��������

	// ������Ĳ���
	int maxIter = 25000; // �ܵĵ�������
	int segment = 100;   // ÿ��һ��segment����removeProb, removeWeight�Ȳ���
	float w = 0.05f;      // ��ʼ�¶��趨�йز���
	float T = w * abs(currentCost) / (float)log(2);   // ��ʼ�¶�
	int p = 6;           // ����shawRemoval�����
	int pworst = 3;      // ����worstRemoval�������
	float ksi = 0.4f;     // ÿ���Ƴ������ڵ���Ŀռ�ܽڵ����ı���
	float eta = 0.025f;   // ����ϵ��
	float noiseAmount = eta * maxd;   // ������
	float c = 0.9998f;    // ��������
	srand(unsigned(time(0)));
	vector<Customer*> removedCustomer(0);    // ���Ƴ��Ľڵ�
	vector<Car*> tempCarSet(0);      // ��ʱ��ŵ�ǰ��
	for(i=0; i<(int)currentCarSet.size();i++){
		Car* newCar = new Car(*currentCarSet[i]);
		tempCarSet.push_back(newCar);
	}
	for(int iter=0; iter<maxIter; iter++){
		if(iter%segment == 0){  // �µ�segment��ʼ
			cout << "...............Segement:" << (int)floor(iter/segment)+1 << "................" << endl;
			cout << "current best cost is:" << globalCost << endl;
			cout << "hash table length is:" << hashTable.size() << endl;
			cout << "shaw   removal:" <<  "(score)-" << removeScore[0] << '\t' << "(freq)-" << removeFreq[0] << endl;
			cout << "random removal:" <<  "(score)-" << removeScore[1] << '\t' << "(freq)-" << removeFreq[1] << endl;
			cout << "worst  removal:" <<  "(score)-" << removeScore[2] << '\t' << "(freq)-" << removeFreq[2] << endl;
			cout << "greedy  insert:" <<  "(score)-" << insertScore[0] << '\t' << "(freq)-" << insertFreq[0] << endl;
			cout << "regret  insert:" <<  "(score)-" << insertScore[1] << '\t' << "(freq)-" << insertFreq[1] << endl;
			cout << "noise    addIn:" <<  "(score)-" << noiseScore[0]  << '\t' << "(freq)-" << noiseFreq[0]  << endl;
			cout << endl;
			if(iter != 0){      // ������ǵ�һ��segment
				// ����Ȩ��
				updateWeight(removeFreq, removeWeight, removeScore, r, removeNum);
				updateWeight(insertFreq, insertWeight, insertScore, r, insertNum);
				updateWeight(noiseFreq, noiseWeight, noiseScore, r, 2);
				// ���¸���
				updateProb(removeProb, removeWeight, removeNum);
				updateProb(insertProb, insertWeight, insertNum);
				updateProb(noiseProb, noiseWeight, 2);
				// ������������
				setZero<int>(removeFreq, removeNum);
				setZero<int>(removeScore, removeNum);
				setZero<int>(insertFreq, insertNum);
				setZero<int>(insertScore, insertNum);
				setZero<int>(noiseFreq, 2);
				setZero<int>(noiseScore, 2);
			}
		}

		// ���������ѡȡremove heuristic��insert heuristic
		// �Ը���ѡ��remove heuristic
		float removeSelection = rand()/(RAND_MAX+1.0f);  // ����0-1֮��������
		float sumation = removeProb[0];
		int removeIndex = 0;    // remove heuristic���
		while(sumation < removeSelection){
			sumation += removeProb[++removeIndex];
		}
		// �Ը���ѡ��insert heurisitc
		float insertSelection = rand()/(RAND_MAX+1.0f);
		sumation = insertProb[0];
		int insertIndex = 0;
		while(sumation < insertSelection){
			sumation += insertProb[++insertIndex];
		}
		// �Ը���ѡ���Ƿ���������Ӱ��
		float noiseSelection = rand()/(RAND_MAX+1.0f);
		bool noiseAdd = false;
		if(noiseProb[0] > noiseSelection) {
			noiseAdd = true;
		}

		////@@@@@@@@@ dangerous!!!!!!!!! @@@@@@@@@//
		//removeIndex = 0;
		//insertIndex = 0;
		////////////////////////////////////////////

		// ��Ӧ����ʹ�ô�����һ
		removeFreq[removeIndex]++;
		insertFreq[insertIndex]++;
		noiseFreq[1-(int)noiseAdd]++;
		int maxRemoveNum = min(100, static_cast<int>(floor(ksi*customerAmount)));  // ����Ƴ���ô��ڵ�
		int minRemoveNum = 4;  // �����Ƴ���ô��ڵ�
		int currentRemoveNum = (int)random(minRemoveNum, maxRemoveNum);  // ��ǰҪ�Ƴ��Ľڵ���Ŀ
		removedCustomer.clear();         // ���removedCustomer
		removedCustomer.resize(0);

		// ִ��remove heuristic
		switch(removeIndex) {
		case 0: 
			{
				// ���ȵõ�maxArrivedTime
				float maxArrivedTime = -MAX_FLOAT;
				for(i=0; i<(int)tempCarSet.size(); i++){
					tempCarSet[i]->getRoute().refreshArrivedTime();	
					vector<float> temp = tempCarSet[i]->getRoute().getArrivedTime();
					sort(temp.begin(), temp.end(), greater<float>());
					if(temp[0] > maxArrivedTime) {
						maxArrivedTime = temp[0];
					}
				}
				shawRemoval(tempCarSet, removedCustomer, currentRemoveNum, p, maxd, maxArrivedTime, maxquantity);
				break;
			}
		case 1:
			{
				randomRemoval(tempCarSet, removedCustomer, currentRemoveNum);
				break;
			}
		case 2:
			{
				worstRemoval(tempCarSet, removedCustomer, currentRemoveNum, pworst, DTpara);
				break;
			}
		}
		// ִ��insert heuristic
		switch(insertIndex) {
		case 0:
			{
				greedyInsert(tempCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara);
				break;
			}
		case 1:
			{
				regretInsert(tempCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara);
				break;
			}
		}
		assert(getCustomerNum(tempCarSet) == customerAmount);
		// �Ƴ���·��
		removeNullRoute(tempCarSet);

		// ʹ��ģ���˻��㷨�����Ƿ���ոý�
		float newCost = getCost(tempCarSet, DTpara);
		float acceptProb = exp(-(newCost - currentCost)/T);
		bool accept = false;
		if(acceptProb > rand()/(RAND_MAX+1.0f)) {
			accept = true;
		}
		T = T * c;   // ����
		size_t newRouteCode = codeForSolution(tempCarSet);

		// �������ж��Ƿ���Ҫ�ӷ�
		// �ӷ�������£�
		// 1. ���õ�һ��ȫ�����Ž�ʱ
		// 2. ���õ�һ����δ�����ܹ��ģ����Ҹ��õĽ�ʱ
		// 3. ���õ�һ����δ�����ܹ��Ľ⣬��Ȼ�����ȵ�ǰ����������ⱻ������
		if(newCost < globalCost){  // ���1
			removeScore[removeIndex] += sigma1;
			insertScore[insertIndex] += sigma1;
			noiseScore[1-(int)noiseAdd] += sigma1;
			for(i=0; i<(int)globalCarSet.size(); i++){  // �������ֵ
				delete globalCarSet[i];
			}
			globalCarSet.resize(0);
			for(i=0; i<(int)tempCarSet.size(); i++){
				Car* newCar = new Car(*tempCarSet[i]);
				globalCarSet.push_back(newCar);
			}
			globalCost = newCost;
		} else {
			vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
			if(tempIter == hashTable.end()){  // �ý����û�б����ܹ�
				if(newCost < currentCost){    // ����ȵ�ǰ��Ҫ�ã����2
					removeScore[removeIndex] += sigma2;
					insertScore[insertIndex] += sigma2;
					noiseScore[1-(int)noiseAdd] += sigma2;
				} else {
					if(accept == true) {       // ���3
						removeScore[removeIndex] += sigma3;
						insertScore[insertIndex] += sigma3;
						noiseScore[1-(int)noiseAdd] += sigma3;						
					}
				}
			}
		}
		if(accept == true) {    // ����������ˣ������currentCarSet�� ����tempCarSet����
			vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
			if(tempIter == hashTable.end()){
				hashTable.push_back(newRouteCode); 
			}
			currentCost = newCost;     // ��������գ�����µ�ǰ��
			for(i=0; i<(int)currentCarSet.size(); i++){
				delete currentCarSet[i];
			}
			currentCarSet.resize(0);
			for(i=0; i<(int)tempCarSet.size(); i++){
				Car* newCar = new Car(*tempCarSet[i]);
				currentCarSet.push_back(newCar);
			}
		} else {    // ����tempCarSet�ָ�ΪcurrentCarSet
			for(i=0; i<(int)tempCarSet.size(); i++){
				delete tempCarSet[i];
			}
			tempCarSet.resize(0);
			for(i=0; i<(int)currentCarSet.size(); i++){
				Car* newCar = new Car(*currentCarSet[i]);
				tempCarSet.push_back(newCar);
			}
		}
	}
	finalCarSet.clear();
	finalCarSet.resize(globalCarSet.size());
	copy(globalCarSet.begin(), globalCarSet.end(), finalCarSet.begin());   // �������ǳ���ƾͿ�����
	finalCost = globalCost;
}

