#pragma once
#include "Camera.h"

class StillCamera :	public Camera{
public:
	StillCamera(Camera cam);

    //void serialize(std::ostream& stream) const
    //{
    //    // Serialize member variables individually
    //    stream.write(reinterpret_cast<const char*>(&m_camPosition), sizeof(m_camPosition));
    //    stream.write(reinterpret_cast<const char*>(&m_camOrientation), sizeof(m_camOrientation));
    //    // ... serialize other member variables
    //}
};

