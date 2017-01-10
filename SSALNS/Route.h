#ifndef _ROUTE_H
#define _ROUTE_H

#include<iostream>
#include<vector>
#include "Customer.h"

using namespace std;

class Route{
public:
	Route(Customer &headNode, Customer &rearNode, float capacity);      // ���캯��
	~Route();   // ��������
	Route(const Route &L);  // ���ƹ��캯��
	Route& operator= (const Route &L);  // ���ظ�ֵ����� 
	Customer& operator[] (int k);     // �õ������еĵ�k��Ԫ��
	const Customer& operator[] (int k) const; // ��̬����
	bool isEmpty();   // �ж������Ƿ�Ϊ��
	bool insertAfter(Customer &item1, Customer &item2); // ����������item1��ͬ�Ľڵ�������ڵ�item2
	void insertAtFront(Customer &item);   // �ڱ�ͷ����item
	bool deleteNode(Customer &item);   // ��������ɾ����item��ͬ�Ľڵ�
	bool next();  // currentָ����ǰ�ߣ�������false����ʾ�Ѿ��ߵ�ͷ
	void printRoute();   // ��ӡ·��
	void clear();     // �������
	void assignCar(int carIndex); // Ϊroute���䳵��
	void replaceRoute(const Route &route);  // ��route�滻��currentָ����·��
	Route& capture();  // ץȡcurrentָ����·��
	Customer& currentPos();   // ���ص�ǰλ��
	int getSize();     // �õ���ǰ����Ĵ�С
	vector<Customer*> getAllCustomer();  // �õ�·�������еĹ˿ͽڵ�
	vector<float> computeReducedCost(float eta1 = 1.0f, float eta2 = 1.0f, float eta3 = 1.0f, bool mark = false);  // �������нڵ���Ƴ�����
	void computeInsertCost(Customer item, float &minValue, Customer &customer1, float &secondValue, Customer &customer2, 
		float noiseAmount, bool noiseAdd, float penaltyPara);  
	// ����item�ڵ���·���е���С������ۺʹ�С�������
	// ���������/�μѲ����ǰ��Ĺ˿ͽڵ�
	// penaltyParaΪ�ͷ�ϵ����������Ҫ�ͷ���penaltyPara = 1
	bool timeWindowJudge(Customer *pre, int pos, Customer item);
	// �����item���뵽pre�����Ƿ��Υ��ʱ�䴰Լ��
	void refreshArrivedTime();   // ����һ�¸����ڵ�ĵ���ʱ��
	void changeCarIndex(int newIndex);  // ���³������
	float getLen();   // �õ�·������
	vector<float> getArrivedTime();     // �õ��������нڵ��arrivedTime��ע���һ��Ԫ����0
private:
	Customer *head, *current, *rear;  // ��ͷ����β�͵�ǰָ�룬��ǰָ��ָ�������ǰ��פ��
	int size;         // ����ĳ���
	int carIndex;     // ��������
	float quantityL, quantityB;  // linehaul��backhaul�ڵ�ֱ������
	float capacity;   // ��������������Ҳ��һ��
	void copy(const Route& L);  // �������������ƹ��캯�������ء�=������ʹ��
	vector<float> arrivedTime;  // �ڸ����ڵ�ĵ���ʱ��
};


#endif