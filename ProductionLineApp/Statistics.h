#pragma once

struct Statistics
{
	long double fstWorkplaceLoad;	// процент загрузки рабочих мест
	long double sndWorkplaceLoad;
	unsigned long avgProdProcTime;	// миллисекундах
	unsigned long deferredProductsCount;
	long double fstWorkplaceAvgQueue;
	long double sndWorkplaceAvgQueue;
	long double fstWorkplaceBlockingTime;	// миллисекундах
};