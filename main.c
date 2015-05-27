#include <assert.h>
#include "lib/console.h"
#include "lib/map_reader.h"

#define TITLE_X 3
#define TITLE_Y 1

#define FIELD_PADDING 3
#define CHAR_ROCKET '1'
#define CHAR_BORDER '#'
#define CHAR_FIELD  ' '
#define CHAR_POINT  219
#define CHAR_UBLOCK 178
#define CHAR_BBLOCK 176

#define COLOR_BORDER	1
#define COLOR_FIELD		2
#define COLOR_POINT		3
#define COLOR_POINT_2	4
#define COLOR_UBLOCK	5
#define COLOR_BBLOCK	6

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
	con_initPair(COLOR_UBLOCK, CON_COLOR_BLUE, CON_COLOR_GREEN);
	con_initPair(COLOR_BBLOCK, CON_COLOR_RED, CON_COLOR_GREEN);
	con_initPair(COLOR_BORDER, CON_COLOR_BLACK, CON_COLOR_WHITE);
}

static void initial_draw() {
    con_clearScr();
    con_gotoXY(TITLE_X, TITLE_Y);
    con_outTxt("Use arrows to move point, use Esc to exit.");

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

/* Returns 1 if quit. */
int process_key(int key) {
    // position change
    int dx = 0;
    int dy = 0;
    int dz=0;

    switch (key) {
        case CON_KEY_ESCAPE:
            return 1;

        case CON_KEY_UP:
            if (point_y - 1 > field_y) {
                dy = -1;
            }
            break;

        case CON_KEY_DOWN:
            if (point_y + 1 < field_y + field_height - 1) {
                dy = 1;
            }
            break;

        case CON_KEY_LEFT:
            if (point_x - 1 > field_x) {
                dx = -1;
            }
            break;

        case CON_KEY_RIGHT:
            if (point_x + 1 < field_x + field_width - 1) {
                dx = 1;
            }
            break;
        case CON_KEY_ENTER:
            {if(point_y-1>field_y)
            dz=1;

            }
            break;
        }


    if (dx != 0 || dy != 0) {
        con_charAt(CHAR_FIELD, COLOR_FIELD, point_x, point_y);
        point_x += dx;
        point_y += dy;
        con_charAt(CHAR_POINT, COLOR_POINT, point_x, point_y);
    }
    if(dz!=0){
            int i;
            int t=point_y;
        for(i=0;i<field_height/2;i++){
            point_y=point_y-1;
            con_charAt(CHAR_FIELD,COLOR_FIELD,point_x,point_y+1);
            con_charAt(CHAR_ROCKET, COLOR_POINT, point_x, point_y);
            //con_charAt(' ',COLOR_POINT,point_x,point_y+i);
        }
        point_y=t;
        con_charAt(CHAR_POINT, COLOR_POINT, point_x, point_y);
    }
    return 0;
}

int main(int argc, char * argv[]) {
    int quit = 0;
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
	field = load_map(field_width - 2, field_height - 2, (argc == 2) ? argv[1] : "example.map");
    initial_draw();

    while (!quit) {
        if (con_keyPressed()) {
            if (process_key(con_getKey())) {
                quit = 1;
            }
        }
    }

    con_clearScr();
    con_deinit();
    return 0;
}
