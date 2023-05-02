#include "Camera.h"
#include "pch.h"

using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

Camera::Camera() {
	m_movespeed = 0.30;
	m_camRotRate = 3.0;

	//camera
	m_camPosition.x = 0.0f;
	m_camPosition.y = 3.7f;
	m_camPosition.z = -3.5f;

	m_camOrientation.x = 0;
	m_camOrientation.y = 0;
	m_camOrientation.z = 0;

	m_camLookAt.x = 0.0f;
	m_camLookAt.y = 0.0f;
	m_camLookAt.z = 0.0f;

	m_camLookDirection.x = 0.0f;
	m_camLookDirection.y = 0.0f;
	m_camLookDirection.z = 0.0f;

	m_camRight.x = 0.0f;
	m_camRight.y = 0.0f;
	m_camRight.z = 0.0f;

	m_camOrientation.x = 0.0f;
	m_camOrientation.y = 0.0f;
	m_camOrientation.z = 0.0f;
	
	m_camTargetPos.x = 0.0f;
	m_camTargetPos.y = 0.0f;
	m_camTargetPos.z = 0.0f;

	m_moving = false;
	m_movingTo = false;
	setLookAt();
}

void Camera::Update(InputCommands input){
	getInput(input);

	if (m_movingTo) {
		float  distance = DirectX::SimpleMath::Vector3::Distance(m_camPosition, m_camTargetPos);

		if (distance >= 5.0f) {
			m_moving = true;
			moveForward();
		}
		else {
			m_moving = false;
			m_movingTo = false;

		}
	}
}

Vector3 Camera::getCamPosition(){return m_camPosition;}

Vector3 Camera::getCamLookAt(){return m_camLookAt;}

DirectX::SimpleMath::Vector3 Camera::getCamOrientaion()
{
	return m_camOrientation;
}

//function for changing look direction to poisition of an object
void Camera::setCamTarget(DirectX::SimpleMath::Vector3 newTarget, bool moveTo) {
	m_camLookDirection.x = (newTarget.x - m_camPosition.x);
	m_camLookDirection.y = (newTarget.y - m_camPosition.y);
	m_camLookDirection.z = (newTarget.z - m_camPosition.z);
	m_camLookDirection.Normalize();
	m_camLookAt = m_camPosition + m_camLookDirection;

	//calculate new orientation from forward vector. This means camera doesn't suddenly jump back to previous rotation from before focusing on an object
	m_camOrientation.x = atan2(m_camLookDirection.y, m_camLookDirection.x) * 180/ 3.1415 ;
	m_camOrientation.y = (asin(m_camLookDirection.z) * 180 / 3.1415);
		
	if (m_camOrientation.x > 90)
		m_camOrientation.x = 90;	
	
	if (m_camOrientation.x < -90)
		m_camOrientation.x = -90;

	if (moveTo) {
		m_camTargetPos = newTarget;
		m_movingTo = true;
	}
}

void Camera::moveForward()
{
	m_camPosition += m_camLookDirection * m_movespeed * 2;
	setLookAt();
}

void Camera::setLookAt(){

	if (m_camOrientation.x > 90)
		m_camOrientation.x = 90;

	if (m_camOrientation.x < -90)
		m_camOrientation.x = -90;

	m_camLookDirection.x = cos((m_camOrientation.y) * 3.1415 / 180) * cos(m_camOrientation.x * 3.1415 / 180);
	m_camLookDirection.y = sin((m_camOrientation.x) * 3.1415 / 180);
	m_camLookDirection.z = sin((m_camOrientation.y) * 3.1415 / 180) * cos(m_camOrientation.x * 3.1415 / 180);
	m_camLookDirection.Normalize();
	m_camLookAt = m_camPosition + m_camLookDirection;

}

void Camera::getInput(InputCommands input){
	if (m_moving)
		return;

	m_moving = true;
	if (input.rotRight)
	{
		m_camOrientation.y += m_camRotRate;
		setLookAt();
	}
	if (input.rotLeft)
	{
		m_camOrientation.y -= m_camRotRate;
		setLookAt();
	}

	if (input.rotDown)
	{
		if(m_camOrientation.x > -90)
			m_camOrientation.x -= m_camRotRate;

		setLookAt();
	}

	if (input.rotUp)
	{
		if (m_camOrientation.x < 90)
		m_camOrientation.x += m_camRotRate;
		setLookAt();
	}

	m_camLookDirection.Cross(Vector3::UnitY, m_camRight);

	if (input.forward)
	{
		m_camPosition += m_camLookDirection * m_movespeed;
		setLookAt();
	}
	if (input.back)
	{
		m_camPosition -= m_camLookDirection * m_movespeed;
		setLookAt();
	}
	if (input.right)
	{
		m_camPosition += m_camRight * m_movespeed;
		setLookAt();
	}
	if (input.left)
	{
		m_camPosition -= m_camRight * m_movespeed;
		setLookAt();
	}	

	if (input.descend)
	{
		m_camPosition.y -= 1 * m_movespeed;
		setLookAt();
	}	
	
	if (input.descend)
	{
		m_camPosition.y -= 1 * m_movespeed;
		setLookAt();
	}	
	
	if (input.ascend)
	{
		m_camPosition.y += 1 * m_movespeed;
		setLookAt();
	}

	//else
		m_moving = false;
}

void Camera::setPos(DirectX::SimpleMath::Vector3 newPos)
{
	m_camPosition = newPos;
}

void Camera::setRot(DirectX::SimpleMath::Vector3 newRot)
{
	m_camOrientation = newRot;
}

