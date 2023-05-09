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
	swichcam(currentCam);
	swichCamType(cine);
}

void CameraManager::swichcam(int camNum)
{
	currentCam = camNum;
}

void CameraManager::swichCamType(CamType newType)
{
	camType = newType;
	/*switch (camType)
	{
	case main:
		camType = cine;
		break;
	case cine:
		camType = still;
		break;
	case still:
		camType = main;
		break;
	default:
		camType = main;
		break;
	}*/
}

void CameraManager::Update(InputCommands input)
{
	if (input.createCineCam)
		addCinematicCam();

	switch (camType)
	{
	case main:
		mainCamera->Update(input);
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
		return { 0,0,0 };
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
		return { 0,0,0 };
		break;
	default:
		return { 0,0,0 };
		break;
	}
}

