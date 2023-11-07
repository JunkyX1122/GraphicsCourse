#include "HeightMap.h"
#include <iostream>

const float HEIGHTMAP_SCALE = 14.0f;
HeightMap::HeightMap(const std::string& name, float textureSize = 16.0f)
{
	int iWidth, iHeight, iChans;

	unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);

	if (!data)
	{
		std::cout << "WHAT HEIGHTMAP?????\n";
		return;
	}
	textureSize = textureSize;
	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	Vector3 vertexScale = Vector3(16.0f, HEIGHTMAP_SCALE, 16.0f);
	Vector2 textureScale = Vector2(1.0f / (textureSize), 1.0f / (textureSize));

	for (int z = 0; z < iHeight; ++z)
	{
		for (int x = 0; x < iWidth; ++x)
		{
			int offset = CalculateOffset(x, z, iWidth);
			float yHeight = data[offset];

			//yHeight /= 5.0f;
			vertices[offset] = Vector3(x, yHeight, z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}
	}
	float k = 0.5f;
	for (int counter = 0; counter < 16; counter++)
	{
		for (int x = 1; x < iWidth; x++)
		{
			for (int z = 0; z < iHeight; z++)
			{
				int offsetCurrent = CalculateOffset(x, z, iWidth);
				int offsetTarget = CalculateOffset(x - 1, z, iWidth);
				vertices[offsetCurrent].y = vertices[offsetTarget].y * (1 - k) + vertices[offsetCurrent].y * k;
			}
		}

		for (int x = iWidth - 2; x < -1; x--)
		{
			for (int z = 0; z < iHeight; z++)
			{
				int offsetCurrent = CalculateOffset(x, z, iWidth);
				int offsetTarget = CalculateOffset(x + 1, z, iWidth);
				vertices[offsetCurrent].y = vertices[offsetTarget].y * (1 - k) + vertices[offsetCurrent].y * k;
			}
		}

		for (int x = 0; x < iWidth; x++)
		{
			for (int z = 1; z < iHeight; z++)
			{
				int offsetCurrent = CalculateOffset(x, z, iWidth);
				int offsetTarget = CalculateOffset(x, z - 1, iWidth);
				vertices[offsetCurrent].y = vertices[offsetTarget].y * (1 - k) + vertices[offsetCurrent].y * k;
			}
		}

		for (int x = 0; x < iWidth; x++)
		{
			for (int z = iHeight; z < -1; z--)
			{
				int offsetCurrent = CalculateOffset(x, z, iWidth);
				int offsetTarget = CalculateOffset(x, z + 1, iWidth);
				vertices[offsetCurrent].y = vertices[offsetTarget].y * (1 - k) + vertices[offsetCurrent].y * k;
			}
		}
	}
	SOIL_free_image_data(data);

	int i = 0;

	for (int z = 0; z < iHeight -1; z++)
	{
		for (int x = 0; x < iWidth - 1; x++)
		{
			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;
			
			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}
	std::cout << "HeightMap Vertex Count: " << i << std::endl;
	GenerateNormals();
	
	GenerateTangents();
	BufferData();

	heightMapSize.x = vertexScale.x * (iWidth - 1);
	heightMapSize.y = vertexScale.y * 255.0f;
	heightMapSize.z = vertexScale.z * (iHeight - 1);
}

int HeightMap::CalculateOffset(int x, int z, int iWidth)
{
	return (z * iWidth) + x;
}