#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: TransparentShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TRANSPARENTSHADERCLASS_H_
#define _TRANSPARENTSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "DeviceResources.h"
#include "Light.h"

#include <d3d11.h>
//#include <d3dx10math.h>
//#include <d3dx11async.h>
#include <fstream>
using namespace std;




////////////////////////////////////////////////////////////////////////////////
// Class name: TransparentShader
////////////////////////////////////////////////////////////////////////////////
class TransparentShader
{
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct TransparentBufferType
	{
		float blendAmount;
		DirectX::SimpleMath::Vector3 padding;
	};

public:
	TransparentShader();
	//TransparentShader(const TransparentShader&);
	~TransparentShader();

	//bool Initialize(ID3D11Device*);
	bool InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
	void TransparentShader::EnableShader(ID3D11DeviceContext* context);
	//void Shutdown();
	//bool Render(ID3D11DeviceContext*, int, DirectX::SimpleMath::Matrix, DirectX::SimpleMath::Matrix, DirectX::SimpleMath::Matrix, ID3D11ShaderResourceView*, float);

//private:
	//bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	//void ShutdownShader();
	//void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, DirectX::SimpleMath::Matrix*, DirectX::SimpleMath::Matrix*, DirectX::SimpleMath::Matrix*, ID3D11ShaderResourceView*, float);
	//void RenderShader(ID3D11DeviceContext*, int);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>								m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>								m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_transparentBuffer;
};

#endif