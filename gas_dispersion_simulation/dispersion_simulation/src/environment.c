#include "dispersion_simulation/environment.h"
#include<string.h>
#include <stdlib.h>
#include <math.h>

// Initializes the environment.
void environment_init(struct sEnvironment *environment, int max_x, int max_y,
		int max_z, double cell_size, char const *filePath) {
	//double cell_size=1;

	environment->lines.count = 0;
	environment->rectangles.count = 0;

	environment->arena.width = max_x; //sepid 18;
	environment->arena.height = max_y; //sepid 10;
	environment->arena.length = max_z;

	/*environment_lines_add(environment, 0, 0, 16, 0);
	 environment_lines_add(environment, 16, 0, 16, 4);
	 environment_lines_add(environment, 0, 0, 0, 4);
	 environment_lines_add(environment, 0, 4, 16, 4);

	 environment_lines_add(environment, 6, 1.2, 6, 2);
	 environment_lines_add(environment, 6, 2, 8, 2);
	 environment_lines_add(environment, 8, 2, 8, 1.2);
	 environment_lines_add(environment, 8, 1.2, 6, 1.2);*/

	//open file
	FILE* file_ptr;
	file_ptr = fopen(filePath, "r");
	if (file_ptr == NULL)
		printf("Error in opening the environment file.\n");

	char ch;

	file_ptr = fopen(filePath, "r");

	double x_pos, y_pos;
	x_pos = y_pos = 0;
	int block_idx = 0;

	do {
		ch = getc(file_ptr);
		if (ch == '1' || ch == '0') {

			if (ch == '1') {

				environment_rectangles_add(environment, x_pos, y_pos,
						x_pos + cell_size, y_pos + cell_size);

				block_idx++;


			}
			y_pos = y_pos + cell_size;

		}

		else if (ch == ';') {
			x_pos = 0;
			y_pos = 0;

			ch = getc(file_ptr);
		} else if (ch == '\n' || ch == '\r') {
			y_pos = 0;
			x_pos = x_pos + cell_size;
		}

	} while (ch != EOF);

	fclose(file_ptr);

	/*environment_rectangles_add(environment, 14, 12, 15, 13);
	 environment_rectangles_add(environment, 15, 13, 16, 14);
	 environment_rectangles_add(environment, 44, 5, 47, 8);*/

	/*
	 environment_lines_add(environment, 0, 10, 10, 10);
	 environment_lines_add(environment, 18, 1, 18, 2);
	 environment_lines_add(environment, 18, 4, 18, 9);
	 environment_rectangles_add(environment, 6, 0, 18, 1);
	 environment_rectangles_add(environment, 10, 9, 18, 10);
	 environment_rectangles_add(environment, 11.5, 4.5, 14.5, 6);
	 environment_rectangles_add(environment, 3, 4.5, 8, 7);
	 environment_rectangles_add(environment, 3, 4.5, 5, 8);
	 environment_rectangles_add(environment, 6, 3.5, 8, 7);*/
}

// Adds a line.
void environment_lines_add(struct sEnvironment *environment, double x1,
		double y1, double x2, double y2) {
	if (environment->lines.count >= 32) {
		printf("Not enough memory allocated for lines.\n");
		exit(1);
	}

	int index = environment->lines.count;
	environment->lines.item[index].x1 = x1;
	environment->lines.item[index].y1 = y1;
	environment->lines.item[index].x2 = x2;
	environment->lines.item[index].y2 = y2;
	environment->lines.count++;
}

// Adds a rectangle.
void environment_rectangles_add(struct sEnvironment *environment, double x1,
		double y1, double x2, double y2) {
	if (environment->rectangles.count >= 20000) {
		printf("Not enough memory allocated for rectangles.\n");
		exit(1);
	}

	int index = environment->rectangles.count;
	environment->rectangles.item[index].x1 = x1;
	environment->rectangles.item[index].y1 = y1;
	environment->rectangles.item[index].x2 = x2;
	environment->rectangles.item[index].y2 = y2;
	environment->rectangles.count++;
}

/*
 if ((x < 0) || (y < 0) || (z < 0)|| (x > environment->arena.width) || (y > environment->arena.height)|| (z > environment->arena.length)) {
 return 0;
 }


 */

// Returns whether a point (world coordinates) is valid or not.
int environment_position_valid(struct sEnvironment *environment, double x,
		double y, double z) {
	// Arena
	if ((z < 0) || (z >= environment->arena.length - 1)
			|| ((x < 0) || (x >= environment->arena.width - 1))
			|| ((y < 0) || (y >= environment->arena.height - 1))) {

		return 0;
	}

	// Rectangles
	int i;
	for (i = 0; i < environment->rectangles.count; i++) {
		if ((x >= environment->rectangles.item[i].x1)
				&& (y >= environment->rectangles.item[i].y1)
				&& (x <= environment->rectangles.item[i].x2)
				&& (y <= environment->rectangles.item[i].y2)) {
			return 0;
		}
	}

	return 1;
}
