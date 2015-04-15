#pragma once

#include <DirectXMath.h>
#include <vector>
#include "DirectXGame.h"
#include "Vertex.h"
#include "Mesh.h"
#include "GameEntity.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"
#include "WICTextureLoader.h"
#include "GameManager.h"
#include "Terrain.h"

// Include run-time memory checking in debug builds
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// For DirectX Math
using namespace DirectX;

struct DirectionalLight
{
	XMFLOAT4 AmbientColor;
	XMFLOAT4 DiffuseColor;
	XMFLOAT3 Direction;
};

// Struct to match vertex shader's constant buffer
// You update one of these locally, then push it to the corresponding
// constant buffer on the device when it needs to be updated
struct VertexShaderConstantBufferLayout
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};

// Demo class which extends the base DirectXGame class
class MyDemoGame : public DirectXGame
{
public:
	MyDemoGame(HINSTANCE hInstance);
	~MyDemoGame();

	// Overrides for base level methods
	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	// For handing mouse input
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	Mesh * box;
	Mesh * sphere;
	Mesh * mesh3;
	Material* mat1;

	Mesh * feild; 

	ID3D11ShaderResourceView * srv;

	ID3D11SamplerState * sampler;

	D3D11_SAMPLER_DESC sam;

	DirectionalLight dlight1;
	DirectionalLight dlight2;

	Camera * cam;

	GameManager manager;

	std::vector<Bullet> bullets;

	Enemy e = Enemy(box, mat1);

	GameEntity terrain = GameEntity(feild, mat1);

	Player entity = Player(box, mat1, cam);

private:
	// Initialization for our "game" demo
	void CreateGeometryBuffers();
	void LoadShadersAndInputLayout();
	void InitializeCameraMatrices();
	void genrateTerrain(ID3D11Device * theDevice, float dist, int size);
private:
	// Buffers to hold actual geometry
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	Vertex a[25];
	unsigned int in[96];

	SimplePixelShader * pixelShader;
	SimpleVertexShader * vertexShader;

	// The matrices to go from model space
	// to screen space
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};