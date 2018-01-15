#include "ProductionLine.h"

ProductionLine::ProductionLine() { };

ProductionLine::ProductionLine(unsigned int workplaceLineCount, unsigned int productionLineCapacity,
	unsigned long putProductTimeInterval, unsigned long workplaceProcTime, unsigned long simulationTime)
{
	this->putProductTimeInterval = putProductTimeInterval;
	this->simulationTime = simulationTime;

	int workplaceLineCapacity = productionLineCapacity / workplaceLineCount - 1;
	workplaceLineCapacity = workplaceLineCapacity < 0 ? 0 : workplaceLineCapacity;

	for (unsigned int i = 0; i < workplaceLineCount; i++)
	{
		if (i == workplaceLineCount - 1)
		{
			int res = productionLineCapacity % workplaceLineCount - 1;
			workplaceLineCapacity += res < 0 ? 0 : res;
		}

		this->workplaceLines.push_back(WorkplaceLine(workplaceLineCapacity, workplaceProcTime));
	}
}

ProductionLine::~ProductionLine()
{
}

bool ProductionLine::PutNewProduct(Product product)
{
	if (this->workplaceLines[0].GetProducts().size() <
		this->workplaceLines[0].GetWorkplaceLineCapacity())
	{
		if (this->workplaceLines[0].GetProducts().size() != 0)
		{
			std::vector<Product> products = this->workplaceLines[0].GetProducts();
			products.insert(products.begin(), product);
			this->workplaceLines[0].SetProducts(products);
		}
		else
		{
			std::vector<Product> products = this->workplaceLines[0].GetProducts();
			products.push_back(product);
			this->workplaceLines[0].SetProducts(products);
		}
		return true;
	}
	else if (!this->workplaceLines[0].GetWorkplace().GetIsBlocked() &&
		this->workplaceLines[0].GetWorkplace().GetIsProductProc() &&
		this->workplaceLines[0].GetWorkplace().GetIsEmpty())
	{
		this->workplaceLines[0].GetWorkplace().SetProcProduct(product);
		this->workplaceLines[0].GetWorkplace().SetIsEmpty(false);
		this->workplaceLines[0].GetWorkplace().SetIsProductProc(true);
		return true;
	}
	else
		return false;
}

void ProductionLine::DeferProduct(Product product)
{
	if (this->deferredProducts.size() != 0)
	{
		std::vector<Product> defProd = this->deferredProducts;
		defProd.insert(defProd.begin(), product);
		this->deferredProducts = defProd;
	}
	else
	{
		this->deferredProducts.push_back(product);
	}
}

bool ProductionLine::ReleaseProcessedProduct()
{
	if (this->workplaceLines[this->workplaceLines.size() - 1].
		GetWorkplace().GetIsProductProc())
	{
		Workplace wp = this->workplaceLines[this->workplaceLines.size() - 1].GetWorkplace();
		wp.SetIsEmpty(true);
		this->workplaceLines[this->workplaceLines.size() - 1].SetWorkplace(wp);
		return true;
	}
	else
		return false;
}

void ProductionLine::SetSimulationTime(unsigned long simulationTime)
{
	this->simulationTime = simulationTime;
}

unsigned long ProductionLine::GetSimulationTime()
{
	return this->simulationTime;
}

void ProductionLine::SetPutProductTimeInterval(unsigned long putProductTimeInterval)
{
	this->putProductTimeInterval = putProductTimeInterval;
}

unsigned long ProductionLine::GetPutProductTimeInterval()
{
	return this->putProductTimeInterval;
}

void ProductionLine::SetWorkplaceLines(std::vector<WorkplaceLine> workplaceLines)
{
	this->workplaceLines = workplaceLines;
}

std::vector<WorkplaceLine> ProductionLine::GetWorkplaceLines()
{
	return this->workplaceLines;
}

void ProductionLine::SetDeferredProducts(std::vector<Product> deferredProducts)
{
	this->deferredProducts = deferredProducts;
}

std::vector<Product> ProductionLine::GetDefferedProducts()
{
	return this->deferredProducts;
}

void ProductionLine::SetStatistics(Statistics statistics)
{
	this->statistics = statistics;
}

Statistics ProductionLine::GetStatistics()
{
	return this->statistics;
}
