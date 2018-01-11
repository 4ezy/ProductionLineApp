#include "ProductionLine.h"

ProductionLine::ProductionLine(unsigned int workplaceLineCount, unsigned int productionLineCapacity,
	unsigned long putProductTimeInterval, unsigned long workplaceProcTime, unsigned long simulationTime)
{
	this->putProductTimeInterval = putProductTimeInterval;
	this->simulationTime = simulationTime;

	int workplaceLineCapacity = productionLineCapacity / workplaceLineCount;

	for (int i = 0; i < workplaceLineCount; i++)
	{
		if (i == workplaceLineCount - 1)
			workplaceLineCapacity += productionLineCapacity % workplaceLineCount;

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
		this->workplaceLines[0].GetProducts().push(product);
		return true;
	}
	else
		return false;
}

void ProductionLine::DeferProduct(Product product)
{
	this->deferredProducts.push(product);
}

bool ProductionLine::ReleaseProcessedProduct()
{
	if (this->workplaceLines[this->workplaceLines.size() - 1].
		GetWorkplace().GetIsProductProc())
	{
		this->workplaceLines[this->workplaceLines.size() - 1].GetWorkplace().SetIsEmpty(true);
		this->workplaceLines[this->workplaceLines.size() - 1].GetWorkplace().SetIsProductProc(false);
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

void ProductionLine::SetDeferredProducts(std::queue<Product> deferredProducts)
{
	this->deferredProducts = deferredProducts;
}

std::queue<Product> ProductionLine::GetDefferedProducts()
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