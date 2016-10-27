#include "curves.h"

bool show_nodes=true;
bool draw_curve=false;
bool f_div = false;
bool even = false;
int w = 700;
int h = 700;
int X, Y;
int _size;
char buf[50]={0};
float _t = -1;

vector<node> nodes;
vector<node> d_nodes;
vector<int> combinations;
node c_nodes[N];

int cur_node=-1;

int f(int i, int n) {
	int s = 1, j;
	for(j = 1; j <= n-i; j++)
		s = s * (i + j) / j;
	return s;
}

void constr_curve(vector<node> &nodes, int w, int h, int _size, int pos = 0)
{
	float step = 0.001, t, b;
	int i, idx = 0;

	memset(c_nodes, 0, sizeof(node) * N);
	
	if(combinations.size() != _size) {
		combinations.resize(_size);
		for(i = 0; i < _size; i++)
			combinations[i] = f(i, _size - 1);
	}

	for(t = 0; t < 1.0; t+=step) {
		for(i = 0; i < _size; i++) {
			b = combinations[i] * pow(t,i) * pow(1 - t,_size - 1 - i);
			c_nodes[idx].x += nodes[pos+i].x * b;
			c_nodes[idx].y += nodes[pos+i].y * b;
		}
		idx++;
	}

	glColor3f(0, 0, 0.5);
	glLineWidth(1.1);
	glBegin(GL_LINE_STRIP);
		for(i = 0; i < N; i++)
		{
			glVertex2f(c_nodes[i].x - w/2, c_nodes[i].y - h/2);
		}
	glEnd();
}

void recomp_base_nodes()
{
	node prev = {0,0,0}, cur = {0,0,0};
	float k;

	nodes.resize(nodes.size() + 1);

	for(int i = 0; i < nodes.size(); i++) {
		k = (float)i / (nodes.size() - 1);
		if(k) {
			cur.x += k * prev.x;
			cur.y += k * prev.y;
		}
		if(1.0 - k) {
			cur.x += (1.0 - k) * nodes[i].x;
			cur.y += (1.0 - k) * nodes[i].y;
		}
		prev = nodes[i];
		nodes[i] = cur;
		cur.x = cur.y = cur.sel = 0;
	}
}

void draw_node(float x, float y, bool color)
{
	if(!color) glColor3f(0.5, 0.0, 0);
	else glColor3f(0, 0.5, 0);
	glLineWidth(0.1);
	x -= w/2; y -= h/2;
	glBegin(GL_POLYGON);
		glVertex2f(x,y+4);
		glVertex2f(x+4, y);
		glVertex2f(x, y-4);
		glVertex2f(x-4, y);
	glEnd();
}

void draw_line(vector <node> &nodes, int beg, int end, bool color)
{
	color==false ? glColor3f(0, 0.5, 0) : glColor3f(0.5, 0, 0);
	glLineWidth(0.4);
	glBegin(GL_LINE_STRIP);
		for(int i=beg; i<end; i++) glVertex2f(nodes[i].x-w/2, nodes[i].y-h/2);
	glEnd();
}

void draw()
{
	int sz = nodes.size();
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.5);
	glLineWidth(0.1);
	if(show_nodes) {
		if(f_div) {
			draw_line(d_nodes, 0, sz, 0);
			draw_line(d_nodes, sz, 2*sz, 1);

			for(int i=0; i<sz; i++)
				draw_node(d_nodes[i].x, d_nodes[i].y, d_nodes[i].sel);
			for(int i=sz; i<2*sz; i++)
				draw_node(d_nodes[i].x, d_nodes[i].y, d_nodes[i].sel); 
		}
		else {
			draw_line(nodes, 0, sz, 0);	

			for(vector<node>::iterator i = nodes.begin(); i!=nodes.end(); i++)
			draw_node(i->x, i->y, i->sel);
		}
	}
	sprintf(buf, "Number of nodes: %d", f_div==false ? sz : 2*sz -1);
	glutSetWindowTitle(buf);
	
	if(draw_curve){
		if(f_div) { constr_curve(d_nodes, w, h, sz);
					constr_curve(d_nodes, w, h, sz, sz); }
		else constr_curve(nodes, w, h, sz);
	}

	glFlush();
}

void split_curve()
{
	float min = 20, temp; 

	for(int i = 0; i < N; i++) {
		temp = (X - c_nodes[i].x)*(X - c_nodes[i].x) + (Y - c_nodes[i].y)*(Y - c_nodes[i].y);
		if(temp <= min) {
			_t = (float)i / (N - 1); 
			min = temp;
		}
	}
	
	if(_t < 0) return;

	f_div = true;
	int _size = nodes.size();
	d_nodes.resize(_size * 2);

	d_nodes[0] = nodes[0];
	
	for(int j = 1; j < _size; j++) {
		for(int i = 0; i < _size - j; i++) {
			nodes[i].x = nodes[i].x * (1.0 - _t) + nodes[i+1].x * _t;
			nodes[i].y = nodes[i].y * (1.0 - _t) + nodes[i+1].y * _t;
		}
		d_nodes[j] = nodes[0];
	}

	for(int i = 0; i < _size; i++)
		d_nodes[_size + i] = nodes[i];

	draw();
}

void menu(int value)
{
	switch(value)
	{
		case(MENU_CLEAR):
			nodes.clear();
			if (f_div) { d_nodes.clear(); f_div = false; }
			glClear(GL_COLOR_BUFFER_BIT);
			draw_curve=false;
			break;
		case(MENU_NODE):
			if(!f_div) {
				node nd; nd.x= X; nd.y = Y;
				nodes.push_back(nd);
				glFlush();
			}
			break;
		case(MENU_SHOW_HIDE):
			show_nodes = !show_nodes;
			glFlush();
			break;
		case(MENU_REMOVE): {
			bool f=false; vector<node>::iterator i;
			for(i = nodes.begin(); i!=nodes.end(); i++) {
				if((X-i->x)*(X-i->x)+(Y-i->y)*(Y-i->y)<=20) {
					f=true;
					break;
				}
			}
			if(f) { nodes.erase(i); draw(); } }
			break;
		case(MENU_CURVE):
			draw_curve=true;
			draw();
			break;
		case(MENU_ADD):
			if(!f_div) recomp_base_nodes();
			break;
		case(MENU_DIV):
			if(!f_div && draw_curve) split_curve();
			break;
	}
	glutPostRedisplay();
}

void create_menu()
{
	glutCreateMenu(menu);
	glutAddMenuEntry("Add node", MENU_NODE);
	glutAddMenuEntry("Remove node", MENU_REMOVE);
	glutAddMenuEntry("Show/Hide nodes", MENU_SHOW_HIDE);
	glutAddMenuEntry("Clear", MENU_CLEAR);
	glutAddMenuEntry("Draw curve", MENU_CURVE);
	glutAddMenuEntry("Add base node", MENU_ADD);
	glutAddMenuEntry("Split curve", MENU_DIV);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


void MousePressed(int button, int state, int ax, int ay)
{
	int down = button == GLUT_LEFT_BUTTON && state==GLUT_DOWN;
	if(down)
	{
		cur_node=-1;
		bool f=false;
		if(f_div)
		{
			for(int i=0; i<nodes.size(); i++)
			{
				if((ax-d_nodes[i].x)*(ax-d_nodes[i].x)+(ay-d_nodes[i].y)*(ay-d_nodes[i].y)<=20) {
					d_nodes[i].sel = true;
					f=true;
					cur_node = i;
				}
				else { d_nodes[i].sel = false; }
			}
			for(int i=nodes.size(); i<2*nodes.size(); i++)
			{
				if((ax-d_nodes[i].x)*(ax-d_nodes[i].x)+(ay-d_nodes[i].y)*(ay-d_nodes[i].y)<=20) {
					d_nodes[i].sel = true;
					f=true;
					cur_node = i;
				}
				else { d_nodes[i].sel = false; }
			}
		}
		else
		{
			for(int i=0; i<nodes.size(); i++)
			{
				if((ax-nodes[i].x)*(ax-nodes[i].x)+(ay-nodes[i].y)*(ay-nodes[i].y)<=20) {
					nodes[i].sel = true;
					f=true;
					cur_node = i;
				}
				else { nodes[i].sel = false; }
			}
		}
		if(f) draw();
	}
}

void MouseMoves(int x, int y)
{ X = x; Y = y; }

void MouseDrag(int x, int y)
{
	if(cur_node!=-1) {
		if(f_div) {
			d_nodes[cur_node].x = x; d_nodes[cur_node].y = y;
			if(cur_node==nodes.size()-1) {
				d_nodes[cur_node+1].x = x; d_nodes[cur_node+1].y = y; }
			else if(cur_node==nodes.size()) {
				d_nodes[cur_node-1].x = x; d_nodes[cur_node-1].y = y; }
		}
		else { nodes[cur_node].x = x; nodes[cur_node].y = y; }
		draw();
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(w, h);
	glutInitWindowPosition(100, 200);
	glutCreateWindow("Number of nodes: 0");
	glutDisplayFunc(draw);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-w/2, h/2, w/2, -h/2, -200.0, 200.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glutMouseFunc(MousePressed);
	glutPassiveMotionFunc(MouseMoves);
	glutMotionFunc(MouseDrag);
	create_menu();
	glutMainLoop();
}
