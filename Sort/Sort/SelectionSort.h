#pragma once
#include <vector>

void SelectionSort(vector<int> v)
{
	for (int i = 0; i < v.size() - 1; i++)
	{
		int _temp = 1;
		for (int j = i + 1; j < v.size(); j++)
		{
			if (v[_temp] >= v[j])
				_temp = j;
		}
		swap(v[i], v[_temp]);
	}
}