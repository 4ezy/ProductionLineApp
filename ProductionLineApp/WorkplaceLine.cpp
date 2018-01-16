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
		this->products.size() != 0)
	{
		this->workplace.SetProcProduct(this->products.back());
		this->workplace.SetIsEmpty(false);
		//this->workplace.SetIsProductProc(true);
		this->products.pop_back();
		return true;
	}
	else
		return false;
}

bool WorkplaceLine::PutProductToWorkplaceLine(WorkplaceLine *workplaceLine)
{
	if (!this->workplace.GetIsEmpty() && !this->workplace.GetIsProductProc())
	{
		if (workplaceLine->products.size() != 0)
		{
			workplaceLine->products.insert(workplaceLine->products.begin(), workplaceLine->workplace.GetProcProduct());
		}
		else
		{
			workplaceLine->products.push_back(workplaceLine->workplace.GetProcProduct());
		}

		this->workplace.SetIsEmpty(true);
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
