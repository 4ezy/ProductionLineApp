#pragma once

struct Statistics
{
	unsigned long fstWorkplaceLoad;	// ������� �������� ������� ����
	unsigned long sndWorkplaceLoad;
	unsigned long avgProdProcTime;	// �������������
	unsigned long deferredProductsCount;
	unsigned long fstWorkplaceAvgQueue;
	unsigned long sndWorkplaceAvgQueue;
	unsigned long fstWorkplaceBlockingTime;	// �������������
};