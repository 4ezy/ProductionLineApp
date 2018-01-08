#pragma once

struct Statistics
{
	unsigned long fstWorkplaceLoad;	// процент загрузки рабочих мест
	unsigned long sndWorkplaceLoad;
	unsigned long avgProdProcTime;	// в секундах (после запятой проценты миллисекунд, например 2,17 секунд)
	int deferredProductsCount;
	int fstWorkplaceAvgQueue;
	int sndWorkplaceAvgQueue;
	unsigned long fstWorkplaceBlockingTime;	// в секундах (после запятой проценты миллисекунд, например 2,17 секунд)
};