#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include "BMP.h"
#include "filter.h"
#include <chrono>

using namespace chrono;
using namespace std;

extern "C"
{
	void bmp_filtering(char* src, char* dst, int line_size, int start_line, int stop_line, int width, int height);
}

int main()
{
	BMP bmp("field.bmp");
	uint32_t rowSize = ((bmp.info_header.bits_per_pixel * bmp.info_header.width + 31) / 32) * 4;
	char* result = new char[rowSize * bmp.info_header.height];

	int choice = 0;
	while (choice != 3)
	{
		cout << "ASM: 1, C++: 2, koniec: 3" << endl;
		cin >> choice;

		if (choice == 1)
		{
			auto start = high_resolution_clock::now();

			bmp_filtering(bmp.data, result, rowSize, 0, (int)bmp.info_header.height - 1, (int)bmp.info_header.width, (int)bmp.info_header.height);
			bmp.save(result, bmp.file_header, bmp.info_header, "output_asm.BMP");

			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<microseconds>(stop - start);
			chrono::duration<long, micro> toInt = duration;

			cout << "Czas wykonania w ASM: " << toInt.count() << "ms" << endl << endl;
		}
		if (choice == 2)
		{
			auto start = high_resolution_clock::now();

			result = filter(bmp, rowSize);
			bmp.save(result, bmp.file_header, bmp.info_header, "output_cpp.BMP");

			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<microseconds>(stop - start);
			chrono::duration<long, micro> toInt = duration;

			cout << "Czas wykonania w C++: " << toInt.count() << "ms" << endl << endl;
		}
	}
}
