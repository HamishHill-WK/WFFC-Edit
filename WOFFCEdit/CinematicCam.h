#pragma once
#include "Camera.h"
#include <vector>
class CinematicCam :public Camera {
public:

	CinematicCam();
	~CinematicCam();
	void getInput(InputCommands input);
	void moving();

	void Update();
	void setPos(DirectX::SimpleMath::Vector3);
	void setRot(DirectX::SimpleMath::Vector3);

private:
	std::vector<DirectX::SimpleMath::Vector3> positions;
	std::vector<DirectX::SimpleMath::Vector3> orientations;
	bool tracking;
	bool infocus;

};

