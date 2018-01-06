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

	bool PutNewProduct();
	bool DeferProduct(Product);
	bool ReleaseProduct();

	void SetCapacity(int);
	int GetCapacity();
	void SetWorkplaceLines(std::vector<WorkplaceLine>);
	std::vector<WorkplaceLine> GetWorkplaceLines();
	void SetDefferedProducts(std::queue<Product>);
	std::queue<Product> GetDefferedProducts();
	void SetStatistics(Statistics);
	Statistics GetStatistics();
private:
	int capacity;
	std::vector<WorkplaceLine> workplaceLines;
	std::queue<Product> deferredProducts;
	Statistics statistics;
};