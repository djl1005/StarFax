#pragma once   

#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Particle.h"
#include <iostream>

using namespace DirectX;
using namespace std;
class Emitter
{
public:
	Emitter(XMFLOAT3 pos, XMFLOAT3 vel, XMFLOAT3 rot, XMFLOAT4 col, Mesh* m, Material* mat, float numParticles, float eRate, bool loop);
	~Emitter();

	Mesh* getMesh();
	Material* getMaterial();

	XMFLOAT3 getStartPos();
	XMFLOAT3 getStartVel();
	XMFLOAT4 getStartCol();
	XMFLOAT3 getStartRot();

	void update(float dt);

	void drawParticles(ID3D11DeviceContext* context, Camera* cam);

	void setBlendState(ID3D11Device* device);

private:
	XMFLOAT3 startPos;
	XMFLOAT3 startVel;
	XMFLOAT4 startCol;
	XMFLOAT3 startRot;

	XMFLOAT4 endCol;
	XMFLOAT3 endRot;

	Mesh* mesh;
	Material* material;

	ID3D11BlendState* blendStateAlphaToCoverage;


	float lifeTime;


	float emmissionRate;
	float idleTime;
	float particleLimit;
	float totalParticles;
	std::vector<Particle*> particles;

	bool looping;
};

