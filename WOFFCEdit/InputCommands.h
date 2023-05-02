#pragma once

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;	
	bool ascend;
	bool descend;
	bool rotRight;
	bool rotLeft;
	bool rotUp;
	bool rotDown;
	int mouse_X;
	int mouse_Y;
	bool mouse_LB_Down;bool mouse_LB_Up;
	float mouse_LB_DoubleClickTime;
	float wireFrameDelay;
	bool mode_terrainEditor;
	bool mode_wireFrame;
};
