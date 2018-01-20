#pragma once
#include "Product.h"
#include <random>
#include "Statistics.h"

class Workplace
{
public:
	Workplace(unsigned long procTime);
	~Workplace();

	bool ProcessProduct(Statistics* stat);

	void SetIsEmpty(bool isEmpty);
	bool GetIsEmpty();
	void SetIsBlocked(bool isBlocked);
	bool GetIsBlocked();
	void SetIsProductProc(bool isProductProc);
	bool GetIsProductProc();
	void SetProcProduct(Product procProduct);
	Product GetProcProduct();
	Product* GetProcProductRef();
	void SetProcTime(unsigned long procTime);
	unsigned long GetProcTime();
private:
	bool isEmpty;
	bool isBlocked;
	bool isProductProc;
	unsigned long procTime;
	Product procProduct;
	std::default_random_engine generator;
};