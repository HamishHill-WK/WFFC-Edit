#pragma once

#include "InputCommands.h"
#include "Camera.h"
#include "pch.h"
#include <vector>
#include <fstream>


class Camera 
{
public:
	Camera();
	//~Camera();

	void getInput(InputCommands input);	//handles input
	void setLookAt();					//calculates forward vector from position and orientation
	void Update(InputCommands input);

	DirectX::SimpleMath::Vector3 getCamPosition();
	DirectX::SimpleMath::Vector3 getCamLookAt();
	DirectX::SimpleMath::Vector3 getCamOrientaion();
	
	void serialize(std::ostream& stream) const	//lambda for serializing data to binary file
	{
		// Serialize the pos and rot variables
		stream.write(reinterpret_cast<const char*>(&m_camPosition.x), sizeof(m_camPosition.x));
		stream.write(reinterpret_cast<const char*>(&m_camPosition.y), sizeof(m_camPosition.y));
		stream.write(reinterpret_cast<const char*>(&m_camPosition.z), sizeof(m_camPosition.z));

		stream.write(reinterpret_cast<const char*>(&m_camOrientation.x), sizeof(m_camOrientation.x));
		stream.write(reinterpret_cast<const char*>(&m_camOrientation.y), sizeof(m_camOrientation.y));
		stream.write(reinterpret_cast<const char*>(&m_camOrientation.z), sizeof(m_camOrientation.z));
	}

	void deserialize(std::istream& stream) //lambda for deserializing data from binary file
	{
		// Deserialize the pos and rot variables
		stream.read(reinterpret_cast<char*>(&m_camPosition.x), sizeof(m_camPosition.x));
		stream.read(reinterpret_cast<char*>(&m_camPosition.y), sizeof(m_camPosition.y));
		stream.read(reinterpret_cast<char*>(&m_camPosition.z), sizeof(m_camPosition.z));

		stream.read(reinterpret_cast<char*>(&m_camOrientation.x), sizeof(m_camOrientation.x));
		stream.read(reinterpret_cast<char*>(&m_camOrientation.y), sizeof(m_camOrientation.y));
		stream.read(reinterpret_cast<char*>(&m_camOrientation.z), sizeof(m_camOrientation.z));
	}

	void setCamTarget(DirectX::SimpleMath::Vector3 newTarget, bool moveTo); //set camera look direction towards object passed in 

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