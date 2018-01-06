#pragma once
#include "Product.h"

class Workplace
{
public:
	Workplace();
	~Workplace();

	bool ProcessProduct();

	void SetIsEmpty(bool);
	bool GetIsEmpty();
	void SetIsBlocked(bool);
	bool GetIsBlocked();
	void SetIsProductProc(bool);
	bool GetIsProductProc();
	void SetProcProduct(Product);
	Product GetProcProduct();
private:
	bool isEmpty;
	bool isBlocked;
	bool isProductProc;
	Product procProduct;
};