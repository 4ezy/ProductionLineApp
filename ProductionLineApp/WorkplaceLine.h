#pragma once
#include <queue>
#include "Workplace.h"
#include "Product.h"

class WorkplaceLine
{
public:
	WorkplaceLine(unsigned int workplaceLineCapacity, unsigned long workplaceProcTime);
	~WorkplaceLine();

	bool PutProductToWorkplace();
	bool PutProductToWorkplaceLine(WorkplaceLine *workplaceLine);

	void SetWorkplaceLineCapacity(unsigned int workplaceLineCapacity);
	unsigned int GetWorkplaceLineCapacity();
	void SetWorkplace(Workplace workplace);
	Workplace GetWorkplace();
	Workplace* GetWorkplaceRef();
	void SetProducts(std::vector<Product> products);
	std::vector<Product> GetProducts();
	std::vector<Product>* GetProductsRef();
private:
	unsigned int workplaceLineCapacity;
	Workplace workplace;
	std::vector<Product> products;
};