#include "Emitter.h"


Emitter::Emitter(XMFLOAT3 pos, XMFLOAT3 vel, XMFLOAT3 rot, XMFLOAT4 col, Mesh* m, Material* mat, float numParticles, float eRate, bool loop)
{
	//Set particle starting values
	startPos = pos;
	startVel = vel;
	startCol = col;
	startRot = rot;

	mesh = m;
	material = mat;

	particleLimit = numParticles;
	emmissionRate = eRate;
	particles = std::vector<Particle*>() ;

	looping = loop;

	idleTime = 5;
	lifeTime = 5;
	totalParticles = 0;

	
}


Emitter::~Emitter()
{
}

void Emitter::setBlendState(ID3D11Device* device)
{
	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	blendStateDesc.AlphaToCoverageEnable = TRUE;
	blendStateDesc.IndependentBlendEnable = FALSE;
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&blendStateDesc, &blendStateAlphaToCoverage);
}

Mesh* Emitter::getMesh(){
	return mesh;
}

Material* Emitter::getMaterial(){
	return material;
}

XMFLOAT3 Emitter::getStartPos()
{
	return startPos;
}

XMFLOAT3 Emitter::getStartVel()
{
	return startVel;
}

XMFLOAT4 Emitter::getStartCol()
{
	return startCol;
}

XMFLOAT3 Emitter::getStartRot()
{
	return startRot;
}

void Emitter::update(float dt)
{
	//Check if we should spawn more particles
	if (particles.size() < particleLimit && idleTime >= emmissionRate)
	{
		//Spawn new particle(s)
		XMFLOAT3 randomPos;
		randomPos.x = startPos.x;
		randomPos.y = startPos.y + rand() % 10 + -5;
		randomPos.z = startPos.z + rand() % 10 + -5;
		Particle* newParticle = new Particle(randomPos, startVel, startRot, startCol, mesh, material);

		particles.push_back(newParticle);

		idleTime = 0;

		totalParticles++;
	}
	else
	{
		idleTime += dt;
	}
	

	//Update particles	  
	for (int i = 0; i < particles.size(); i++)
	{
		//Is the particle too old
		if (particles[i]->getAge() >= lifeTime)
		{
			OutputDebugStringA("Dead Particle");
			delete(particles[i]);
			particles.erase(particles.begin() + i);
			if (looping)
			{
				totalParticles--;
			}
		}
		//Otherwise move the particle
		else
		{
			particles[i]->update(dt, XMFLOAT3(0, 0, 0));
		}
	}
}

void Emitter::drawParticles(ID3D11DeviceContext* context, Camera* cam)
{
	context->OMSetBlendState(blendStateAlphaToCoverage, NULL, 0xFFFFFF);
	for each (Particle* p in particles)
	{
		p->draw(context, cam);
	}
}
