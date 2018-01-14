#include "BubbleSort.h"
#include "SelectionSort.h"
#include "InsertionSort.h"
#include "MergeSort.h"
#include "QuickSort.h"
#include <iostream>
#include <vector>

using namespace std;

enum class SORT
{
	BUBBLESORT,
	SELECTIONSORT,
	INSERTIONSORT,
	MERGESORT,
	QUICKSORT,
};

int main()
{
	int _iNum = 0;
	cout << "Input Sort : ";
	cin >> _iNum;

	switch (_iNum)
	{
		case static_cast<int>(SORT::BUBBLESORT):
		{

		}
		break;
		case  static_cast<int>(SORT::SELECTIONSORT):
		{

		}
		break;
		case  static_cast<int>(SORT::INSERTIONSORT):
		{
			
		}
		break;
		case  static_cast<int>(SORT::MERGESORT):
		{

		}
		break;
		case  static_cast<int>(SORT::QUICKSORT):
		{

		}
		break;
	default:
		break;
	}
}