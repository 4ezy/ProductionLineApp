#pragma once

struct ProductionLineStatistics
{
	double firstWorkplaceLoad;	// идеально - в процентах
	double secondWorkplaceLoad;
	double avgProdProcTime;	// в секундах (после запятой проценты миллисекунд, например 2,17 секунд)
	int deferredProductsCount;
	int firstWorkplaceAvgQueue;
	int secondWorkplaceAvgQueue;
	double firstWorkplaceBlockingTime;	// в секундах (после запятой проценты миллисекунд, например 2,17 секунд)
};