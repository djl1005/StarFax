// ----------------------------------------------------------------------------
//  A few notes on project settings
//
//  - The project is set to use the UNICODE character set
//    - This was changed in Project Properties > Config Properties > General > Character Set
//    - This basically adds a "#define UNICODE" to the project
//
//  - The include directories were automagically correct, since the DirectX 
//    headers and libs are part of the windows SDK
//    - For instance, $(WindowsSDK_IncludePath) is set as a project include 
//      path by default.  That's where the DirectX headers are located.
//
//  - Two libraries had to be manually added to the Linker Input Dependencies
//    - d3d11.lib
//    - d3dcompiler.lib
//    - This was changed in Project Properties > Config Properties > Linker > Input > Additional Dependencies
//
//  - The Working Directory was changed to match the actual .exe's 
//    output directory, since we need to load the compiled shader files at run time
//    - This was changed in Project Properties > Config Properties > Debugging > Working Directory
//
// ----------------------------------------------------------------------------

#include <Windows.h>
#include <d3dcompiler.h>
#include "MyDemoGame.h"


#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Make the game, initialize and run
	MyDemoGame game(hInstance);

	// If we can't initialize, we can't run
	if (!game.Init())
		return 0;

	// All set to run the game
	return game.Run();
}

#pragma endregion

#pragma region Constructor / Destructor

MyDemoGame::MyDemoGame(HINSTANCE hInstance) : DirectXGame(hInstance)
{
	// Set up our custom caption and window size
	windowCaption = L"Demo DX11 Game";
	windowWidth = 800;
	windowHeight = 600;

}

MyDemoGame::~MyDemoGame()
{
	// Release all of the D3D stuff that's still hanging out
	ReleaseMacro(vertexBuffer);
	ReleaseMacro(indexBuffer);


	delete(box);
	delete(sphere);
	delete(mesh3);

}

#pragma endregion

#pragma region Initialization

// Initializes the base class (including the window and D3D),
// sets up our geometry and loads the shaders (among other things)
bool MyDemoGame::Init()
{
	// Make sure DirectX initializes properly
	if (!DirectXGame::Init())
		return false;

	// Create the necessary DirectX buffers to draw something
	CreateGeometryBuffers();

	// Load pixel & vertex shaders, and then create an input layout
	LoadShadersAndInputLayout();

	dlight1.AmbientColor = XMFLOAT4(0.3, 0.3, 0.3, 1);
	dlight1.DiffuseColor = XMFLOAT4(.8, .8, .8, 1);
	dlight1.Direction = XMFLOAT3(0, -1, 1);

	// Set up camera-related matrices
	InitializeCameraMatrices();

	entity = Player(playerMesh, playerMat, cam);
	e = Enemy(enemyMesh, enemyMat);

	terrain = GameEntity(feild, terrainMat);
	terrain.setPosition(0, -3, 0);
	terrain.calcWorld();

	e.setPosition(-3, 0, 10);
	e.calcWorld();

	snowEmitter = new Emitter(XMFLOAT3(-7, 2, 5), XMFLOAT3(3, -0.5, 0), XMFLOAT3(0, 0, 0), XMFLOAT4(1, 0, 0, 1), 1000, 0.03);
	//snowEmitter->setBlendState(device);

	// Successfully initialized
	return true;
}

// Creates the vertex and index buffers for a single triangle
void MyDemoGame::CreateGeometryBuffers()
{
	box = new Mesh("cube.obj", device);
	sphere = new Mesh("sphere.obj", device);
	enemyMesh = new Mesh("Enemy.obj", device);
	playerMesh = new Mesh("Player.obj", device);
	genrateTerrain(device, 1, 512);

	particleMesh = new Mesh("Particle.obj", device);
}

// Loads shaders from compiled shader object (.cso) files, and uses the
// vertex shader to create an input layout which is needed when sending
// vertex data to the device
void MyDemoGame::LoadShadersAndInputLayout()
{
	// Set up the vertex layout description
	// This has to match the vertex input layout in the vertex shader
	// We can't set up the input layout yet since we need the actual vert shader
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	pixelShader = new SimplePixelShader(device, deviceContext);
	vertexShader = new SimpleVertexShader(device, deviceContext);
	

	pixelShader->LoadShaderFile(L"PixelShader.cso");
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	shadowVertexShader = new SimpleVertexShader(device, deviceContext);
	shadowVertexShader->LoadShaderFile(L"ShadowVertexShader.cso");


	LoadParticleShaders();

	CreateWICTextureFromFile(device, deviceContext, L"Brick.jpg", 0, &srv);

	//D3D11_SAMPLER_DESC sam;

	ZeroMemory(&sam, sizeof(D3D11_SAMPLER_DESC));

	sam.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sam.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sam, &sampler);

	mat1 = new Material(vertexShader, pixelShader, srv, sampler);


	//Enemy Material
	CreateWICTextureFromFile(device, deviceContext, L"enemyTexture.jpg", 0, &srv);

	//D3D11_SAMPLER_DESC sam;

	ZeroMemory(&sam, sizeof(D3D11_SAMPLER_DESC));

	sam.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sam.MaxLOD = D3D11_FLOAT32_MAX;;

	device->CreateSamplerState(&sam, &sampler);

	enemyMat = new Material(vertexShader, pixelShader, srv, sampler);



	//Player Material
	CreateWICTextureFromFile(device, deviceContext, L"enemyTexture.jpg", 0, &srv);			//Will be updated with correct texture when textures stop being broken

	//D3D11_SAMPLER_DESC sam;

	

	ZeroMemory(&sam, sizeof(D3D11_SAMPLER_DESC));

	sam.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sam.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sam, &sampler);

	playerMat = new Material(vertexShader, pixelShader, srv, sampler);


	//Terrain Material
	CreateWICTextureFromFile(device, deviceContext, L"Ice.jpg", 0, &srv);

	//D3D11_SAMPLER_DESC sam;

	ZeroMemory(&sam, sizeof(D3D11_SAMPLER_DESC));

	sam.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sam.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sam, &sampler);

	terrainMat = new Material(vertexShader, pixelShader, srv, sampler);

	//Snowflake Material	  
	CreateWICTextureFromFile(device, deviceContext, L"SnowFlake.png", 0, &srv);

	//D3D11_SAMPLER_DESC sam;

	ZeroMemory(&sam, sizeof(D3D11_SAMPLER_DESC));

	sam.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sam.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sam.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sam, &sampler);

	//snowFlake = new Material(vertexShader, particlePixelShader, srv, sampler);



}

void MyDemoGame::LoadParticleShaders()
{
	particleVertexShader = new SimpleVertexShader(device, deviceContext);
	particleVertexShader->LoadShaderFile(L"ParticleVertexShader.cso");

	particlePixelShader = new SimplePixelShader(device, deviceContext);
	particlePixelShader->LoadShaderFile(L"ParticlePixelShader.cso");

	particleGeometryShader = new SimpleGeometryShader(device, deviceContext);
	particleGeometryShader->LoadShaderFile(L"ParticleGeometryShader.cso");

	spawnPGS = new SimpleGeometryShader(device, deviceContext, true, false);
	spawnPGS->LoadShaderFile(L"SpawnPGS.cso");

	spawnPVS = new SimpleVertexShader(device, deviceContext);
	spawnPVS->LoadShaderFile(L"SpawnPVS.cso");

	// Create SO buffers
	spawnPGS->CreateCompatibleStreamOutBuffer(&soBufferRead, 1000000);
	spawnPGS->CreateCompatibleStreamOutBuffer(&soBufferWrite, 1000000);
	spawnFlip = false;
	frameCount = 0;
}

// Initializes the matrices necessary to represent our 3D camera
void MyDemoGame::InitializeCameraMatrices()
{
	cam = new Camera(XMFLOAT3(0, 0, -5), XMFLOAT3(0, 0, 1), 0, 0);
	lightCam = new Camera(XMFLOAT3(0, 25, -25), dlight1.Direction, 0, 0);

	XMFLOAT4X4 proj;

	XMStoreFloat4x4(&proj, XMMatrixTranspose(XMMatrixOrthographicLH(125, 125, .01f, 150)));

	lightCam->setProjection(proj);

	cam->updateProjection(AspectRatio());
	//lightCam->updateProjection(AspectRatio());
}

#pragma endregion

#pragma region Window Resizing

// Handles resizing the window and updating our projection matrix to match
void MyDemoGame::OnResize()
{
	// Handle base-level DX resize stuff
	DirectXGame::OnResize();

	// Update our projection matrix since the window size changed
	if (cam != 0)
	{
		cam->updateProjection(AspectRatio());
		//lightCam->updateProjection(AspectRatio());
	}


}
#pragma endregion

#pragma region Game Loop

// Update your game state
void MyDemoGame::UpdateScene(float dt)
{
	// Take input, update game logic, etc.

	cam->update(dt);
	lightCam->update(dt);

	//entity.offsetPosition(1 * dt, 0, 0);
	//entity.offsetRotation(0, 0,  3 * dt);
	//entity.setPosition(0, 0, 3);
	if (manager.getState() == 1)
	{

		entity.update(dt);

		e.Move(dt);
		e.update(dt);

		entity.Move(dt);

		if (entity.Fire()) {
			Bullet temp = Bullet(sphere, mat1);
			temp.setPosition(entity.getPosition().x, entity.getPosition().y, entity.getPosition().z);
			bullets.push_back(temp);
		}

		for (int i = bullets.size() - 1; i >= 0; i--) {
			bullets[i].update(dt);
			if (bullets[i].getPosition().z > 15) {
				bullets.erase(bullets.begin() + i);
			}

			else if (bullets[i].GetColider().Sat(&e.GetColider()))
			{
				bullets.erase(bullets.begin() + i);
			}
		}

		snowEmitter->update(dt);

		terrain.update(dt);

	}
	
	manager.manageInput();
}

// Clear the screen, redraw everything, present
void MyDemoGame::DrawScene()
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the buffer (erases what's on the screen)
	//  - Do this once per frame
	//  - At the beginning (before drawing anything)
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	deviceContext->ClearDepthStencilView(
		shadowMapStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pixelShader->SetData("light", &dlight1, sizeof(DirectionalLight));

	deviceContext->OMSetRenderTargets(0, nullptr, shadowMapStencilView);


	//shadow draw
	if (manager.getState() == 1)
	{
		
		entity.draw(deviceContext, lightCam, lightCam, "depthTexture");
		e.draw(deviceContext, lightCam, lightCam, "depthTexture");

		for each(Bullet b in bullets) {
			b.draw(deviceContext, lightCam, lightCam, "depthTexture");
		}

		terrain.draw(deviceContext, lightCam, lightCam, "depthTexture");
		

		//entity.drawShadow(deviceContext, shadowVertexShader, lightCam);
		//e.drawShadow(deviceContext, shadowVertexShader, lightCam);
		//terrain.drawShadow(deviceContext, shadowVertexShader, lightCam);
	}
	
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	//normal draw
	if (manager.getState() == 1)
	{
		entity.getMaterial()->getPixShader()->SetShaderResourceView("depthTexture", shadowMap);
		entity.draw(deviceContext, cam, lightCam, "diffuseTexture");
		e.getMaterial()->getPixShader()->SetShaderResourceView("depthTexture", shadowMap);
		e.draw(deviceContext, cam, lightCam, "diffuseTexture");


		for each(Bullet b in bullets) {
			b.draw(deviceContext, cam, lightCam, "diffuseTexture");
		}

		//snowEmitter->drawParticles(deviceContext, cam);
		terrain.getMaterial()->getPixShader()->SetShaderResourceView("depthTexture", shadowMap);
		terrain.draw(deviceContext, cam, lightCam, "diffuseTexture");
	}

	// Present the buffer
	//  - Puts the stuff on the screen
	//  - Do this EXACTLY once per frame
	//  - Always at the end of the frame
	HR(swapChain->Present(0, 0));
}

#pragma endregion

#pragma region Mouse Input

// These methods don't do much currently, but can be used for mouse-related input

void MyDemoGame::OnMouseDown(WPARAM btnState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;

	SetCapture(hMainWnd);
}

void MyDemoGame::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MyDemoGame::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (cam != 0 && btnState & 0x001)
	{
		cam->mouse(x - prevMousePos.x, y - prevMousePos.y);
		//lightCam->mouse(x - prevMousePos.x, y - prevMousePos.y);
	}

	prevMousePos.x = x;
	prevMousePos.y = y;
}
#pragma endregion

XMFLOAT3 avrage(int num, XMFLOAT3 currentValue, XMVECTOR numToAdd)
{

	XMVECTOR temp = XMVectorSet(currentValue.x, currentValue.y, currentValue.z , 0);

	XMFLOAT3 value;

	if (num == 0)
	{
		XMStoreFloat3(&value, numToAdd);
		return value;
	}



	temp *= num;
	temp += numToAdd;
	temp /= num + 1;
	XMStoreFloat3(&value, temp);
	return value;

}



void MyDemoGame::genrateTerrain(ID3D11Device * theDevice, float dist, int size)
{

	Vertex * a = new Vertex[size * size];
	unsigned int * in = new unsigned int[size * size * 6];
	unsigned int * nums = new unsigned int[size * size];
	//tri * tris = new tri[size * size * 2];



	int curentIndex = 0;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			Vertex temp;
			temp.Position = XMFLOAT3((dist * i) - (dist* (size - 1) / 2), cos(i * 3.14 * 2 / size) * 5, (dist * j) - (dist* (size - 1) / 2));
			temp.Normal = XMFLOAT3(sin(i * 3.14 * 2 / size), 1, 0);
			//temp.Normal = XMFLOAT3(0, 0, 0);

			float u = float(i) * 16 / float(size - 1);
			float v = float(j) * 16 / float(size - 1);

			temp.UV = XMFLOAT2(u, v);
			a[curentIndex] = temp;

			curentIndex++;
		}
	}

	int index = 0;

	for (int i = 0; i < size * size; i += 1)
	{
		if (!(i%size == size - 1))
		{

			if (i + size < size * size)
			{
				XMVECTOR aPos;
				XMVECTOR bPos;
				XMVECTOR cPos;
				XMVECTOR ba;
				XMVECTOR bc;
				XMVECTOR n;

				in[index] = i;
				aPos = XMVectorSet(a[in[index]].Position.x, a[in[index]].Position.y, a[in[index]].Position.z, 0);
				index++;

				in[index] = i + 1;
				bPos = XMVectorSet(a[in[index]].Position.x, a[in[index]].Position.y, a[in[index]].Position.z, 0);
				index++;
				in[index] = i + size;
				cPos = XMVectorSet(a[in[index]].Position.x, a[in[index]].Position.y, a[in[index]].Position.z, 0);
				index++;

				bc = bPos - cPos;
				ba = bPos - aPos;

				n = XMVector3Cross(ba, bc);

				a[in[index - 3]].Normal = avrage(nums[in[index - 3]], a[in[index - 3]].Normal, n);
				nums[in[index - 3]] ++;
				a[in[index - 2]].Normal = avrage(nums[in[index - 2]], a[in[index - 2]].Normal, n);
				nums[in[index - 2]] ++;
				a[in[index - 1]].Normal = avrage(nums[in[index -1]], a[in[index -1]].Normal, n);
				nums[in[index - 1]] ++;


			}
			if (i - size >= 0)
			{
				XMVECTOR aPos;
				XMVECTOR bPos;
				XMVECTOR cPos;
				XMVECTOR ba;
				XMVECTOR bc;
				XMVECTOR n;

				in[index] = i;
				aPos = XMVectorSet(a[in[index]].Position.x, a[in[index]].Position.y, a[in[index]].Position.z, 0);
				index++;

				in[index] = i + 1 - size;
				bPos = XMVectorSet(a[in[index]].Position.x, a[in[index]].Position.y, a[in[index]].Position.z, 0);
				index++;

				in[index] = i + 1;
				cPos = XMVectorSet(a[in[index]].Position.x, a[in[index]].Position.y, a[in[index]].Position.z, 0);
				index++;

				bc = bPos - cPos;
				ba = bPos - aPos;

				n = XMVector3Cross(ba, bc);

				a[in[index - 3]].Normal = avrage(nums[in[index - 3]], a[in[index - 3]].Normal, n);
				nums[in[index - 3]] ++;
				a[in[index - 2]].Normal = avrage(nums[in[index - 2]], a[in[index - 2]].Normal, n);
				nums[in[index - 2]] ++;
				a[in[index - 1]].Normal = avrage(nums[in[index - 1]], a[in[index - 1]].Normal, n);
				nums[in[index - 1]] ++;

			}
		}


	}

	feild = new Mesh(a, size* size, in, index, theDevice);

	delete[] a;
	delete[]in;
	delete[] nums;

}