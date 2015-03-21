#pragma once

#include "SimpleShader.h"
class Material
{
public:
	Material(SimpleVertexShader*, SimplePixelShader*, ID3D11ShaderResourceView * s, ID3D11SamplerState * sam);
	~Material();

	void setShaders();

	SimpleVertexShader * getVerShader();
	SimplePixelShader * getPixShader();
	ID3D11ShaderResourceView * getSrv();
	ID3D11SamplerState * getState();
private:
	SimpleVertexShader * verShader;
	SimplePixelShader * pixShader;
	ID3D11ShaderResourceView * srv;
	ID3D11SamplerState * state;
};

