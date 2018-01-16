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
			this->workplaceLines[0].GetProductsRef()->insert(
				this->workplaceLines[0].GetProductsRef()->begin(), product);
		}
		else
		{
			this->workplaceLines[0].GetProductsRef()->push_back(product);
		}
		return true;
	}
	else
		return false;
}

void ProductionLine::DeferProduct(Product product)
{
	if (this->deferredProducts.size() != 0)
	{
		this->deferredProducts.insert(this->deferredProducts.begin(), product);
	}
	else
	{
		this->deferredProducts.push_back(product);
	}
}

bool ProductionLine::ReleaseProcessedProduct()
{
	if (!this->workplaceLines[this->workplaceLines.size() - 1].GetWorkplace().GetIsEmpty() &&
		this->workplaceLines[this->workplaceLines.size() - 1].GetWorkplace().GetProcProduct().isProcessed)
	{
		this->workplaceLines[this->workplaceLines.size() - 1].GetWorkplaceRef()->SetIsEmpty(true);
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

std::vector<WorkplaceLine>* ProductionLine::GetWorkplaceLinesRef()
{
	return &this->workplaceLines;
}

void ProductionLine::SetDeferredProducts(std::vector<Product> deferredProducts)
{
	this->deferredProducts = deferredProducts;
}

std::vector<Product> ProductionLine::GetDeferredProducts()
{
	return this->deferredProducts;
}

std::vector<Product>* ProductionLine::GetDeferredProductsRef()
{
	return &this->deferredProducts;
}

void ProductionLine::SetStatistics(Statistics statistics)
{
	this->statistics = statistics;
}

Statistics ProductionLine::GetStatistics()
{
	return this->statistics;
}
