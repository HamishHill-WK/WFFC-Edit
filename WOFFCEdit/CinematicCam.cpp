#include "CinematicCam.h"

CinematicCam::CinematicCam()
{
	positions.push_back(getCamPosition()); //first position of camera added on creation 
	infocus = false;
	tracking = false;
}

void CinematicCam::getInput(InputCommands input)
{
	if(tracking)
		Camera::getInput(input);
}

void CinematicCam::updatePos()
{
	for(const DirectX::SimpleMath::Vector3 &X : positions)
		setPos(X);	
	
	for(const DirectX::SimpleMath::Vector3 &Y : orientations)
		setRot(Y);

	Camera::setLookAt();
}

void CinematicCam::Update(InputCommands input)
{
	getInput(input);

	if (tracking && m_moving) { //while tracking cam position and rotation updates and while the camera is moving 
		DirectX::SimpleMath::Vector3 newPos = getCamPosition();
		DirectX::SimpleMath::Vector3 newRot = getCamOrientaion();
		if (*positions.end() != newPos)	//check last position is different to current pos 
			positions.push_back(newPos);	//add pos to vector of positions 

		if (*orientations.end() != newRot)
			positions.push_back(newRot);
	}
}