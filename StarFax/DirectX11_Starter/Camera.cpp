#include "Camera.h"


Camera::Camera(XMFLOAT3 pos, XMFLOAT3 dir, float rx, float ry)
{
	position = pos;
	direction = dir;
	rotX = rx;
	rotY = ry;
}


Camera::~Camera()
{
}

#pragma region gets / sets

void Camera::setPostion(XMFLOAT3 pos){ position = pos; }
void Camera::offsetPosition(XMFLOAT3 offPos){ position = XMFLOAT3(position.x + offPos.x, position.y + offPos.y, position.z + offPos.z); }

void Camera::setDirection(XMFLOAT3 dir){ direction = dir; }
void Camera::offsetDirection(XMFLOAT3 offDir){ direction = XMFLOAT3(direction.x + offDir.x, direction.y + offDir.y, direction.z + offDir.z); }

void Camera::setRotX(float rad){ rotX = rad; }
void Camera::offsetRotX(float offRad){ rotX += offRad; }

void Camera::setRotY(float rad){ rotY = rad; }
void Camera::offsetRotY(float offRad){ rotY += offRad; }

void Camera::setProjection(XMFLOAT4X4 proj)
{
	projection = proj;
}

void Camera::updateProjection(float a)
{

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * 3.1415926535f,a,0.1f,100.0f);					
	XMStoreFloat4x4(&projection, XMMatrixTranspose(P));

}


XMFLOAT4X4 Camera::getProjection(){ return projection; }
XMFLOAT3 Camera::getPosition(){ return position;}
XMFLOAT3 Camera::getdirection(){ return direction;}
float Camera::getRotX(){ return rotX;}
float Camera::getRotY(){ return rotY;}

XMFLOAT4X4 Camera::getViewMat(){ return viewMat; }

#pragma endregion

void Camera::mouse(float x, float y)
{
	rotX += y * rotScale;
	rotY += x * rotScale;

}

void Camera::update(float dt)
{
	XMVECTOR view = XMQuaternionRotationRollPitchYaw(rotX,rotY,0);

	XMVECTOR dir = XMLoadFloat3(&direction);

	//XMMATRIX rotx = XMMatrixRotationX(rotX);
	//XMMATRIX roty = XMMatrixRotationX(rotY);

	XMVECTOR z = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR p = XMLoadFloat3(&position);

	view = XMVector3Rotate(dir, view);

	/*
	if (GetAsyncKeyState('W') & 0x8000){ p += (XMVector3Normalize(view) * speed *dt);}
	if (GetAsyncKeyState('S') & 0x8000) { p += (XMVector3Normalize(view) * -speed*dt); }
	if (GetAsyncKeyState('A') & 0x8000) { p += (XMVector3Cross(view, up) * speed*dt); }
	if (GetAsyncKeyState('D') & 0x8000) { p += (XMVector3Cross(view, up) * -speed*dt); }
	if (GetAsyncKeyState(' ') & 0x8000) { p += up * speed*dt; }
	if (GetAsyncKeyState('X') & 0x8000) { p += up * -speed*dt; }
	*/

	//XMStoreFloat3(&position, p);
	//XMStoreFloat3(&direction, view);


	// Create the View matrix
	// In an actual game, update this when the camera moves (every frame)

	
	//XMMATRIX V = XMMatrixLookAtLH(p, d, up);
	//XMStoreFloat4x4(&viewMat, XMMatrixTranspose(V));

	

	XMMATRIX v = XMMatrixLookToLH(p, view, up);

	XMStoreFloat4x4(&viewMat, XMMatrixTranspose(v));

	
}