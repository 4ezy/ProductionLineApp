#include "stdafx.h"
#include "Workplace.h"

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
		this->isProductProc = true;
		Sleep(this->procTime); // TODO: заменить на нормальное распределение
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
