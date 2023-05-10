#include "CameraManager.h"

//class for controlling different cameras in the scene 

CameraManager::CameraManager()
{
	mainCamera = new Camera();	//create the default camera 
	currentCam = 0;
	camType = main;
}

void CameraManager::addCinematicCam()	//create a cinematic cam
{
	CinematicCam* cam = (new CinematicCam(*mainCamera));
	CinematicCams.push_back(cam);
	swichcam(StillCams.size()-1);	//view is switched to new camera 
	swichCamType(cine);
}

void CameraManager::addStillCam()	//create static camera 
{
	StillCamera* cam = (new StillCamera(*mainCamera));
	StillCams.push_back(cam);
	swichcam(StillCams.size()-1);	//view siwtched to new camera 
	swichCamType(still);
}

void CameraManager::swichcam()	//function for cycling through cameras
{
	switch (camType)
	{
	case main:
		currentCam = 0;
		break;
	case cine:
		if (currentCam < CinematicCams.size())
			currentCam++;
		if (currentCam >= CinematicCams.size())
			currentCam = 0;
		break;
	case still:
		if (currentCam < StillCams.size())
			currentCam++;
		if (currentCam >= StillCams.size())
			currentCam = 0;
		break;
	default:
		break;
	}
}

void CameraManager::swichcam(int camId)	//switch to desired camera id
{
	currentCam = camId;
}

void CameraManager::swichCamType()	//cycle through cam types -- may be redundant now
{
	switch (camType)
	{
	case main:
		if (CinematicCams.size() == 0) {
			if (StillCams.size() > 0)
				camType = still;
			else
				break;
		}
		currentCam = 0;
		camType = cine;
		break;
	case cine:
		currentCam = 0;
		camType = still;
		break;
	case still:
		currentCam = 0;
		camType = main;
		break;
	default:
		camType = main;
		break;
	}
}

void CameraManager::swichCamType(CamType newType)	//switch to type passed in
{
	if (newType == cine)
		if (CinematicCams.size() == 0) {	//if no cinematic cams exist return
			return;
		}	
	if (newType == still)
		if (StillCams.size() == 0) {	//if no static cams exist return 
			return;
		}

	camType = newType;
}

void CameraManager::Update(InputCommands input)	//function to pass input to the current camera
{
	if (input.createCamDelay > 0.0f)
		input.createCamDelay -= .5f;

	switch (camType)
	{
	case main:
		mainCamera->Update(input);
		if (input.createCineCam)
			if (input.createCamDelay <= 0.f) {	//delay added to camera creation so it doesn't create hundreds in one press
				addCinematicCam();
				input.createCamDelay = 10.0f;
			}
		if(input.createStillCam)
			if (input.createCamDelay <= 0.f) {
				addStillCam();
				input.createCamDelay = 10.0f;
			}
		break;
	case cine:
		CinematicCams.at(currentCam)->Update(input);
		break;
	case still:
		break;	//no input passed to static cam 
	default:
		break;
	}
}

DirectX::SimpleMath::Vector3 CameraManager::getCamPosition()	//returns position of the current camera
{
	switch (camType)
	{
	case main:
		return mainCamera->getCamPosition();
		break;
	case cine:
		return CinematicCams.at(currentCam)->getCamPosition();
		break;
	case still:
		return StillCams.at(currentCam)->getCamPosition();
		break;
	default:
		return { 0,0,0 };
		break;
	}
}

DirectX::SimpleMath::Vector3 CameraManager::getCamLookAt()	//returns the lookat vector of the current camera
{
	switch (camType)
	{
	case main:
		return mainCamera->getCamLookAt();
		break;
	case cine:
		return CinematicCams.at(currentCam)->getCamLookAt();
		break;
	case still:
		return StillCams.at(currentCam)->getCamLookAt();
		break;
	default:
		return { 0,0,0 };
		break;
	}
}