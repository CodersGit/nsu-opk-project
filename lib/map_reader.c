#include "map_reader.h"
#include <stdio.h>
#include <malloc.h>

int load_map(int width, int height, char* file_name, int*** result) {
	int** arr = (int**)malloc(sizeof(int*) * width);
	for (int i = 0; i < width; i++) {
		arr[i] = (int*)malloc(sizeof(int) * height);
	}
	int i, j, errcode;
	for (i = 0; i<width; i++){
		for (j = 0; j<height; j++){
			arr[i][j] = 0;
		}
	}
	FILE *file;
	file = fopen(file_name, "r");
	if (file == NULL){
		free(arr);
		return 1;
	}
	int x, y, data;
	while (!feof(file))
		if (fscanf(file, "%d %d %d", &x, &y, &data) == 3  && (data == 2 || data == 1)) {
			if ((x > width || y > height || x < 1 || y < 1) && ((x <= 2 && y >= height - 2) || (x >= width && y <= 2)))
				continue;
			arr[x - 1][y - 1] = data;
		} else {
			errcode = 2;
			free(arr);
			break;
		}
	fclose(file);
	*result = arr;
	return errcode;
}
