#pragma once   

#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include <iostream>

using namespace DirectX;
using namespace std;

// Vertex struct for triangles
struct PVertex
{
	int Type;
	float Age;
	XMFLOAT3 StartPosition;
	XMFLOAT3 StartVelocity;
	XMFLOAT4 StartColor;
	XMFLOAT4 MidColor;
	XMFLOAT4 EndColor;
	XMFLOAT3 StartMidEndSize;
};

class Emitter
{
public:
	Emitter(XMFLOAT3 pos, XMFLOAT3 vel, XMFLOAT3 rot, XMFLOAT4 col, float numParticles, float eRate);
	~Emitter();

	XMFLOAT3 getStartPos();
	XMFLOAT3 getStartVel();
	XMFLOAT4 getStartCol();
	XMFLOAT3 getStartRot();

	void update(float dt);

	void drawParticles(ID3D11DeviceContext* context, Camera* cam);

	void setBlendState(ID3D11Device* device);
	void setShaders(SimpleVertexShader* pvs, SimplePixelShader* pps, SimpleGeometryShader* pgs, 
		SimpleVertexShader* spvs, SimpleGeometryShader* spgs);
	void createBuffers(ID3D11Device* device, ID3D11DeviceContext* context);

private:
	XMFLOAT3 startPos;
	XMFLOAT3 startVel;
	XMFLOAT4 startCol;
	XMFLOAT4 midCol;
	XMFLOAT4 endCol;
	float startSize;
	float midSize;
	float endSize;
	XMFLOAT3 startRot;
	XMFLOAT3 endRot;

	ID3D11BlendState* blendStateAlphaToCoverage;


	float lifeTime;


	float emmissionRate;
	float idleTime;
	float particleLimit;
	float totalParticles;

	SimpleVertexShader* particleVertexShader;
	SimplePixelShader* particlePixelShader;
	SimpleGeometryShader* particleGeometryShader;

	SimpleVertexShader* spawnPVS;
	SimpleGeometryShader* spawnPGS;

	ID3D11Texture1D* randomTexture;
	ID3D11ShaderResourceView* randomSRV;
	ID3D11SamplerState* randomSampler;

	ID3D11BlendState* blendState;
	ID3D11DepthStencilState* depthState;

	//Buffers
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
};

