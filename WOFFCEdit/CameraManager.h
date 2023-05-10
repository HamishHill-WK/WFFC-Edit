#pragma once
#include "Camera.h"
#include "CinematicCam.h"
#include <vector>

enum CamType {
	main,
	cine,
	still
};


class CameraManager
{
public:
	CameraManager();
	~CameraManager();

	void addCinematicCam();
	void swichcam();
	void swichCamType();

	void Update(InputCommands input);

	DirectX::SimpleMath::Vector3 getCamPosition();
	DirectX::SimpleMath::Vector3 getCamLookAt();

	std::vector<CinematicCam*> CinematicCams;
	Camera* mainCamera;
	
	int currentCam;
	CamType camType; 

};

