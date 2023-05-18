#include "CameraManager.h"
//class for controlling different cameras in the scene 

CameraManager::CameraManager()
{
	//uncomment to clear binary file of existing still cams and cinematic cams 
	//emptyBinaryFile(); 

	mainCamera = new Camera();	//create the default camera 
	currentCam = 0;
	camType = main;
	loadBinary();	//still cams and cinematic cams are loaded in from binary files 
}

CameraManager::~CameraManager()
{
	saveToBinary();	// cinematic and still cams vectors saved to binary file on deconstruction
}

void CameraManager::addCinematicCam()	//create a cinematic cam
{
	CinematicCam* cam = (new CinematicCam(*mainCamera));
	CinematicCams.push_back(cam);
	swichcam(CinematicCams.size()-1);	//view is switched to new camera 
	swichCamType(cine);
}

void CameraManager::addStillCam()	//create static camera 
{
	StillCamera* cam = (new StillCamera(*mainCamera));
	StillCams.push_back(cam);
	swichcam(StillCams.size()-1);	//view siwtched to new camera 
	swichCamType(still);
}

void CameraManager::swichcam()	//function for cycling through cameras
{
	switch (camType)
	{
	case main:
		currentCam = 0;
		break;
	case cine:
		if (currentCam < CinematicCams.size())
			currentCam++;
		if (currentCam >= CinematicCams.size())
			currentCam = 0;
		break;
	case still:
		if (currentCam < StillCams.size())
			currentCam++;
		if (currentCam >= StillCams.size())
			currentCam = 0;
		break;
	default:
		break;
	}
}

void CameraManager::swichcam(int camId)	//switch to desired camera id
{
	currentCam = camId;
}

void CameraManager::swichCamType()	//cycle through cam types -- may be redundant now
{
	switch (camType)
	{
	case main:
		if (CinematicCams.size() == 0) {
			if (StillCams.size() > 0)
				camType = still;
			else
				break;
		}
		currentCam = 0;
		camType = cine;
		break;
	case cine:
		currentCam = 0;
		camType = still;
		break;
	case still:
		currentCam = 0;
		camType = main;
		break;
	default:
		camType = main;
		break;
	}
}

void CameraManager::swichCamType(CamType newType)	//switch to type passed in
{
	if (newType == cine)
		if (CinematicCams.size() == 0) {	//if no cinematic cams exist return
			return;
		}	
	if (newType == still)
		if (StillCams.size() == 0) {	//if no static cams exist return 
			return;
		}

	camType = newType;
}

void CameraManager::Update(InputCommands input)	//function to pass input to the current camera
{
	if (input.createCamDelay > 0.0f)
		input.createCamDelay -= .5f;

	switch (camType)
	{
	case main:
		mainCamera->Update(input);
		if (input.createCineCam)
			if (input.createCamDelay <= 0.f) {	//delay added to camera creation so it doesn't create hundreds in one press
				addCinematicCam();
				input.createCamDelay = 10.0f;
			}
		if(input.createStillCam)
			if (input.createCamDelay <= 0.f) {
				addStillCam();
				input.createCamDelay = 10.0f;
			}
		break;
	case cine:
		CinematicCams.at(currentCam)->Update(input);
		break;
	case still:
		break;	//no input passed to static cam 
	default:
		break;
	}
}

DirectX::SimpleMath::Vector3 CameraManager::getCamPosition()	//returns position of the current camera
{
	switch (camType)
	{
	case main:
		return mainCamera->getCamPosition();
		break;
	case cine:
		return CinematicCams.at(currentCam)->getCamPosition();
		break;
	case still:
		return StillCams.at(currentCam)->getCamPosition();
		break;
	default:
		return { 0,0,0 };
		break;
	}
}

DirectX::SimpleMath::Vector3 CameraManager::getCamLookAt()	//returns the lookat vector of the current camera
{
	switch (camType)
	{
	case main:
		return mainCamera->getCamLookAt();
		break;
	case cine:
		return CinematicCams.at(currentCam)->getCamLookAt();
		break;
	case still:
		return StillCams.at(currentCam)->getCamLookAt();
		break;
	default:
		return { 0,0,0 };
		break;
	}
}

void CameraManager::loadBinary()
{
	std::ifstream file("CineCams.bin", std::ios::binary);
	if (!file) return;

	std::ifstream file1("StillCams.bin", std::ios::binary);
	if (!file1) return;

	// Read the size of the vector
	size_t size;
	file.read(reinterpret_cast<char*>(&size), sizeof(size));

	if (size < MAXSSIZE_T)	//if the file is empty is will return maxsize_t for its size - should have a better check for this 
		// Read each 3D vector
		for (size_t j = 0; j < size; ++j) {
			CinematicCam* element = new CinematicCam(Camera());
			element->deserialize1(file);
			element->tracking = false;	
			element->playBack();
			CinematicCams.push_back(element);
		}

	file.close();

	// Read the size of the vector
	size_t size1;
	file1.read(reinterpret_cast<char*>(&size1), sizeof(size1));

	if (size1 < MAXSSIZE_T)
		// Read each 3D vector
		for (size_t j = 0; j < size1; ++j) {
			StillCamera* element = new StillCamera(Camera());
			element->deserialize(file1);
			StillCams.push_back(element);
		}

	file1.close();
}

void CameraManager::saveToBinary()
{	//vectors are saved in seperate files to allow for modularity and avoid the data getting messy 
	std::ofstream file("CineCams.bin", std::ios::binary);
	if (!file) return; // if binary file could not be accessed return

	size_t vectorCount = CinematicCams.size();
	if (vectorCount < MAXSSIZE_T) {
		file.write(reinterpret_cast<const char*>(&vectorCount), sizeof(vectorCount));

		// Write each 3D vector to the file
		for (const auto& vector : CinematicCams) {
			vector->serialize1(file);
		}
	}
	file.close();

	std::ofstream file1("StillCams.bin", std::ios::binary);
	if (!file1) return;  // if binary file could not be accessed return

	if (vectorCount < MAXSSIZE_T) {
		size_t vectorCount1 = StillCams.size();
		file1.write(reinterpret_cast<const char*>(&vectorCount1), sizeof(vectorCount1));

		for (const auto& vector1 : StillCams) {
			vector1->serialize(file1);
		}
	}

	file1.close();
}

void CameraManager::emptyBinaryFile()	//debug function for clearing binary file 
{
	std::ofstream file("StillCams.bin", std::ios::binary | std::ios::trunc);
	if (!file) return;

	file.close();

	std::ofstream file1("CineCams.bin", std::ios::binary | std::ios::trunc);
	if (!file1) return;
	file1.close();
}