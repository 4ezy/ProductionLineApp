#pragma once
#include <queue>
#include "Workplace.h"
#include "Product.h"

class WorkplaceLine
{
public:
	WorkplaceLine();
	~WorkplaceLine();
	Workplace workplace;
	std::queue<Product> products;
private:

};

//WorkplaceLine::WorkplaceLine()
//{
//}
//
//WorkplaceLine::~WorkplaceLine()
//{
//}