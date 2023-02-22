#include "Graphics.h"
#include <WICTextureLoader.h>
#include "../Time.h"
#include "../../ComException.h"
#include "Component/ComponentTypes.h"
#include <memory>
#define DC deviceContext
//Input Assembler - Complete
//Vertex Shader - Complete
//Raster Rizer
//Pixel Shader
//Ouput Merger

bool Graphics::InitializeDirectX(HWND hWnd, int w, int h)
{   
    try
    {
        ScreenWidth = w;
        ScreenHeight = h;

        vector<AdapterData> adapters = AdapterReader::GetAdapters();
        if (adapters.size() == 0)
        {
            Debug::Log(L"그래픽 카드를 발견 하지 못했습니다.");
            return false;
        }

        //Create Swapchain
        DXGI_SWAP_CHAIN_DESC scd = { 0 };
        scd.BufferDesc.Width = w;
        scd.BufferDesc.Height = h;
        scd.BufferDesc.RefreshRate.Numerator = 60;
        scd.BufferDesc.RefreshRate.Denominator = 1;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        scd.SampleDesc.Count = 1;
        scd.SampleDesc.Quality = 0;

        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.BufferCount = 1;
        scd.OutputWindow = hWnd;
        scd.Windowed = TRUE;
        scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            adapters[0].pAdapter,
            D3D_DRIVER_TYPE_UNKNOWN,
            NULL, NULL, NULL, 0,
            D3D11_SDK_VERSION,
            &scd,
            swapChain.GetAddressOf(),
            device.GetAddressOf(),
            NULL,
            deviceContext.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"DirectX 초기화에 실패 하였습니다.");        

        ComPtr<ID3D11Texture2D> backBuffer;
        hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
        if (FAILED(hr))
        {
            Debug::Log(hr, L"후면 버퍼를 가져오지 못했습니다.");
            return false;
        }

        hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"Render Target View 생성에 실패 하였습니다.");
        

        //Create Viewport
        CD3D11_VIEWPORT vp(0.0f, 0.0f, w, h);        
        //Set to Rendering pipeline
        DC->RSSetViewports(1, &vp);

        //Create Rasterizer state
        CD3D11_RASTERIZER_DESC rsDesc(D3D11_DEFAULT);        
        hr = device->CreateRasterizerState(&rsDesc, rsCullBack.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"레스터라이저 생성에 실패 하였습니다.");        

        rsDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
        hr = device->CreateRasterizerState(&rsDesc, rsCullFront.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"레스터라이저 생성에 실패 하였습니다.");        

        //Create Depth Stencil State
        CD3D11_DEPTH_STENCIL_DESC dsDesc(D3D11_DEFAULT);        
        hr = device->CreateDepthStencilState(&dsDesc, ds.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"뎁스스텐실버퍼 생성에 실패 하였습니다.");        

        //Create draw Depth Stencil State
        CD3D11_DEPTH_STENCIL_DESC dsDraw(D3D11_DEFAULT);
        dsDraw.DepthEnable = false;
        dsDraw.StencilEnable = true;

        dsDraw.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
        dsDraw.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
        dsDraw.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
        dsDraw.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;

        dsDraw.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
        dsDraw.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
        dsDraw.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
        dsDraw.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR_SAT;

        hr = device->CreateDepthStencilState(&dsDraw, ds_drawMask.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"뎁스스텐실버퍼 생성에 실패 하였습니다.");

        //Create draw Depth Stencil State
        CD3D11_DEPTH_STENCIL_DESC dsApply(D3D11_DEFAULT);        
        dsApply.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
        dsApply.StencilEnable = true;

        dsApply.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
        dsApply.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
        dsApply.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
        dsApply.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;

        dsApply.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
        dsApply.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
        dsApply.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
        dsApply.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;

        hr = device->CreateDepthStencilState(&dsApply, ds_applyMask.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"뎁스스텐실버퍼 생성에 실패 하였습니다.");

        //Create Depth Stencil Buffer
        CD3D11_TEXTURE2D_DESC dsbDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, w, h, 1, 1, D3D11_BIND_DEPTH_STENCIL);        
        hr = device->CreateTexture2D(&dsbDesc, NULL, dsBuffer.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"뎁스스텐실버퍼 생성에 실패 하였습니다.");        

        //Create Depth Stencil View
        hr = device->CreateDepthStencilView(dsBuffer.Get(), NULL, dsView.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"뎁스스텐실 뷰 생성에 실패 하였습니다.");        

        DC->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), dsView.Get());

        //Sprite font
        spriteBatch = make_unique<SpriteBatch>(DC.Get());
        spriteFont = make_unique<SpriteFont>(device.Get(), L"Assets/Fonts/dxfont.font");

        //Create Sampler State
        CD3D11_SAMPLER_DESC samDesc(D3D11_DEFAULT);                        
        hr = device->CreateSamplerState(&samDesc, samp.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"샘플러스테이트 생성에 실패 하였습니다.");        

        //Create Blend State
        CD3D11_BLEND_DESC bsDesc(D3D11_DEFAULT);
        D3D11_RENDER_TARGET_BLEND_DESC rtbDesc = { 0 };

        rtbDesc.BlendEnable = true;
        rtbDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
        rtbDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
        rtbDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;

        rtbDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
        rtbDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
        rtbDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
        rtbDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

        bsDesc.RenderTarget[0] = rtbDesc;

        hr = device->CreateBlendState(&bsDesc, blendState.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"Blend State 생성에 실패 하였습니다.");        
    }
    catch (ComException& error)
    {
        Debug::Log(error.what());
        return false;
    }

    return true;
}

bool Graphics::InitializeShaders()
{
    wstring shaderfolder = L"";
#pragma region DetermineShaderPath
    if (IsDebuggerPresent() == TRUE)
    {
#ifdef _DEBUG
    #ifdef _WIN64
        shaderfolder = L"C:/Users/unwoo52/source/repos/DirectX11Engine/DirectX11Engine/x64/Debug/";
    #else //Wind32
        shaderfolder = L"../Debug/";
    #endif
#else //Release
    #ifdef _WIN64
            shaderfolder = L"../x64/Release/";
    #else //Wind32
            shaderfolder = L"../Release/";
    #endif
#endif
    }
#pragma endregion
    
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONEIDS", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"WEIGHTS", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT numElements = ARRAYSIZE(layout);    

    if (!vs.Initialize(device.Get(), shaderfolder + L"vsSkined.cso", layout, numElements))
    {
        return false;
    }

    if (!ps.Initialize(device.Get(), shaderfolder + L"ps.cso"))
    {
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layout2D[] =
    {
        {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},        
        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},        
    };

    numElements = ARRAYSIZE(layout2D);
    if (!vs2D.Initialize(device.Get(), shaderfolder + L"vsSprite.cso", layout2D, numElements))
    {
        return false;
    }

    if (!ps2D.Initialize(device.Get(), shaderfolder + L"psSprite.cso"))
    {
        return false;
    }
    return true;
}

bool Graphics::InitializeScene()
{
    try
    {   HRESULT hr = cbvs.Initialize(device.Get(), DC.Get());
        COM_ERROR_IF_FAILED(hr, L"콘스탄트 버퍼 생성에 실패 하였습니다.");    

        hr = cbvs2D.Initialize(device.Get(), DC.Get());
        COM_ERROR_IF_FAILED(hr, L"콘스탄트 버퍼 생성에 실패 하였습니다.");

        hr = cbps.Initialize(device.Get(), DC.Get());
        COM_ERROR_IF_FAILED(hr, L"콘스탄트 버퍼 생성에 실패 하였습니다.");      

        hr = cbLight.Initialize(device.Get(), DC.Get());
        COM_ERROR_IF_FAILED(hr, L"콘스탄트 버퍼 생성에 실패 하였습니다.");
        cbLight.data.ambientColor = { 1.f, 1, 1 };
        cbLight.data.ambientStrength = 0.4f;
        cbLight.data.lightColor = { 1, 1, 1 };
        cbLight.data.lightStrength = 1;
        cbLight.data.lightDir = { 1.f, -1.f, 1.f };        

        hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/lion.jpg", nullptr, tex.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"텍스쳐 로딩에 실패 하였습니다.");        

        hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/pink.jpg", nullptr, pink.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"텍스쳐 로딩에 실패 하였습니다.");

        hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/grass.jpg", nullptr, grass.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"텍스쳐 로딩에 실패 하였습니다.");

        hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Assets/Textures/stone.jpg", nullptr, stone.GetAddressOf());
        COM_ERROR_IF_FAILED(hr, L"텍스쳐 로딩에 실패 하였습니다.");

        shared_ptr<Renderer> renderer = make_shared<SkinedMeshRenderer>();
        renderer->Initialize("Assets/Objects/Capoeira.fbx", device.Get(), DC.Get(), tex.Get(), cbvs);
        obj.AddComponent(renderer);
        obj.GetTransform()->SetScale(0.01f, 0.01f, 0.01f);

        shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>();
        if (spriteRenderer->Initialize(device.Get(), DC.Get(), 256, 256, "Assets/Textures/lion.jpg", cbvs2D))
        {
            sprite.AddComponent(spriteRenderer);
            spriteRenderer->SetTransform();
            sprite.GetTransform()->Translate(ScreenWidth/2, ScreenHeight/2, 0);
        }


        shared_ptr<Camera> cam = make_shared<Camera>();        
        cam->SetPerspectiveProjection(90.0f, (float)ScreenWidth / (float)ScreenHeight, 0.1f, 1000.0f);        
        camera.AddComponent(cam);
        camera.GetTransform()->SetPos(0, 0, -4);        

        shared_ptr<Camera> cam2D = make_shared<Camera>();
        cam2D->SetOrthoProjection(ScreenWidth, ScreenHeight, 0.0f, 10.0f);
        camera2D.AddComponent(cam2D);
        camera2D.GetTransform()->SetPos(0, 0, -1);
    }
    catch (ComException& error)
    {
        Debug::Log(error.what());
        return false;
    }
    return true;
}

bool Graphics::Initialize(HWND hWnd, int w, int h)
{
    if (!InitializeDirectX(hWnd, w, h)) return false;
    if (!InitializeShaders()) return false;
    if (!InitializeScene()) return false;
    //Setup imGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(device.Get(), DC.Get());
    ImGui::StyleColorsDark();
    return true;
}

void Graphics::RenderFrame()
{
    float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    DC->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
    DC->ClearDepthStencilView(dsView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
    
    DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    DC->OMSetDepthStencilState(ds.Get(), 0);
    DC->OMSetBlendState(blendState.Get(), NULL, 0xffffffff);

    //Draw
    cbps.data.alpha = 1.0f;
    cbps.Update();
    DC->PSSetConstantBuffers(0, 1, cbps.GetAddressOf());    
    /*
    {//Sprite
        DC->OMSetDepthStencilState(ds_drawMask.Get(), 0);
        DC->IASetInputLayout(vs2D.GetInputLayout());
        DC->VSSetShader(vs2D.GetShader(), 0, 0);
        DC->PSSetShader(ps2D.GetShader(), 0, 0);
        sprite.GetComponent<SpriteRenderer>()->Draw(camera2D.GetComponent<Camera>()->GetViewProjMatrix());
    }
    */
    {//Model
        //DC->OMSetDepthStencilState(ds_applyMask.Get(), 0);
        DC->IASetInputLayout(vs.GetInputLayout());
        cbLight.Update();
        DC->PSSetConstantBuffers(1, 1, cbLight.GetAddressOf());
        DC->VSSetShader(vs.GetShader(), 0, 0);
        DC->PSSetShader(ps.GetShader(), 0, 0);
        //obj.GetTransform()->Rotate(0, Time::GetDeltaTime() * XM_PI, 0);
        obj.Draw(camera.GetComponent<Camera>()->GetViewProjMatrix());
    }

    static int fpsCounter = 0;
    fpsCounter++;
    static float playTime = 0.0f;
    static wstring fpsStr = L"FPS: 0";
    playTime += Time::GetDeltaTime();
    if (playTime >= 1.0f)
    {
        fpsStr = L"FPS: " + to_wstring(fpsCounter);
        fpsCounter = 0;
        playTime = 0.0f;
    }
    
    //Draw Text    
    spriteBatch->Begin();
    spriteFont->DrawString(spriteBatch.get(), fpsStr.c_str(),
        XMFLOAT2(0, 0), Colors::White, 0, XMFLOAT2(0, 0), XMFLOAT2(1, 1));
    spriteBatch->End();

    //Draw imGUI
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    //Create Test Window
    ImGui::Begin("Test");
    //ImGui::DragFloat("Aplha", &alpha, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat3("Light Color", &cbLight.data.lightColor.x, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Light Power", &cbLight.data.lightStrength, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat3("Light Dir", &cbLight.data.lightDir.x, 0.01f, -10.0f, 10.0f);
    ImGui::DragFloat3("Ambient", &cbLight.data.ambientColor.x, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Ambient Power", &cbLight.data.ambientStrength, 0.01f, 0.0f, 1.0f);    
    ImGui::End();
    
    //Draw Call imGUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
     
    swapChain->Present(0, NULL); //swap front <-> back
}
