#pragma once

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;
	bool rotRight;
	bool rotLeft;
	bool rotUp;
	bool rotDown;
	int mouse_X;
	int mouse_Y;
	bool mouse_LB_Down;bool mouse_LB_Up;
	float mouse_LB_DoubleClickTime;
	bool mode_terrainEditor;
};
