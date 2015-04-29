#include "Emitter.h"


Emitter::Emitter(XMFLOAT3 pos, XMFLOAT3 vel, XMFLOAT3 rot, XMFLOAT3 col, Mesh* m, Material* mat, float numParticles, float eRate, bool loop)
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

XMFLOAT3 Emitter::getStartCol()
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
			particles[i]->update(dt);
		}
	}
}

void Emitter::drawParticles(ID3D11DeviceContext* context, Camera* cam)
{
	for each (Particle* p in particles)
	{
		p->draw(context, cam);
	}
}
