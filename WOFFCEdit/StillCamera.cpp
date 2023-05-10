#include "StillCamera.h"

StillCamera::StillCamera(Camera cam)
{
	m_camPosition = cam.getCamPosition();
	m_camOrientation = cam.getCamOrientaion();
	setLookAt();
}
