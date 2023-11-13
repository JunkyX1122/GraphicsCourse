#pragma once

#include <string>
#include "Mesh.h"

class HeightMap : public Mesh
{
public:
	HeightMap(const std::string& name, float textureSize, int smoothLevel);
	~HeightMap(void) {};

	Vector3 GetHeightMapSize() const { return heightMapSize; }
	
	float GetHeightAtCoord(int x, int z);

protected:
	Vector3 heightMapSize;
	float textureSize;

	int CalculateOffset(int x, int z, int iWidth);
	int iWidth;
	int iHeight; 
	int iChans;
};