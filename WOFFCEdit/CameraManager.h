#pragma once
#include "Camera.h"
#include "CinematicCam.h"
#include <vector>

enum CamType {
	main,
	cine,
	stat
};


class CameraManager
{
public:
	CameraManager();
	~CameraManager();

	void addCinematicCam();
	void swichcam(int camNum);
	void swichCamTyoe(int camType);

	std::vector<CinematicCam> CinematicCams;
	Camera mainCamera;
	
	int currentCam;
	CamType camType; 
};

