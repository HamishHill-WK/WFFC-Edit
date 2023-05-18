#include "CinematicCam.h"
//this class is for cameras which move alone a pre recorded path 

CinematicCam::CinematicCam(Camera cam)
{
	infocus = false;
	tracking = true;
	totalPos = 0;
	totalRotation = 0;
	currentPos = 0;
	currentRot = 0;
	setPos(cam.getCamPosition());
	setRot(cam.getCamOrientaion());
	setLookAt();
	positions.push_back(cam.getCamPosition()); //first position of camera added on creation 
	orientations.push_back(cam.getCamOrientaion()); //first position of camera added on creation 
}

void CinematicCam::getInput(InputCommands input)
{
	if(tracking)	//if positions are being recorded then inputs are handled the same as in parent camera class.
		Camera::getInput(input);	//stops handling inputs when playback has been begun

	if (input.completeCineCam) {
		tracking = false;
		playBack();
	}
}

void CinematicCam::updatePos()	//this function is for looping through the saved positions and orientations 
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

	if (tracking){// while recording, positions and orientation and added to corresponding vectors 
		DirectX::SimpleMath::Vector3 newPos = getCamPosition();
		DirectX::SimpleMath::Vector3 newRot = getCamOrientaion();

		positions.push_back(newPos);	//add pos to vector of positions 

		orientations.push_back(newRot); //add rot to vector of orientations 
	}
	
	if(!tracking)	//if the camera is no longer being controlled and playback has begun
	{
		updatePos();	//update position
	}
}

void CinematicCam::playBack()	//Camera returns to the first position and orienation saved 
{
	totalPos = positions.size();
	totalRotation = orientations.size();
	currentPos = 0;
	currentRot = 0;
}