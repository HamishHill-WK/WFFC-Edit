#pragma once
#include "Camera.h"
#include "CinematicCam.h"
#include "StillCamera.h"
#include <vector>
#include <iostream>
#include <fstream>

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
	void addStillCam();
	void swichcam();
	void swichcam(int camId);
	void swichCamType();
	void swichCamType(CamType newType);

	void Update(InputCommands input);

	void loadBinary();	//load cinematic and still cam vectors from binary files

	void saveToBinary(); //function to save cinematic and still cam vectors to binary file

	void emptyBinaryFile();	//function to clear binary files

	DirectX::SimpleMath::Vector3 getCamPosition();
	DirectX::SimpleMath::Vector3 getCamLookAt();

	std::vector<CinematicCam*> CinematicCams;
	std::vector<StillCamera*> StillCams;
	Camera* mainCamera;
	
	int currentCam;
	CamType camType; 

};

