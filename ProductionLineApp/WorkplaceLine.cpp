#include "stdafx.h"
#include "WorkplaceLine.h"

WorkplaceLine::WorkplaceLine(unsigned int workplaceLineCapacity, unsigned long workplaceProcTime)
	: workplace(workplaceProcTime)
{
	this->SetWorkplaceLineCapacity(workplaceLineCapacity);
}

WorkplaceLine::~WorkplaceLine()
{
}

bool WorkplaceLine::PutProductToWorkplace()
{
	if (this->workplace.GetIsEmpty() && !this->workplace.GetIsBlocked() &&
		this->products.size() > 0 && this->workplace.GetProcProductRef()->isProcessed == false)
	{
		this->workplace.SetProcProduct(this->products.back());
		this->workplace.SetIsEmpty(false);
		this->products.pop_back();
		return true;
	}
	else
		return false;
}

bool WorkplaceLine::PutProductToWorkplaceLine(WorkplaceLine *workplaceLine)
{
	if (!this->workplace.GetIsEmpty() && !this->workplace.GetIsProductProc() &&
		this->workplace.GetProcProductRef()->isProcessed)
	{
		if (workplaceLine->products.size() != 0)
		{
			workplaceLine->products.insert(workplaceLine->products.begin(), this->workplace.GetProcProduct());
		}
		else
		{
			workplaceLine->products.push_back(this->workplace.GetProcProduct());
		}

		workplaceLine->products[0].isProcessed = false;
		this->workplace.SetIsEmpty(true);
		this->workplace.GetProcProductRef()->isProcessed = false;
		return true;
	}
	else
		return false;
}

void WorkplaceLine::SetWorkplaceLineCapacity(unsigned int workplaceLineCapacity)
{
	this->workplaceLineCapacity = workplaceLineCapacity;
}

unsigned int WorkplaceLine::GetWorkplaceLineCapacity()
{
	return this->workplaceLineCapacity;
}

void WorkplaceLine::SetWorkplace(Workplace workplace)
{
	this->workplace = workplace;
}

Workplace WorkplaceLine::GetWorkplace()
{
	return this->workplace;
}

Workplace* WorkplaceLine::GetWorkplaceRef()
{
	return &this->workplace;
}

void WorkplaceLine::SetProducts(std::vector<Product> products)
{
	this->products = products;
}

std::vector<Product> WorkplaceLine::GetProducts()
{
	return this->products;
}

std::vector<Product>* WorkplaceLine::GetProductsRef()
{
	return &this->products;
}
