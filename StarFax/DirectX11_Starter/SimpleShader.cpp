#include "SimpleShader.h"

// Constructor accepts DirectX device & context, sets up tables
ISimpleShader::ISimpleShader(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Save the device
	this->device = device;
	this->deviceContext = context;

	// Set up fields
	constantBufferCount = 0;
}

// Destructor
ISimpleShader::~ISimpleShader()
{
	// Intentionally left empty
	// Derived class destructors will call this class's CleanUp method
}

// Cleans up the variable table and buffers - Some things will
// be handled by derived classes
void ISimpleShader::CleanUp()
{
	// Handle constant buffers and local data buffers
	for (unsigned int i = 0; i < constantBufferCount; i++)
	{
		constantBuffers[i].ConstantBuffer->Release();
		delete[] constantBuffers[i].LocalDataBuffer;
	}
	delete[] constantBuffers;
	constantBufferCount = 0;

	// Clean up tables
	varTable.clear();
	cbTable.clear();
	samplerTable.clear();
	textureTable.clear();
}

// Loads the specified shader and builds the variable table using
// shader reflection
//
// shaderFile - A "wide string" specifying the compiled shader to load
// 
// Returns true if shader is loaded properly, false otherwise
bool ISimpleShader::LoadShaderFile(LPCWSTR shaderFile)
{
	// Load the shader to a blob and ensure it worked
	ID3DBlob* shaderBlob = 0;
	HRESULT hr = D3DReadFileToBlob(shaderFile, &shaderBlob);
	if (hr != S_OK)
	{
		return false;
	}

	// Create the shader - Calls an overloaded version of this abstract
	// method in the appropriate child class
	shaderValid = CreateShader(shaderBlob);
	if (!shaderValid)
	{
		shaderBlob->Release();
		return false;
	}

	// Set up shader reflection to get information about
	// this shader and its variables,  buffers, etc.
	ID3D11ShaderReflection* refl;
	D3DReflect(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		(void**)&refl);
	
	// Get the description of the shader
	D3D11_SHADER_DESC shaderDesc;
	refl->GetDesc(&shaderDesc);

	// Create an array of constant buffers
	constantBufferCount = shaderDesc.ConstantBuffers;
	constantBuffers = new SimpleConstantBuffer[constantBufferCount];
	
	// Handle bound resources (like shaders and samplers)
	unsigned int resourceCount = shaderDesc.BoundResources;
	for (unsigned int r = 0; r < resourceCount; r++)
	{
		// Get this resource's description
		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		refl->GetResourceBindingDesc(r, &resourceDesc);

		// Check the type
		switch (resourceDesc.Type)
		{
		case D3D_SIT_TEXTURE: // A texture resource
			textureTable.insert(std::pair<std::string, unsigned int>(resourceDesc.Name, resourceDesc.BindPoint));
			break;

		case D3D_SIT_SAMPLER: // A sampler resource
			samplerTable.insert(std::pair<std::string, unsigned int>(resourceDesc.Name, resourceDesc.BindPoint));
			break;
		}
	}

	// Loop through all constant buffers
	for (unsigned int b = 0; b < constantBufferCount; b++)
	{
		// Get this buffer
		ID3D11ShaderReflectionConstantBuffer* cb =
			refl->GetConstantBufferByIndex(b);
		
		// Get the description of this buffer
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		cb->GetDesc(&bufferDesc);

		// Get the description of the resource binding, so
		// we know exactly how it's bound in the shader
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		refl->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc);
		
		// Set up the buffer and put its pointer in the table
		constantBuffers[b].BindIndex = bindDesc.BindPoint;
		cbTable.insert(std::pair<std::string, SimpleConstantBuffer*>(bufferDesc.Name, &constantBuffers[b]));

		// Create this constant buffer
		D3D11_BUFFER_DESC newBuffDesc;
		newBuffDesc.Usage = D3D11_USAGE_DEFAULT;
		newBuffDesc.ByteWidth = bufferDesc.Size;
		newBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		newBuffDesc.CPUAccessFlags = 0;
		newBuffDesc.MiscFlags = 0;
		newBuffDesc.StructureByteStride = 0;
		device->CreateBuffer(&newBuffDesc, 0, &constantBuffers[b].ConstantBuffer);

		// Set up the data buffer for this constant buffer
		constantBuffers[b].LocalDataBuffer = new unsigned char[bufferDesc.Size];
		ZeroMemory(constantBuffers[b].LocalDataBuffer, bufferDesc.Size);

		// Loop through all variables in this buffer
		for (unsigned int v = 0; v < bufferDesc.Variables; v++)
		{
			// Get this variable
			ID3D11ShaderReflectionVariable* var =
				cb->GetVariableByIndex(v);
			
			// Get the description of the variable
			D3D11_SHADER_VARIABLE_DESC varDesc;
			var->GetDesc(&varDesc);

			// Create the variable struct
			SimpleShaderVariable varStruct;
			varStruct.ConstantBufferIndex = b;
			varStruct.ByteOffset = varDesc.StartOffset;
			varStruct.Size = varDesc.Size;
			
			// Get a string version
			std::string varName(varDesc.Name);

			// Add this variable to the table
			varTable.insert(std::pair<std::string, SimpleShaderVariable>(varName, varStruct));
		}
	}

	// All set
	refl->Release();
	shaderBlob->Release();
	return true;
}



// Helper for looking up a variable by name and also
// verifying that it is the requested size
SimpleShaderVariable* ISimpleShader::FindVariable(std::string name, int size)
{
	// Look for the key
	std::unordered_map<std::string, SimpleShaderVariable>::iterator result =
		varTable.find(name);

	// Did we find the key?
	if (result == varTable.end())
		return 0;

	// Grab the result from the iterator
	SimpleShaderVariable* var = &(result->second);

	// Is the data size correct ?
	if (var->Size != size)
		return 0;

	// Success
	return var;
}

// Helper for looking up a constant buffer by name
SimpleConstantBuffer* ISimpleShader::FindConstantBuffer(std::string name)
{
	// Look for the key
	std::unordered_map<std::string, SimpleConstantBuffer*>::iterator result =
		cbTable.find(name);

	// Did we find the key?
	if (result == cbTable.end())
		return 0;

	// Success
	return result->second;
}

// Helper for looking up texture index by name
unsigned int ISimpleShader::FindTextureBindIndex(std::string name)
{
	// Look for the key
	std::unordered_map<std::string, unsigned int>::iterator result =
		textureTable.find(name);

	// Did we find the key?
	if (result == textureTable.end())
		return -1;

	// Success
	return result->second;
}

// Helper for looking up sampler index by name
unsigned int ISimpleShader::FindSamplerBindIndex(std::string name)
{
	// Look for the key
	std::unordered_map<std::string, unsigned int>::iterator result =
		samplerTable.find(name);

	// Did we find the key?
	if (result == samplerTable.end())
		return -1;

	// Success
	return result->second;
}

// Sets the shader and constant buffers in DirectX
//
// copyData - OPTIONAL param for whether to automatically
//            copy data to the shader's constant buffers.
//            Default is to copy - Beware of performance implications
void ISimpleShader::SetShader(bool copyData)
{
	// Ensure the shader is valid
	if (!shaderValid) return;

	// Should we automatically copy the data?
	if (copyData) CopyAllBufferData();

	// Set the shader and any relevant constant buffers
	SetShaderAndCB();
}

// Copies the data to the shader's constant buffers
//
// bufferName - OPTIONAL param for only copying a specific buffer.
//              Useful for updating more frequently-changing
//              variables without having to re-copy all buffers.
void ISimpleShader::CopyBufferData(std::string bufferName)
{
	// Ensure the shader is valid
	if (!shaderValid) return;

	// Check for the buffer
	SimpleConstantBuffer* cb = this->FindConstantBuffer(bufferName);
	if (!cb) return;

	// Copy the data and get out
	deviceContext->UpdateSubresource(
		cb->ConstantBuffer, 0, 0, 
		cb->LocalDataBuffer, 0, 0);
}


// Copies the relevant data to the all of this 
// shader's constant buffers.  To just copy one
// buffer, use CopyBufferData()
void ISimpleShader::CopyAllBufferData()
{
	// Ensure the shader is valid
	if (!shaderValid) return;

	// Loop through the constant buffers and copy all data
	for (unsigned int i = 0; i < constantBufferCount; i++)
	{
		// Copy the entire local data buffer
		deviceContext->UpdateSubresource(
			constantBuffers[i].ConstantBuffer, 0, 0,
			constantBuffers[i].LocalDataBuffer, 0, 0);
	}
}

// Sets a variable by name with arbitrary data of the specified size
//
// name - The name of the shader variable
// data - The data to set in the buffer
// size - The size of the data (this must match the variable's size)
//
// Returns true if data is copied, false if variable doesn't 
// exist or sizes don't match
bool ISimpleShader::SetData(std::string name, const void* data, unsigned int size)
{
	// Look for the variable and verify
	SimpleShaderVariable* var = FindVariable(name, size);
	if (var == 0)
		return false;

	// Set the data in the local data buffer
	memcpy(
		constantBuffers[var->ConstantBufferIndex].LocalDataBuffer + var->ByteOffset,
		data,
		size);

	// Success
	return true;
}

// INT ------------------------------------------------------------------------
bool ISimpleShader::SetInt(std::string name, int data)
{
	return this->SetData(name, (void*)(&data), sizeof(int));
}

// FLOAT ----------------------------------------------------------------------

// Sets a float variable by name in the local data buffer
bool ISimpleShader::SetFloat(std::string name, float data)
{
	return this->SetData(name, (void*)(&data), sizeof(float));
}

// FLOAT2 ---------------------------------------------------------------------

// Sets a float2 variable by name in the local data buffer
bool ISimpleShader::SetFloat2(std::string name, const float data[2])
{
	return this->SetData(name, (void*)data, sizeof(float) * 2);
}

// Sets a float2 variable by name in the local data buffer
bool ISimpleShader::SetFloat2(std::string name, const DirectX::XMFLOAT2 data)
{
	return this->SetData(name, &data, sizeof(float) * 2);
}

// FLOAT3 ---------------------------------------------------------------------

// Sets a float3 variable by name in the local data buffer
bool ISimpleShader::SetFloat3(std::string name, const float data[3])
{
	return this->SetData(name, (void*)data, sizeof(float) * 3);
}

// Sets a float3 variable by name in the local data buffer
bool ISimpleShader::SetFloat3(std::string name, const DirectX::XMFLOAT3 data)
{
	return this->SetData(name, &data, sizeof(float) * 3);
}

// FLOAT4 ---------------------------------------------------------------------

// Sets a float4 variable by name in the local data buffer
bool ISimpleShader::SetFloat4(std::string name, const float data[4])
{
	return this->SetData(name, (void*)data, sizeof(float) * 4);
}

// Sets a float4 variable by name in the local data buffer
bool ISimpleShader::SetFloat4(std::string name, const DirectX::XMFLOAT4 data)
{
	return this->SetData(name, &data, sizeof(float) * 4);
}

// 4x4 MATRIX -----------------------------------------------------------------

// Sets a matrix variable by name in the local data buffer
bool ISimpleShader::SetMatrix4x4(std::string name, const float data[16])
{
	return this->SetData(name, (void*)data, sizeof(float) * 16);
}

// Sets a matrix variable by name in the local data buffer
bool ISimpleShader::SetMatrix4x4(std::string name, const DirectX::XMFLOAT4X4 data)
{
	return this->SetData(name, &data, sizeof(float) * 16);
}



///////////////////////////////////////////////////////////////////////////////
// ------ SIMPLE VERTEX SHADER ------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

// Constructor just calls the base
SimpleVertexShader::SimpleVertexShader(ID3D11Device* device, ID3D11DeviceContext* context)
	: ISimpleShader(device, context) { }

// Destructor - Clean up actual shader (base will be called automatically)
SimpleVertexShader::~SimpleVertexShader()
{
	CleanUp();
}

// Handles cleaning up shader and base class destructor
void SimpleVertexShader::CleanUp()
{
	ISimpleShader::CleanUp();
	if (shader) { shader->Release(); shader = 0; }
	if (inputLayout) { inputLayout->Release(); inputLayout = 0; }
}

// Creates the DirectX vertex shader
//
// shaderBlob - The shader's compiled code
//
// Returns true if shader is created correctly, false otherwise
bool SimpleVertexShader::CreateShader(ID3DBlob* shaderBlob)
{
	// Clean up first, in the event this method is
	// called more than once on the same object
	this->CleanUp();

	// Create the shader from the blob
	HRESULT result = device->CreateVertexShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		&shader);

	// Did the creation work?
	if (result != S_OK)
		return false;

	// Vertex shader was created successfully, so we now use the
	// shader code to re-reflect and create an input layout that 
	// matches what the vertex shader expects.  Code adapted from:
	// https://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/

	// Reflect shader info
	ID3D11ShaderReflection* refl;
	D3DReflect(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		(void**)&refl);
	
	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	refl->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (unsigned int i = 0; i< shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		refl->GetInputParameterDesc(i, &paramDesc);

		// Fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// Determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		// Save element desc
		inputLayoutDesc.push_back(elementDesc);
	}

	// Try to create Input Layout
	HRESULT hr = device->CreateInputLayout(
		&inputLayoutDesc[0], 
		inputLayoutDesc.size(), 
		shaderBlob->GetBufferPointer(), 
		shaderBlob->GetBufferSize(),
		&inputLayout);

	// All done, clean up
	refl->Release();
	return true;
}

// Sets the vertex shader, input layout and constant buffers
// for future DirectX drawing
void SimpleVertexShader::SetShaderAndCB()
{
	// Is shader valid?
	if (!shaderValid) return;

	// Set the shader and input layout
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->VSSetShader(shader, 0, 0);

	// Set the constant buffers
	for (unsigned int i = 0; i < constantBufferCount; i++)
	{
		deviceContext->VSSetConstantBuffers(
			constantBuffers[i].BindIndex,
			1,
			&constantBuffers[i].ConstantBuffer);
	}
}

// Sets a shader resource view in the vertex shader stage
//
// name - The name of the texture resource in the shader
// srv - The shader resource view of the texture in GPU memory
//
// Returns true if a texture of the given name was found, false otherwise
bool SimpleVertexShader::SetShaderResourceView(std::string name, ID3D11ShaderResourceView* srv)
{
	// Look for the variable and verify
	unsigned int bindIndex = FindTextureBindIndex(name);
	if (bindIndex == -1)
		return false;

	// Set the shader resource view
	deviceContext->VSSetShaderResources(bindIndex, 1, &srv);

	// Success
	return true;
}

// Sets a sampler state in the vertex shader stage
//
// name - The name of the sampler state in the shader
// samplerState - The sampler state in GPU memory
//
// Returns true if a sampler of the given name was found, false otherwise
bool SimpleVertexShader::SetSamplerState(std::string name, ID3D11SamplerState* samplerState)
{
	// Look for the variable and verify
	unsigned int bindIndex = FindSamplerBindIndex(name);
	if (bindIndex == -1)
		return false;

	// Set the shader resource view
	deviceContext->VSSetSamplers(bindIndex, 1, &samplerState);

	// Success
	return true;
}



///////////////////////////////////////////////////////////////////////////////
// ------ SIMPLE PIXEL SHADER -------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

// Constructor just calls the base
SimplePixelShader::SimplePixelShader(ID3D11Device* device, ID3D11DeviceContext* context)
	: ISimpleShader(device, context) { }

// Destructor - Clean up actual shader (base will be called automatically)
SimplePixelShader::~SimplePixelShader()
{
	CleanUp();
}

// Handles cleaning up shader and base class destructor
void SimplePixelShader::CleanUp()
{
	ISimpleShader::CleanUp();
	if (shader) { shader->Release(); shader = 0; }
}

// Creates the DirectX pixel shader
//
// shaderBlob - The shader's compiled code
//
// Returns true if shader is created correctly, false otherwise
bool SimplePixelShader::CreateShader(ID3DBlob* shaderBlob)
{
	// Clean up first, in the event this method is
	// called more than once on the same object
	this->CleanUp();

	// Create the shader from the blob
	HRESULT result = device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		&shader);

	// Check the result
	return (result == S_OK);
}

// Sets the vertex shader and constant buffers for
// future DirectX drawing
void SimplePixelShader::SetShaderAndCB()
{
	// Is shader valid?
	if (!shaderValid) return;

	// Set the shader
	deviceContext->PSSetShader(shader, 0, 0);

	// Set the constant buffers
	for (unsigned int i = 0; i < constantBufferCount; i++)
	{
		deviceContext->PSSetConstantBuffers(
			constantBuffers[i].BindIndex,
			1,
			&constantBuffers[i].ConstantBuffer);
	}
}

// Sets a shader resource view in the pixel shader stage
//
// name - The name of the texture resource in the shader
// srv - The shader resource view of the texture in GPU memory
//
// Returns true if a texture of the given name was found, false otherwise
bool SimplePixelShader::SetShaderResourceView(std::string name, ID3D11ShaderResourceView* srv)
{
	// Look for the variable and verify
	unsigned int bindIndex = FindTextureBindIndex(name);
	if (bindIndex == -1)
		return false;

	// Set the shader resource view
	deviceContext->PSSetShaderResources(bindIndex, 1, &srv);

	// Success
	return true;
}

// Sets a sampler state in the pixel shader stage
//
// name - The name of the sampler state in the shader
// samplerState - The sampler state in GPU memory
//
// Returns true if a sampler of the given name was found, false otherwise
bool SimplePixelShader::SetSamplerState(std::string name, ID3D11SamplerState* samplerState)
{
	// Look for the variable and verify
	unsigned int bindIndex = FindSamplerBindIndex(name);
	if (bindIndex == -1)
		return false;

	// Set the shader resource view
	deviceContext->PSSetSamplers(bindIndex, 1, &samplerState);

	// Success
	return true;
}