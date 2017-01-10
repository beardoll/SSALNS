#include "Route.h"
#include<vector>
#ifndef _CAR_H
#define _CAR_H

enum State{wait, depature, arrived, startservice};

class Car{
public:
	Car(Customer &headNode, Customer &rearNode, float capacity, int index, bool artificial);  // ���캯��
	~Car();          // ��������
	Car(Car &item);  //���ƹ��캯��
	Car& operator= (Car &item);       // ���ظ�ֵ����
	void updateState(char newState);  // ����״̬
	char getState();                  // �õ���ǰ��״̬
	void updateRoute(Route &L);       // ����currentָ����·��
	void moveForword();         // ��ǰ��
	void getLeftCapacity();     // �õ�ʣ��ĳ�����
	void getServedCustomers();  // �õ��Ѿ�������Ĺ˿͵㼯
	Route& getRoute();    // �õ�����·��
	void changeCarIndex(int newIndex);    // ���ĳ������
	int getCarIndex() {return carIndex;}  // �õ��������
	bool judgeArtificial() {return artificial;} // ���س�������
private:
	State state;    // ����״̬�������н����ȴ�����
	Route route;    // �ƻ�Ҫ�ߵ�·��
	bool artificial;  // Ϊtrue��ʾ���鹹�ĳ�����false��ʾ��ʵ�ĳ���
	int carIndex;     // �������
};

#endif