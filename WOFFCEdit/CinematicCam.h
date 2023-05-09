#pragma once
#include "Camera.h"
#include <vector>
class CinematicCam :public Camera {
public:

	CinematicCam(Camera cam);
	//~CinematicCam();
	void getInput(InputCommands input);
	void updatePos();

	void Update(InputCommands input);
	void playBack();

	int totalPos;
	int totalRotation;
	int currentPos;
	int	currentRot;

private:
	std::vector<DirectX::SimpleMath::Vector3> positions;
	std::vector<DirectX::SimpleMath::Vector3> orientations;
	bool tracking;
	bool infocus;

};

