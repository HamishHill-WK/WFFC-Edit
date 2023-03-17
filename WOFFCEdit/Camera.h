#pragma once

#include "InputCommands.h"
#include "Camera.h"
#include "pch.h"
#include <vector>


class Camera 
{
public:
	Camera();
	~Camera();

	void moveForward();
	void moveBack();
	void moveUp();
	void moveDown();

	void getInput(InputCommands input);
	void setLookAt();
	void Update(InputCommands input);

	DirectX::SimpleMath::Vector3 getCamPosition();
	DirectX::SimpleMath::Vector3 getCamLookAt();

private:
	DirectX::SimpleMath::Vector3		m_camPosition;
	DirectX::SimpleMath::Vector3		m_camOrientation;
	DirectX::SimpleMath::Vector3		m_camLookAt;
	DirectX::SimpleMath::Vector3		m_camLookDirection;
	DirectX::SimpleMath::Vector3		m_camRight;
	float								m_camRotRate;
	float								m_movespeed;

	
};

