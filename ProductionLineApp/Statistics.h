#pragma once

struct Statistics
{
	float fstWorkplaceLoad;	// ������� �������� ������� ����
	float sndWorkplaceLoad;
	float avgProdProcTime;	// � �������� (����� ������� �������� �����������, �������� 2,17 ������)
	int deferredProductsCount;
	int fstWorkplaceAvgQueue;
	int sndWorkplaceAvgQueue;
	float fstWorkplaceBlockingTime;	// � �������� (����� ������� �������� �����������, �������� 2,17 ������)
};