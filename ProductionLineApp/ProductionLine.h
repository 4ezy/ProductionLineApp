#pragma once
#include <queue>
#include "ProductionLineStatistics.h"

class ProductionLine
{
public:
	ProductionLine();
	~ProductionLine();
	//std::queue<Products> deferredProducts;	// отложенные изделия
	ProductionLineStatistics statistics;
private:

};

//ProductionLine::ProductionLine()
//{
//}
//
//ProductionLine::~ProductionLine()
//{
//}