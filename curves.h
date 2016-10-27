#include <vector>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>

using namespace std;

#define N 1001

struct node {
	float x, y;
	bool sel;
};

enum MENU
{
	MENU_CLEAR,
	MENU_NODE,
	MENU_SHOW_HIDE,
	MENU_REMOVE,
	MENU_CURVE,
	MENU_ADD,
	MENU_DIV,
};

