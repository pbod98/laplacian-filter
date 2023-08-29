#pragma once

char* filter(BMP bitmap, int row_size)
{
	char* result = new char[row_size * bitmap.info_header.height];

	for (uint32_t i = 1; i < bitmap.info_header.height - 1; i++)
	{
		for (uint32_t j = 3; j < (bitmap.info_header.width * 3 - 3); j++)
		{
			int central = (4 * static_cast<uint8_t>(bitmap.data[i * row_size + j]));			
			int up = (-1 * static_cast<uint8_t>(bitmap.data[(i - 1) * row_size + j]));			
			int left = (-1 * static_cast<uint8_t>(bitmap.data[i * row_size + (j - 3)]));		
			int right = (-1 * static_cast<uint8_t>(bitmap.data[i * row_size + (j + 3)]));		
			int down = (-1 * static_cast<uint8_t>(bitmap.data[(i + 1) * row_size + j]));		

			int sum = central + up + left + right + down;

			if (sum > 255)
				sum = 255;
			if (sum < 0)
				sum = 0;

			result[i * row_size + j] = sum;
		}
	}

	return result;
}