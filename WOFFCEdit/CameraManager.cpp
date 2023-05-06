#include "CameraManager.h"

void CameraManager::addCinematicCam()
{
	CinematicCam cam = *(new CinematicCam());
	CinematicCams.push_back(cam);
}

void CameraManager::swichcam(int camNum)
{
	currentCam = camNum;
}
