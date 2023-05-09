#include "CinematicCam.h"

CinematicCam::CinematicCam(Camera cam)
{
	positions.push_back(cam.getCamPosition()); //first position of camera added on creation 
	orientations.push_back(cam.getCamOrientaion()); //first position of camera added on creation 
	infocus = false;
	tracking = true;
}

void CinematicCam::getInput(InputCommands input)
{
	if(tracking)
		Camera::getInput(input);

	if (input.completeCineCam) {
		tracking = false;
		playBack();
	}
}

void CinematicCam::updatePos()
{
	if (currentPos == totalPos && currentRot == totalRotation)
		playBack();

	setPos(positions.at(currentPos));	
	
	setRot(orientations.at(currentRot));

	if (currentPos < totalPos )
		currentPos++;

	if (currentRot < totalRotation)
		currentRot++;

	Camera::setLookAt();
}

void CinematicCam::Update(InputCommands input)
{
	getInput(input);

	if (tracking){// && m_moving) { //while tracking cam position and rotation updates and while the camera is moving 
		DirectX::SimpleMath::Vector3 newPos = getCamPosition();
		DirectX::SimpleMath::Vector3 newRot = getCamOrientaion();
		if (!positions.empty())
			if(positions.at(positions.size()-1) != newPos)	//check last position is different to current pos 
				positions.push_back(newPos);	//add pos to vector of positions 

		if (!orientations.empty())
			if (orientations.at(orientations.size()-1) != newRot)
				orientations.push_back(newRot);
	}
	
	if(!tracking)
	{
		updatePos();
	}
}

void CinematicCam::playBack()
{
	totalPos = positions.size();
	totalRotation = orientations.size();
	currentPos = 0;
	currentRot = 0;
}
