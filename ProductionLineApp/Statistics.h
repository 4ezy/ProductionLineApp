#pragma once

struct Statistics
{
	long double fstWorkplaceLoad;	// ������� �������� ������� ����
	long double sndWorkplaceLoad;
	unsigned long avgProdProcTime;	// �������������
	unsigned long deferredProductsCount;
	long double fstWorkplaceAvgQueue;
	long double sndWorkplaceAvgQueue;
	long double fstWorkplaceBlockingTime;	// �������������
};