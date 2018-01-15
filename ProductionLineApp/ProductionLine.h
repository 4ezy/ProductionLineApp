#pragma once
#include <vector>
#include "WorkplaceLine.h"
#include "Product.h"
#include "Statistics.h"

class ProductionLine
{
public:
	ProductionLine();
	ProductionLine(unsigned int workplaceLineCount, unsigned int workplaceLineCapacity,
		unsigned long putProductTimeInterval, unsigned long workplaceProcTime, unsigned long simulationTime);
	~ProductionLine();

	bool PutNewProduct(Product product);
	void DeferProduct(Product product);
	bool ReleaseProcessedProduct();

	void SetSimulationTime(unsigned long simulationTime);
	unsigned long GetSimulationTime();
	void SetPutProductTimeInterval(unsigned long putProductTimeInterval);
	unsigned long GetPutProductTimeInterval();
	void SetWorkplaceLines(std::vector<WorkplaceLine> workplaceLines);
	std::vector<WorkplaceLine> GetWorkplaceLines();
	std::vector<Product> GetDefferedProducts();
	void SetStatistics(Statistics statistics);
	Statistics GetStatistics();
private:
	unsigned long simulationTime;
	unsigned long putProductTimeInterval;
	std::vector<WorkplaceLine> workplaceLines;
	std::vector<Product> deferredProducts;
	Statistics statistics;
	void SetDeferredProducts(std::vector<Product> deferredProducts);
};