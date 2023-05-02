#pragma once
#include "Camera.h"
#include <vector>
class CinematicCam :public Camera {
public:

	CinematicCam();
	//~CinematicCam();
	void getInput(InputCommands input);
	void updatePos();

	void Update(InputCommands input);


private:
	std::vector<DirectX::SimpleMath::Vector3> positions;
	std::vector<DirectX::SimpleMath::Vector3> orientations;
	bool tracking;
	bool infocus;

};

