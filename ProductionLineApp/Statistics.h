#pragma once

struct Statistics
{
	double fstWorkplaceLoad;	// ������� �������� ������� ����
	double sndWorkplaceLoad;
	double avgProdProcTime;	// � �������� (����� ������� �������� �����������, �������� 2,17 ������)
	int deferredProductsCount;
	int fstWorkplaceAvgQueue;
	int sndWorkplaceAvgQueue;
	double fstWorkplaceBlockingTime;	// � �������� (����� ������� �������� �����������, �������� 2,17 ������)
};