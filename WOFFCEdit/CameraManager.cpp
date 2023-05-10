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
	swichcam();
	swichCamType();
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
		else if (currentCam >= CinematicCams.size())
			currentCam = 0;
		break;
	case still:
		break;
	default:
		break;
	}
}

void CameraManager::swichCamType()
{
	switch (camType)
	{
	case main:
		if (CinematicCams.size() == 0)
			addCinematicCam();

		camType = cine;
		break;
	case cine:
		camType = main;
		break;
	case still:
		camType = main;
		break;
	default:
		camType = main;
		break;
	}
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

