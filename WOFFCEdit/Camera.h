#pragma once

#include "InputCommands.h"
#include "Camera.h"
#include "pch.h"
#include <vector>


class Camera 
{
public:
	Camera();
	//~Camera();

	void getInput(InputCommands input);
	void setLookAt();
	void Update(InputCommands input);

	DirectX::SimpleMath::Vector3 getCamPosition();
	DirectX::SimpleMath::Vector3 getCamLookAt();
	DirectX::SimpleMath::Vector3 getCamOrientaion();
	
	void setCamTarget(DirectX::SimpleMath::Vector3 newTarget, bool moveTo);
private:

	DirectX::SimpleMath::Vector3		m_camLookAt;
	DirectX::SimpleMath::Vector3		m_camLookDirection;
	DirectX::SimpleMath::Vector3		m_camRight;
	float								m_camRotRate;
	float								m_movespeed;
	void moveForward();

protected:
	DirectX::SimpleMath::Vector3		m_camPosition;
	DirectX::SimpleMath::Vector3		m_camOrientation;
	DirectX::SimpleMath::Vector3		m_camTargetPos;
	bool								m_moving; 
	bool								m_movingTo; 
	
	
	void setPos(DirectX::SimpleMath::Vector3);
	void setRot(DirectX::SimpleMath::Vector3);
};

