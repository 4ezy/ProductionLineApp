#pragma once

struct Statistics
{
	unsigned long fstWorkplaceLoad;	// процент загрузки рабочих мест
	unsigned long sndWorkplaceLoad;
	unsigned long avgProdProcTime;	// миллисекундах
	unsigned long deferredProductsCount;
	unsigned long fstWorkplaceAvgQueue;
	unsigned long sndWorkplaceAvgQueue;
	unsigned long fstWorkplaceBlockingTime;	// миллисекундах
};