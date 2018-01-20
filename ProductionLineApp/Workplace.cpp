#include "stdafx.h"
#include "Workplace.h"
#include <time.h>

Workplace::Workplace(unsigned long procTime)
{
	this->isEmpty = true;
	this->isBlocked = false;
	this->isProductProc = false;
	this->procTime = procTime;
	this->procProduct.isProcessed = false;
}

Workplace::~Workplace()
{
}

bool Workplace::ProcessProduct()
{
	if (!this->isEmpty && !this->isProductProc &&
		!this->procProduct.isProcessed)
	{
		unsigned long t = (unsigned long)time(NULL);
		generator = std::default_random_engine(t);
		long double val = this->procTime;
		val /= 1000;
		long double distr = val;
		std::normal_distribution<double> distribution(distr);
		distr = distribution(generator);

		while (distr <= 0 || distr >(val + 0.5) || distr < (val - 0.5))
		{
			distr = distribution(generator);
		}

		distr *= 1000;
		//stat->avgProdProcTime += (unsigned long)distr;
		this->isProductProc = true;
		Sleep((unsigned long)distr);
		this->isProductProc = false;
		this->procProduct.isProcessed = true;
		return true;
	}
	else
		return false;
}

void Workplace::SetIsEmpty(bool isEmpty)
{
	this->isEmpty = isEmpty;
}

bool Workplace::GetIsEmpty()
{
	return this->isEmpty;
}

void Workplace::SetIsBlocked(bool isBlocked)
{
	this->isBlocked = isBlocked;
}

bool Workplace::GetIsBlocked()
{
	return this->isBlocked;
}

void Workplace::SetIsProductProc(bool isProductProc)
{
	this->isProductProc = isProductProc;
}

bool Workplace::GetIsProductProc()
{
	return this->isProductProc;
}

void Workplace::SetProcProduct(Product procProduct)
{
	this->procProduct = procProduct;
}

Product Workplace::GetProcProduct()
{
	return this->procProduct;
}

Product * Workplace::GetProcProductRef()
{
	return &this->procProduct;
}

void Workplace::SetProcTime(unsigned long procTime)
{
	this->procTime = procTime;
}

unsigned long Workplace::GetProcTime()
{
	return this->procTime;
}
