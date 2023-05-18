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

	DirectX::SimpleMath::Vector3 getCamPosition();	//get position of current cam
	DirectX::SimpleMath::Vector3 getCamLookAt();	//get look direction of current cam

	std::vector<CinematicCam*> CinematicCams;	//Container for cinematic cams 
	std::vector<StillCamera*> StillCams;		//container for stillcams
	Camera* mainCamera;
	
	int currentCam;	//current cam id
	CamType camType; //current type of cam
};