#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Vertex.h"
//for Sprite font
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "Buffers.h"
#include "ConstantBufferTypes.h"

//imGUI
#include "../imGUI/imgui.h"
#include "../imGUI/imgui_impl_win32.h"
#include "../imGUI/imgui_impl_dx11.h"

#include "Object/GameObject.h"

class Graphics
{
	ComPtr<ID3D11Device> device; //그래픽카드
	ComPtr<ID3D11DeviceContext> deviceContext; //렌더링파이프라인
	ComPtr<IDXGISwapChain> swapChain; //이중버퍼링
	ComPtr<ID3D11RenderTargetView> renderTargetView; //출력대상
	ComPtr<ID3D11RasterizerState> rsCullBack;
	ComPtr<ID3D11RasterizerState> rsCullFront;
	//Depth Stencil
	ComPtr<ID3D11DepthStencilView> dsView;
	ComPtr<ID3D11Texture2D> dsBuffer;
	ComPtr<ID3D11DepthStencilState> ds;
	ComPtr<ID3D11DepthStencilState> ds_drawMask;
	ComPtr<ID3D11DepthStencilState> ds_applyMask;
	//Blend State
	ComPtr<ID3D11BlendState> blendState;
	//Sprite font
	unique_ptr<SpriteBatch> spriteBatch;
	unique_ptr<SpriteFont> spriteFont;
	//for Texture
	ComPtr<ID3D11ShaderResourceView> tex;
	ComPtr<ID3D11ShaderResourceView> pink;
	ComPtr<ID3D11ShaderResourceView> grass;
	ComPtr<ID3D11ShaderResourceView> stone;
	ComPtr<ID3D11SamplerState> samp;

	VertexShader vs;
	PixelShader ps;
	VertexShader vs2D;
	PixelShader ps2D;

	bool InitializeDirectX(HWND hWnd, int w, int h);
	bool InitializeShaders();
	bool InitializeScene();
public:
	ConstantBuffer<CB_VS_Transform> cbvs;
	ConstantBuffer<CB_VS_Transform2D> cbvs2D;
	ConstantBuffer<CB_PS_Alpha> cbps;	
	ConstantBuffer<CB_PS_Light> cbLight;
	int ScreenWidth, ScreenHeight;
	GameObject camera;
	GameObject camera2D;
	GameObject obj;
	GameObject sprite;

	bool Initialize(HWND hWnd, int w, int h);
	void RenderFrame();
};

