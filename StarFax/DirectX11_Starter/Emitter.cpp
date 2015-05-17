#include "Emitter.h"


Emitter::Emitter(XMFLOAT3 pos, XMFLOAT3 vel, XMFLOAT3 rot, XMFLOAT4 col, float numParticles, float eRate)
{
	//Set particle starting values
	startPos = pos;
	startVel = vel;
	startCol = col;
	startRot = rot;

	particleLimit = numParticles;
	emmissionRate = eRate;

	idleTime = 5;
	lifeTime = 5;
	totalParticles = 0;

	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&world, XMMatrixTranspose(W));
}


Emitter::~Emitter()
{
}

void Emitter::calcWorld()
{

}

void Emitter::setBlendState(ID3D11Device* device, ID3D11DeviceContext* context)
{
}

void Emitter::setShaders(SimpleVertexShader* pvs, SimplePixelShader* pps, SimpleGeometryShader* pgs,
	SimpleVertexShader* spvs, SimpleGeometryShader* spgs)
{
	particleVertexShader = pvs;
	particlePixelShader = pps;
	particleGeometryShader = pgs;

	spawnPVS = spvs;
	spawnPGS = spgs;
}

void Emitter::createBuffers(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Create some temporary variables to represent colors
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices we want to put into the Vertex Buffer
	PVertex vertices[1];
	vertices[0].Type = 0;
	vertices[0].Age = 0.0f;
	vertices[0].StartPosition = startPos;
	vertices[0].StartVelocity = startVel;
	vertices[0].StartColor = startCol;
	vertices[0].MidColor = midCol;
	vertices[0].EndColor = endCol;
	vertices[0].StartMidEndSize = XMFLOAT3(
		startSize,
		midSize,
		endSize);

	// Create the vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(PVertex) * 1; // Number of vertices in the "model" you want to draw
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer));

	// Set up the indices of the vertices (necessary for indexed drawing)
	UINT indices[] = { 0 };

	// Create the index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 1; // Number of indices in the "model" you want to draw
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer));


	// Set up "random" stuff -------------------------------------
	unsigned int randomTextureWidth = 512;

	// Random data for the 1D texture
	std::vector<float> data(randomTextureWidth * 4);
	for (unsigned int i = 0; i < randomTextureWidth * 4; i++)
		data[i] = rand() / (float)RAND_MAX * 2.0f - 1.0f;

	// Set up texture
	D3D11_TEXTURE1D_DESC textureDesc;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.Width = 100;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = (void*)&data[0];
	initData.SysMemPitch = randomTextureWidth * sizeof(float) * 4;
	initData.SysMemSlicePitch = 0;
	device->CreateTexture1D(&textureDesc, &initData, &randomTexture);

	// Set up SRV for texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	srvDesc.Texture1D.MipLevels = 1;
	srvDesc.Texture1D.MostDetailedMip = 0;
	device->CreateShaderResourceView(randomTexture, &srvDesc, &randomSRV);

	// Blend state
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendDesc, &blendState);

	float factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(blendState, factor, 0xffffffff);

	// Sampler
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	device->CreateSamplerState(&samplerDesc, &randomSampler);

	//// Depth state
	D3D11_DEPTH_STENCIL_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthDesc.DepthEnable = false;
	device->CreateDepthStencilState(&depthDesc, &depthState);
	context->OMSetDepthStencilState(depthState, 0);
}

void Emitter::swapSOBuffers(ID3D11Buffer* soBufferRead, ID3D11Buffer* soBufferWrite)
{
	ID3D11Buffer* temp = soBufferRead;
	soBufferRead = soBufferWrite;
	soBufferWrite = temp;
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
}

void Emitter::drawSpawn(ID3D11DeviceContext* context, float frameCount, float dt, float tt, ID3D11Buffer* soBufferRead, ID3D11Buffer* soBufferWrite)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Set/unset correct shaders
	// Set the delta time for the spawning
	spawnPGS->SetFloat("dt", dt);
	spawnPGS->SetFloat("ageToSpawn", emmissionRate);
	spawnPGS->SetFloat("maxLifetime", lifeTime);
	spawnPGS->SetFloat("totalTime", tt);
	spawnPGS->SetSamplerState("randomSampler", randomSampler);
	spawnPGS->SetShaderResourceView("randomTexture", randomSRV);

	spawnPVS->SetShader();
	spawnPGS->SetShader();
	context->PSSetShader(0, 0, 0); // No pixel shader needed

	// Unbind vertex buffers (incase)
	ID3D11Buffer* unset = 0;
	context->IASetVertexBuffers(0, 1, &unset, &stride, &offset);

	// First frame?
	if (frameCount == 0)
	{
		// Draw using the seed vertex
		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->SOSetTargets(1, &soBufferWrite, &offset);
		context->Draw(1, 0);
		frameCount++;
	}
	else
	{
		// Draw using the buffers
		context->IASetVertexBuffers(0, 1, &soBufferRead, &stride, &offset);
		context->SOSetTargets(1, &soBufferWrite, &offset);
		context->DrawAuto();
	}

	// Unbind SO targets
	SimpleGeometryShader::UnbindStreamOutStage(context);

}

void Emitter::drawParticles(ID3D11DeviceContext* context, Camera* cam, float frameCount, float dt, float tt, ID3D11Buffer* soBufferRead, ID3D11Buffer* soBufferWrite)
{
	drawSpawn(context, frameCount, dt, tt, soBufferRead, soBufferWrite);

	swapSOBuffers(soBufferRead, soBufferWrite);

	// Set the current vertex and pixel shaders
	//  - These don't need to be set every frame YET
	//  - Once you have multiple shaders, you will need to change these
	//    between drawing objects that will use different shaders
	particleGeometryShader->SetMatrix4x4("world", world);
	particleGeometryShader->SetMatrix4x4("view", cam->getViewMat());
	particleGeometryShader->SetMatrix4x4("projection", cam->getProjection());

	particleVertexShader->SetFloat3("acceleration", startVel);
	particleVertexShader->SetFloat("maxLifetime", lifeTime);

	particleVertexShader->SetShader(true);
	particlePixelShader->SetShader(true);
	particleGeometryShader->SetShader(true);

	// Set buffers
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &soBufferRead, &stride, &offset);

	// Draw auto because of stream out
	context->DrawAuto();

	
}
