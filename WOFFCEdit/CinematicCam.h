#pragma once
#include "Camera.h"
#include <vector>
class CinematicCam :public Camera {
public:
	CinematicCam(Camera cam);
	//~CinematicCam();
	void getInput(InputCommands input);
	void updatePos();

	void Update(InputCommands input);
	void playBack();

	int totalPos;
	int totalRotation;
	int currentPos;
	int	currentRot;

	void serialize1(std::ostream& stream) const
	{
		// Serialize the vector size
		size_t vectorCount = positions.size();
		stream.write(reinterpret_cast<const char*>(&vectorCount), sizeof(vectorCount));		
		


		// Serialize each Vector3 object in the vector
		for (const auto& vector : positions)
		{
			stream.write(reinterpret_cast<const char*>(&vector.x), sizeof(vector.x));
			stream.write(reinterpret_cast<const char*>(&vector.y), sizeof(vector.y));
			stream.write(reinterpret_cast<const char*>(&vector.z), sizeof(vector.z));
		}		

		vectorCount = orientations.size();
		stream.write(reinterpret_cast<const char*>(&vectorCount), sizeof(vectorCount));
		
		for (const auto& vector : orientations)
		{
			stream.write(reinterpret_cast<const char*>(&vector.x), sizeof(vector.x));
			stream.write(reinterpret_cast<const char*>(&vector.y), sizeof(vector.y));
			stream.write(reinterpret_cast<const char*>(&vector.z), sizeof(vector.z));
		}
	}

	//void deserialize(std::istream& stream)
	//{
	//	// Deserialize the vector size
	//	size_t vectorCount;
	//	stream.read(reinterpret_cast<char*>(&vectorCount), sizeof(vectorCount));

	//	// Deserialize each Vector3 object in the vector
	//	positions.resize(vectorCount);
	//	for (auto& vector : positions)
	//	{
	//		stream.read(reinterpret_cast<char*>(&vector.x), sizeof(vector.x));
	//		stream.read(reinterpret_cast<char*>(&vector.y), sizeof(vector.y));
	//		stream.read(reinterpret_cast<char*>(&vector.z), sizeof(vector.z));
	//	}		
	//	
	//	stream.read(reinterpret_cast<char*>(&vectorCount), sizeof(vectorCount));

	//	// Deserialize each Vector3 object in the vector
	//	orientations.resize(vectorCount);
	//	for (auto& vector : orientations)
	//	{
	//		stream.read(reinterpret_cast<char*>(&vector.x), sizeof(vector.x));
	//		stream.read(reinterpret_cast<char*>(&vector.y), sizeof(vector.y));
	//		stream.read(reinterpret_cast<char*>(&vector.z), sizeof(vector.z));
	//	}
	//}

	void deserialize1(std::istream& stream)
	{
		// Deserialize the positions vector
		size_t positionsCount;
		stream.read(reinterpret_cast<char*>(&positionsCount), sizeof(positionsCount));
		positions.resize(positionsCount);
		for (auto& vector : positions)
		{
			stream.read(reinterpret_cast<char*>(&vector.x), sizeof(vector.x));
			stream.read(reinterpret_cast<char*>(&vector.y), sizeof(vector.y));
			stream.read(reinterpret_cast<char*>(&vector.z), sizeof(vector.z));
		}

		// Deserialize the orientations vector
		size_t orientationsCount;
		stream.read(reinterpret_cast<char*>(&orientationsCount), sizeof(orientationsCount));
		orientations.resize(orientationsCount);
		for (auto& vector : orientations)
		{
			stream.read(reinterpret_cast<char*>(&vector.x), sizeof(vector.x));
			stream.read(reinterpret_cast<char*>(&vector.y), sizeof(vector.y));
			stream.read(reinterpret_cast<char*>(&vector.z), sizeof(vector.z));
		}
	}

private:
	std::vector<DirectX::SimpleMath::Vector3> positions;
	std::vector<DirectX::SimpleMath::Vector3> orientations;
	bool tracking;
	bool infocus;
};