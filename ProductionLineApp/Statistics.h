#pragma once

struct Statistics
{
	unsigned long fstWorkplaceLoad;	// ������� �������� ������� ����
	unsigned long sndWorkplaceLoad;
	unsigned long avgProdProcTime;	// � �������� (����� ������� �������� �����������, �������� 2,17 ������)
	int deferredProductsCount;
	int fstWorkplaceAvgQueue;
	int sndWorkplaceAvgQueue;
	unsigned long fstWorkplaceBlockingTime;	// � �������� (����� ������� �������� �����������, �������� 2,17 ������)
};