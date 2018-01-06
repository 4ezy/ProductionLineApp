#pragma once
#include <queue>
#include "Workplace.h"
#include "Product.h"

class WorkplaceLine
{
public:
	WorkplaceLine();
	~WorkplaceLine();

	bool PutProductToWorkplace();
	bool PutProductToWorkplaceLine(
		WorkplaceLine);

	void SetWorkplace(Workplace);
	Workplace GetWorkplace();
	void SetProducts(std::queue<Product>);
	std::queue<Product> GetProducts();
private:
	Workplace workplace;
	std::queue<Product> products;
};