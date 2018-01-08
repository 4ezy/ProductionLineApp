#pragma once
#include <queue>
#include "Workplace.h"
#include "Product.h"

class WorkplaceLine
{
public:
	WorkplaceLine(int workplaceLineCapacity, unsigned long workplaceProcTime);
	~WorkplaceLine();

	bool PutProductToWorkplace();
	bool PutProductToWorkplaceLine(WorkplaceLine workplaceLine);

	void SetWorkplaceLineCapacity(int workplaceLineCapacity);
	int GetWorkplaceLineCapacity();
	void SetWorkplace(Workplace workplace);
	Workplace GetWorkplace();
	void SetProducts(std::queue<Product> products);
	std::queue<Product> GetProducts();
private:
	int workplaceLineCapacity;
	Workplace workplace;
	std::queue<Product> products;
};