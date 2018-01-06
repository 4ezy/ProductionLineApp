#pragma once
#include <queue>
#include <vector>
#include "WorkplaceLine.h"
#include "Product.h"
#include "Statistics.h"

class ProductionLine
{
public:
	std::vector<WorkplaceLine> workplaceLines;
	std::queue<Product> deferredProducts;
	Statistics statistics;

	ProductionLine();
	~ProductionLine();

	bool PutNewProduct();
	bool DeferProduct(Product product);
	bool ReleaseProduct();
private:

};

//ProductionLine::ProductionLine()
//{
//}
//
//ProductionLine::~ProductionLine()
//{
//}