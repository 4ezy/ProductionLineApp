#pragma once
#include <queue>
#include <vector>
#include "WorkplaceLine.h"
#include "Product.h"
#include "Statistics.h"

class ProductionLine
{
public:
	ProductionLine();
	~ProductionLine();
	std::vector<WorkplaceLine> workplaceLines;
	std::queue<Product> deferredProducts;
	Statistics statistics;
private:

};

//ProductionLine::ProductionLine()
//{
//}
//
//ProductionLine::~ProductionLine()
//{
//}