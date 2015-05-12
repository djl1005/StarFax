#pragma once

#include <DirectXMath.h>
#include <Windows.h>

using namespace DirectX;


class Camera
{
public:
	Camera(XMFLOAT3, XMFLOAT3, float, float);
	~Camera();

	void mouse(float, float);

#pragma region gets / sets


	void updateProjection(float);
	void setPostion(XMFLOAT3);
	void offsetPosition(XMFLOAT3);
	
	void setDirection(XMFLOAT3);
	void offsetDirection(XMFLOAT3);

	void setProjection(XMFLOAT4X4);
	
	void setRotX(float);
	void offsetRotX(float);
	
	void setRotY(float);
	void offsetRotY(float);

	XMFLOAT3 getPosition();
	XMFLOAT3 getdirection();
	float getRotX();
	float getRotY();
	XMFLOAT4X4 getViewMat();
	XMFLOAT4X4 getProjection();

#pragma endregion

	void update(float);

private:
	
	XMFLOAT3 position;
	XMFLOAT3 direction;
	float rotX;
	float rotY;
	XMFLOAT4X4 viewMat;
	XMFLOAT4X4 projection;

	float speed = 1;
	float rotScale = .001f;

};

