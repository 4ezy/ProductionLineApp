#pragma once

struct ProductionLineStatistics
{
	double firstWorkplaceLoad;	// �������� - � ���������
	double secondWorkplaceLoad;
	double avgProdProcTime;	// � �������� (����� ������� �������� �����������, �������� 2,17 ������)
	int deferredProductsCount;
	int firstWorkplaceAvgQueue;
	int secondWorkplaceAvgQueue;
	double firstWorkplaceBlockingTime;	// � �������� (����� ������� �������� �����������, �������� 2,17 ������)
};