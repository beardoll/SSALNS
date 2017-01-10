#include "Car.h"

Car::Car(Customer &headNode, Customer &rearNode, float capacity, int index, bool artificial):
	carIndex(index), route(headNode, rearNode, capacity), artificial(artificial){
	state = wait;
}

Car::~Car(){  // ��Ƕ��������������ᱻ���ã������ڴ˴�delete route

}

Car::Car(Car &item):route(item.route){  //���ƹ��캯��
	this->state = item.state;
	this->carIndex = item.carIndex;
}


Car& Car::operator= (Car &item){ // ���ظ�ֵ����
	this->route.clear();
	this->route = item.route;
	this->carIndex = item.carIndex;
	this->state = item.state;
	return *this;
}

Route& Car::getRoute(){    // �õ�����·��
	return this->route;
}

void Car::updateRoute(Route &L){       // ����currentָ����·��
	route.replaceRoute(L);
}

void Car::changeCarIndex(int newIndex){
	carIndex = newIndex;
	getRoute().changeCarIndex(newIndex);
}