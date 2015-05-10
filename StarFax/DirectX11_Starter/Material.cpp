#include "Material.h"


Material::Material(SimpleVertexShader* v, SimplePixelShader* p, ID3D11ShaderResourceView * s , ID3D11SamplerState * sam)
{
	verShader = v;
	pixShader =p;
	srv = s;
	state = sam;
}

SimpleVertexShader * Material::getVerShader()
{
	return verShader;
}

SimplePixelShader * Material::getPixShader()
{
	return pixShader;
}

ID3D11ShaderResourceView * Material::getSrv()
{
	return srv;
}
ID3D11SamplerState * Material::getState()
{
	return state;
}

void Material::setShaders(char * texture)
{

	pixShader->SetSamplerState("basicSampler", state);
	pixShader->SetShaderResourceView(texture, srv);
	//pixShader->SetShaderResourceView("diffuseTexture", srv);

	verShader->SetShader(true);
	pixShader->SetShader(true);
}


Material::~Material()
{
	//delete verShader;
	//delete pixShader;
}
