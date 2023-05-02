#include "CinematicCam.h"

CinematicCam::CinematicCam()
{
	positions.push_back(getCamPosition()); //first position of camera added on creation 
}

void CinematicCam::getInput(InputCommands input)
{
	if(tracking)
		Camera::getInput(input);


}

void CinematicCam::moving()
{
	Camera::setLookAt();
	for(const DirectX::SimpleMath::Vector3 &X : positions)
		setPos(X);	
	
	for(const DirectX::SimpleMath::Vector3 &Y : orientations)
		setRot(Y);
}

void CinematicCam::Update()
{
	if (tracking) {
		if (*positions.end() != getCamPosition())
			positions.push_back(getCamPosition());

		if (*positions.end() != getCamPosition())
			positions.push_back(getCamPosition());
	}
}

void CinematicCam::setPos(DirectX::SimpleMath::Vector3 newPos)
{
	m_camPosition = newPos;
}

