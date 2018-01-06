#pragma once
#include <queue>
#include "Workplace.h"
#include "Product.h"

class WorkplaceLine
{
public:
	Workplace workplace;
	std::queue<Product> products;

	WorkplaceLine();
	~WorkplaceLine();

	bool PutProductToWorkplace();
	bool PutProductToWorkplaceLine(
		WorkplaceLine workplaceLine);
private:

};

//WorkplaceLine::WorkplaceLine()
//{
//}
//
//WorkplaceLine::~WorkplaceLine()
//{
//}