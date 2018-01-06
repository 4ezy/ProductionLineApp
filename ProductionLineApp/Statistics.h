#pragma once

struct Statistics
{
	float fstWorkplaceLoad;	// процент загрузки рабочих мест
	float sndWorkplaceLoad;
	float avgProdProcTime;	// в секундах (после запятой проценты миллисекунд, например 2,17 секунд)
	int deferredProductsCount;
	int fstWorkplaceAvgQueue;
	int sndWorkplaceAvgQueue;
	float fstWorkplaceBlockingTime;	// в секундах (после запятой проценты миллисекунд, например 2,17 секунд)
};