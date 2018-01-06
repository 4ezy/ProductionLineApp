#pragma once
#include "Product.h"

class Workplace
{
public:
	bool isEmpty;
	bool isBlocked;
	bool isProductProc;
	Product procProduct;

	Workplace();
	~Workplace();

	bool ProcessProduct();
private:

};

//Workplace::Workplace()
//{
//}
//
//Workplace::~Workplace()
//{
//}