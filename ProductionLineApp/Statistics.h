#pragma once

struct Statistics
{
	double fstWorkplaceLoad;	// процент загрузки рабочих мест
	double sndWorkplaceLoad;
	double avgProdProcTime;	// в секундах (после запятой проценты миллисекунд, например 2,17 секунд)
	int deferredProductsCount;
	int fstWorkplaceAvgQueue;
	int sndWorkplaceAvgQueue;
	double fstWorkplaceBlockingTime;	// в секундах (после запятой проценты миллисекунд, например 2,17 секунд)
};