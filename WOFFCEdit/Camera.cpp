#include "Camera.h"
#include "pch.h"

using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;


Camera::Camera()
{
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
}

void Camera::moveForward()
{
}

void Camera::moveBack()
{
}

void Camera::moveUp()
{
}

void Camera::moveDown()
{
}

void Camera::Update(InputCommands input)
{
	getInput(input);
	setLookAt();

}

Vector3 Camera::getCamPosition()
{
	return m_camPosition;
}

Vector3 Camera::getCamLookAt()
{
	return m_camLookAt;
}

void Camera::setLookAt()
{
	m_camLookDirection.x = cos((m_camOrientation.y) * 3.1415 / 180) * cos(m_camOrientation.x * 3.1415 / 180);
	m_camLookDirection.y = sin((m_camOrientation.x) * 3.1415 / 180);
	m_camLookDirection.z = sin((m_camOrientation.y) * 3.1415 / 180) * cos(m_camOrientation.x * 3.1415 / 180);
	m_camLookDirection.Normalize();
	m_camLookAt = m_camPosition + m_camLookDirection;

}

void Camera::getInput(InputCommands input)
{

	if (input.rotRight)
	{
		m_camOrientation.y -= m_camRotRate;
	}
	if (input.rotLeft)
	{
		m_camOrientation.y += m_camRotRate;
	}

	if (input.rotDown)
	{
		m_camOrientation.x -= m_camRotRate;
	}

	if (input.rotUp)
	{
		m_camOrientation.x += m_camRotRate;
	}

	m_camLookDirection.Cross(Vector3::UnitY, m_camRight);

	if (input.forward)
	{
		m_camPosition += m_camLookDirection * m_movespeed;
	}
	if (input.back)
	{
		m_camPosition -= m_camLookDirection * m_movespeed;
	}
	if (input.right)
	{
		m_camPosition += m_camRight * m_movespeed;
	}
	if (input.left)
	{
		m_camPosition -= m_camRight * m_movespeed;
	}

	
}
