#include <assert.h>
#include <windows.h>
#include <stdbool.h>
#include <process.h>
#include "lib/console.h"
#include "lib/map_reader.h"

#define TITLE_X 3
#define TITLE_Y 1

#define FIELD_PADDING	3
#define CHAR_ROCKET		'1'
#define CHAR_BORDER		'#'
#define CHAR_FIELD		' '
#define CHAR_POINT		219
#define CHAR_POINT_3	220
#define CHAR_UBLOCK		178
#define CHAR_BBLOCK		177

#define COLOR_BORDER	1
#define COLOR_FIELD		2
#define COLOR_POINT		3
#define COLOR_POINT_2	4
#define COLOR_UBLOCK	5
#define COLOR_BBLOCK	6
#define COLOR_POINT_3	7
#define COLOR_CONSOLE	8

static int field_x, field_y; // top-left corner
static int field_width, field_height;
static int point_x, point_y;
static int point_x2, point_y2;
static int** field;

/* Output char using given color pair at given position. */
static void con_charAt(int ch, int color, int x, int y) {
    con_gotoXY(x, y);
    con_setColor(color);
    con_outTxt("%c", ch);
}

static void init_colors() {
	con_initPair(COLOR_FIELD, CON_COLOR_GREEN, CON_COLOR_GREEN);
	con_initPair(COLOR_POINT, CON_COLOR_CYAN, CON_COLOR_GREEN);
	con_initPair(COLOR_POINT_2, CON_COLOR_YELLOW, CON_COLOR_GREEN);
	con_initPair(COLOR_POINT_3, CON_COLOR_CYAN, CON_COLOR_YELLOW);
	con_initPair(COLOR_UBLOCK, CON_COLOR_BLUE, CON_COLOR_GREEN);
	con_initPair(COLOR_BBLOCK, CON_COLOR_RED, CON_COLOR_GREEN);
	con_initPair(COLOR_BORDER, CON_COLOR_BLACK, CON_COLOR_WHITE);
	con_initPair(COLOR_CONSOLE, CON_COLOR_WHITE, CON_COLOR_BLACK);
}

static void initial_draw() {
    con_clearScr();
    con_gotoXY(TITLE_X, TITLE_Y);
	con_outTxt("Yellow player uses W, A, S, D, Space to plant bomb; cyan - arrows + Enter to plant bomb");

    {
        int i, j;
        for (i = 0; i < field_width; ++i) {
            for (j = 0; j < field_height; ++j) {
                int ch;
                int color;
				if (i == 0 || i == field_width - 1 || j == 0 || j == field_height - 1) {
                    ch = CHAR_BORDER;
                    color = COLOR_BORDER;
                } else {
					switch (field[i - 1][j - 1]) {
					case 2:
						ch = CHAR_UBLOCK;
						color = COLOR_UBLOCK;
						break;
					case 1:
						ch = CHAR_BBLOCK;
						color = COLOR_BBLOCK;
						break;
					case 0:
					default:
						ch = CHAR_FIELD;
						color = COLOR_FIELD;
					break;
					}
                }
				con_charAt(ch, color, field_x + i, field_y + j);
            }
        }
    }

	point_x = field_x + 1;
	point_y = field_y + field_height - 2;
	con_charAt(CHAR_POINT, COLOR_POINT, point_x, point_y);
	point_x2 = field_x + field_width - 2;
	point_y2 = field_y + 1;
	con_charAt(CHAR_POINT, COLOR_POINT_2, point_x2, point_y2);
}
int player_derection_1 = 0, player_derection_2 = 0;
bool player_planted_bomb_1 = false, player_planted_bomb_2 = false;

int process_key(int key) {

	switch (key) {

	case CON_KEY_UP:
		player_derection_1 = 1;
		break;

	case CON_KEY_DOWN:
		player_derection_1 = 2;
		break;

	case CON_KEY_LEFT:
		player_derection_1 = 3;
		break;

	case CON_KEY_RIGHT:
		player_derection_1 = 4;
		break;
	case CON_KEY_ENTER:
		player_planted_bomb_1 = true;
		break;

	case CON_KEY_W:
		player_derection_2 = 1;
		break;

	case CON_KEY_S:
		player_derection_2 = 2;
		break;

	case CON_KEY_A:
		player_derection_2 = 3;
		break;

	case CON_KEY_D:
		player_derection_2 = 4;
		break;
	case CON_KEY_SPACE:
		player_planted_bomb_2 = true;
		break;
	}
	return 0;
}

bool player_dead_1 = false, player_dead_2 = false, game_over = false;
void refresh_player_1() {
	int dx = 0;
	int dy = 0;
	int dz = 0;

	switch (player_derection_1) {
	case 1:
		if (point_y - 1 > field_y && field[point_x - field_x - 1][point_y - field_y - 1 - 1] == 0) {
			dy = -1;
		}
		break;

	case 2:
		if (point_y + 1 < field_y + field_height - 1 && field[point_x - (field_x + 1)][point_y + 1 - (field_y + 1)] == 0) {
			dy = 1;
		}
		break;

	case 3:
		if (point_x - 1 > field_x && field[point_x - 1 - (field_x + 1)][point_y - (field_y + 1)] == 0) {
			dx = -1;
		}
		break;

	case 4:
		if (point_x + 1 < field_x + field_width - 1 && field[point_x + 1 - (field_x + 1)][point_y - (field_y + 1)] == 0) {
			dx = 1;
		}
		break;
	}

	if (player_planted_bomb_1) {
		field[point_x - field_x - 1][point_y - field_y - 1] = -60;
		player_planted_bomb_1 = false;
		return;
	}

	if (dx != 0 || dy != 0) {
		con_charAt(CHAR_FIELD, COLOR_FIELD, point_x, point_y);
		point_x += dx;
		point_y += dy;
		con_charAt(CHAR_POINT, COLOR_POINT, point_x, point_y);
	}
	if (field[point_x - field_x - 1][point_y - field_y - 1] < -60)
		player_dead_1 = true;
}

void refresh_player_2() {
	int dx = 0;
	int dy = 0;
	int dz = 0;

	switch (player_derection_2) {
	case 1:
		if (point_y2 - 1 > field_y && field[point_x2 - field_x - 1][point_y2 - field_y - 1 - 1] == 0) {
			dy = -1;
		}
		break;

	case 2:
		if (point_y2 + 1 < field_y + field_height - 1 && field[point_x2 - (field_x + 1)][point_y2 + 1 - (field_y + 1)] == 0) {
			dy = 1;
		}
		break;

	case 3:
		if (point_x2 - 1 > field_x && field[point_x2 - 1 - (field_x + 1)][point_y2 - (field_y + 1)] == 0) {
			dx = -1;
		}
		break;

	case 4:
		if (point_x2 + 1 < field_x + field_width - 1 && field[point_x2 + 1 - (field_x + 1)][point_y2 - (field_y + 1)] == 0) {
			dx = 1;
		}
		break;
	}

	if (player_planted_bomb_2) {
		field[point_x2 - field_x - 1][point_y2 - field_y - 1] = -60;
		player_planted_bomb_2 = false;
		return;
	}

	bool specified = (point_x == point_x2 && point_y == point_y2) ? true : false;
	if (dx != 0 || dy != 0) {
		(specified) ? con_charAt(CHAR_FIELD, COLOR_FIELD, point_x, point_y) : con_charAt(CHAR_FIELD, COLOR_FIELD, point_x2, point_y2);
		point_x2 += dx;
		point_y2 += dy;
		specified = (point_x == point_x2 && point_y == point_y2) ? true : false;
	}
	con_charAt((specified) ? CHAR_POINT_3 : CHAR_POINT, (specified) ? COLOR_POINT_3 : COLOR_POINT_2, point_x2, point_y2);
	if (field[point_x2 - field_x - 1][point_y2 - field_y - 1] < -60)
		player_dead_2 = true;
}

void bomb_blow(int x, int y, int d, char axis) {
	if (axis == 1) {
		for (int i = 0; abs(i*d) < 5 && x + i*d >= 0 && x + i*d < field_width - 2 && field[x + i*d][y] != 2; i++) {
			field[x + i*d][y] = -70;
			con_charAt(CHAR_POINT, COLOR_BORDER, field_x + 1 + x + i*d, field_y + 1 + y);
		}
	} else {
		for (int i = 0; abs(i*d) < 5 && y + i*d >= 0 && y + i*d < field_height - 2 && field[x][y + i*d] != 2; i++) {
			field[x][y + i*d] = -70;
			con_charAt(CHAR_POINT, COLOR_BORDER, field_x + 1 + x, field_y + 1 + y + i*d);
		}
	}
}

void repaint() {
	for (int i = 0; i < field_width - 2; i++)
		for (int j = 0; j < field_height - 2; j++) {
			if (field[i][j] < 0) {
				switch (field[i][j])
				{
				case -1:
					bomb_blow(i, j, -1, 0);
					bomb_blow(i, j, 1, 0);
					bomb_blow(i, j, 1, 1);
					bomb_blow(i, j, -1, 1);
					break;
				case -61:
					field[i][j] = 0;
					con_charAt(CHAR_FIELD, COLOR_FIELD, field_x + 1 + i, field_y + 1 + j);
					break;
				default:
					(abs(field[i][j])%10 < 5)? 
						con_charAt((field[i][j] < -60) ? CHAR_POINT : CHAR_ROCKET, (field[i][j] < -60) ? COLOR_BORDER : COLOR_BBLOCK, field_x + 1 + i, field_y + 1 + j) :
					con_charAt(CHAR_FIELD, COLOR_FIELD, field_x + 1 + i, field_y + 1 + j);
					field[i][j]++;
					break;
				}
			}
		}
	refresh_player_1();
	refresh_player_2();
	if (player_dead_1 == true || player_dead_2 == true)
		game_over = true;
}

void thread_key_listener(void* unused) {
	while (1) {
		if (con_keyPressed()) {
			process_key(con_getKey());
		}
	}
}

int main(int argc, char * argv[]) {
	if (argc > 1 && !(strcmp(argv[1], "help") && strcmp(argv[1], "-h"))) {
		printf("The bomberman game v1.0\n\n* You can run it like \"%s filename.map\" to load a custom map or without arguments to use map from example.map\n* run with key \"about\" to view credits.", argv[0]);
		return 0;
	}
	if (argc > 1 && !(strcmp(argv[1], "info") && strcmp(argv[1], "-v") && strcmp(argv[1], "about"))) {
		printf("The bomberman game v1.0 (c) Zhirov Sergey, 2015\n\nWroted as a project for a NSU BPC cource.\n\nThanks:\n Alexander Senchenko - seminarian\n Ivan Ashkarin - for inspiration\n Hudson Soft company - for idea.\n\nAlso you can visit my blog http://blogofcoder.ru/ \nHave fun!\n");
		return 0;
	}

    int max_x, max_y;

    con_init();
    con_hideCursor();

    init_colors();

    // calculate size of field
    con_getMaxXY(&max_x, &max_y);
    field_x = FIELD_PADDING + TITLE_Y + 1;
    field_y = FIELD_PADDING;
	field_width = max_x - field_x - FIELD_PADDING;
    field_height = max_y - field_y - FIELD_PADDING;
    assert(field_width > 2);
    assert(field_height > 2);
	int err;
	char* map = (argc > 1) ? argv[1] : "example.map";
	if (err = load_map(field_width - 2, field_height - 2, map, &field)) {
		con_clearScr();
		con_deinit();
		switch (err)
		{
		case 1:
			printf("Can't open map \"%s\": no such file or directory.\n", map);
			break;
		case 2:
			printf("Can't open map \"%s\": file have been damaged.\n", map);
			break;
		}
		return 0;
	}
    initial_draw();
	_beginthread(thread_key_listener, 0, NULL);
    while (!game_over) {
		repaint();
		Sleep(50);
    }

	if (player_dead_1 && player_dead_2){
		con_gotoXY(field_x + field_width / 2 - 5, field_y + field_height / 2);
		con_outTxt("Round draw");
	} else if (player_dead_1) {
		con_gotoXY(field_x + field_width / 2 - 16, field_y + field_height / 2);
		con_outTxt("Yellow player have won this round");
	} else {
		con_gotoXY(field_x + field_width / 2 - 15, field_y + field_height / 2);
		con_outTxt("Cyan player have won this round");
	}
	con_charAt(CHAR_FIELD, COLOR_CONSOLE, 0, 0);
	Sleep(10000);
    con_clearScr();
    con_deinit();
    return 0;
}
