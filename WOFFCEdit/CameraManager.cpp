#include "CameraManager.h"

CameraManager::CameraManager()
{
	mainCamera = new Camera();
	currentCam = 0;
	camType = main;
}

void CameraManager::addCinematicCam()
{
	CinematicCam* cam = (new CinematicCam(*mainCamera));
	CinematicCams.push_back(cam);
	swichcam(StillCams.size()-1);
	swichCamType(cine);
}

void CameraManager::addStillCam()
{
	StillCamera* cam = (new StillCamera(*mainCamera));
	StillCams.push_back(cam);
	swichcam(StillCams.size()-1);
	swichCamType(still);
}

void CameraManager::swichcam()
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

void CameraManager::swichcam(int camId)
{
	currentCam = camId;
}

void CameraManager::swichCamType()
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

void CameraManager::swichCamType(CamType newType)
{
	if (newType == cine)
		if (CinematicCams.size() == 0) {
			return;
		}	
	if (newType == still)
		if (StillCams.size() == 0) {
			return;
		}

	camType = newType;
}

void CameraManager::Update(InputCommands input)
{
	if (input.createCamDelay > 0.0f)
		input.createCamDelay -= .5f;

	switch (camType)
	{
	case main:
		mainCamera->Update(input);
		if (input.createCineCam)
			if (input.createCamDelay <= 0.f) {
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
		break;
	default:
		break;
	}
}

DirectX::SimpleMath::Vector3 CameraManager::getCamPosition()
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

DirectX::SimpleMath::Vector3 CameraManager::getCamLookAt()
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