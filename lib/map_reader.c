#include "map_reader.h"
#include <stdio.h>
#include <malloc.h>

int** load_map(int width, int height, char* file_name) {
	int** arr = (int**)malloc(sizeof(int*) * width);
	for (int i = 0; i < width; i++) {
		arr[i] = (int*)malloc(sizeof(int) * height);
	}
	int i, j;
	for (int i = 0; i<width; i++){
		for (j = 0; j<height; j++){
			arr[i][j] = '0';
		}
	}
	FILE *file;
	file = fopen(file_name, "r");
	if (file == NULL){
		printf("couldn't open %s\n", file_name);
		return arr;
	}
	int x, y, data;
	while (!feof(file))
		if (fscanf(file, "%d %d %d", &x, &y, &data) == 3  && (data == 2 || data == 1)) {
			if (x > width || y > height || x < 1 || y < 1)
				continue;
			arr[x - 1][y - 1] = data;
		} else {
			printf("Map file incorrect or is damaged");
		}
	fclose(file);
	return arr;
}
