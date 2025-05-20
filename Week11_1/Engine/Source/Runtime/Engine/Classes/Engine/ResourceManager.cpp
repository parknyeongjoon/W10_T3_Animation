#include "ResourceManager.h"
#include <wincodec.h>
#include <ranges>
#include "Define.h"
#include "D3D11RHI/GraphicDevice.h"
#include "DirectXTK/Include/DDSTextureLoader.h"
#include "Engine/FLoaderOBJ.h"
#include "UserInterface/Console.h"

void FResourceManager::Initialize(FGraphicsDevice* device)
{
    GraphicDevice = device;
    //RegisterMesh(renderer, "Quad", quadVertices, sizeof(quadVertices) / sizeof(FVertexSimple), quadInices, sizeof(quadInices)/sizeof(uint32));

    //FManagerOBJ::LoadObjStaticMeshAsset("Assets/AxisArrowX.obj");
    //FManagerOBJ::LoadObjStaticMeshAsset("Assets/AxisArrowY.obj");
    //FManagerOBJ::LoadObjStaticMeshAsset("Assets/AxisArrowZ.obj");
    //FManagerOBJ::LoadObjStaticMeshAsset("Assets/AxisScaleArrowX.obj");
    //FManagerOBJ::LoadObjStaticMeshAsset("Assets/AxisScaleArrowY.obj");
    //FManagerOBJ::LoadObjStaticMeshAsset("Assets/AxisScaleArrowZ.obj");
    //FManagerOBJ::LoadObjStaticMeshAsset("Assets//AxisCircleX.obj");
    //FManagerOBJ::LoadObjStaticMeshAsset("Assets//AxisCircleY.obj");
    //FManagerOBJ::LoadObjStaticMeshAsset("Assets//AxisCircleZ.obj");
    // FManagerOBJ::LoadObjStaticMeshAsset("Assets/helloBlender.obj");

	LoadTextureFromFile(GraphicDevice->Device, GraphicDevice->DeviceContext, L"Assets/Texture/ocean_sky.jpg");
	LoadTextureFromFile(GraphicDevice->Device, GraphicDevice->DeviceContext, L"Assets/Texture/font.png");
	LoadTextureFromDDS(GraphicDevice->Device, GraphicDevice->DeviceContext, L"Assets/Texture/font.dds");
	LoadTextureFromFile(GraphicDevice->Device, GraphicDevice->DeviceContext, L"Assets/Texture/emart.png");
	LoadTextureFromFile(GraphicDevice->Device, GraphicDevice->DeviceContext, L"Assets/Texture/T_Explosion_SubUV.png");
	LoadTextureFromFile(GraphicDevice->Device, GraphicDevice->DeviceContext, L"Assets/Texture/UUID_Font.png");
	LoadTextureFromDDS(GraphicDevice->Device, GraphicDevice->DeviceContext, L"Assets/Texture/UUID_Font.dds");
	LoadTextureFromFile(GraphicDevice->Device, GraphicDevice->DeviceContext, L"Assets/Texture/Wooden Crate_Crate_BaseColor.png");
	LoadTextureFromFile(GraphicDevice->Device, GraphicDevice->DeviceContext, L"Assets/Texture/spotLight.png");
	LoadTextureFromFile(GraphicDevice->Device, GraphicDevice->DeviceContext, L"Contents/Textures/Logo.png");
    LoadTextureFromFile(GraphicDevice->Device, GraphicDevice->DeviceContext, L"Contents/Textures/Particles/jewel_particle_yellow@2x.png");
}

void FResourceManager::Release()
{
    for (auto& Pair : Textures)
    {
        if (Pair.Value)
            Pair.Value->Release();
    }
    Textures.Empty();
}

std::shared_ptr<FTexture> FResourceManager::GetTexture(const FWString& name)
{
    auto it = Textures.Find(name);
    if (it != nullptr)
        return *it;

    // 아직 로드되지 않은 경우, 확장자별로 로드
    const wchar_t* filename = name.c_str();
    HRESULT hr = S_OK;
    if (name.size() >= 4 && _wcsicmp(filename + name.size() - 4, L".dds") == 0)
    {
        hr = LoadTextureFromDDS(GraphicDevice->Device, GraphicDevice->DeviceContext, filename);
    }
    else
    {
        hr = LoadTextureFromFile(GraphicDevice->Device, GraphicDevice->DeviceContext, filename);
    }

    if (FAILED(hr))
        return nullptr;

    auto newIt = Textures.Find(name);
    return newIt ? *newIt : nullptr;
}

std::shared_ptr<FTexture> FResourceManager::GetDefaultWhiteTexture()
{
    static const FWString Key   = L"DefaultWhite1x1";
    if (auto Found = Textures.Find(Key))
    {
        return *Found;
    }

    ID3D11Device*           device  = GraphicDevice->Device;
    ID3D11DeviceContext*    ctx     = GraphicDevice->DeviceContext;

    UINT32 whitePixel = 0xFFFFFFFF; // RGBA = (1,1,1,1)
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width              = 1;
    desc.Height             = 1;
    desc.MipLevels          = 1;
    desc.ArraySize          = 1;
    desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count   = 1;
    desc.Usage              = D3D11_USAGE_DEFAULT;
    desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem           = &whitePixel;
    initData.SysMemPitch       = sizeof(whitePixel);
    initData.SysMemSlicePitch  = sizeof(whitePixel);

    ID3D11Texture2D* tex2D = nullptr;
    HRESULT hr = device->CreateTexture2D(&desc, &initData, &tex2D);
    if (FAILED(hr) || !tex2D)
    {
        return nullptr;
    }

    ID3D11ShaderResourceView* srv = nullptr;
    hr = device->CreateShaderResourceView(tex2D, nullptr, &srv);
    if (FAILED(hr) || !srv)
    {
        tex2D->Release();  // SRV 생성 실패 시엔 Texture만 해제
        return nullptr;
    }

    auto DefaultTex = std::make_shared<FTexture>(
        srv,       // ID3D11ShaderResourceView*
        tex2D,     // ID3D11Texture2D*
        1,         // width
        1,         // height
        Key        // path
    );

    // 캐싱
    Textures.Add(Key, DefaultTex);
    return DefaultTex;
}

HRESULT FResourceManager::LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* filename)
{
	IWICImagingFactory* wicFactory = nullptr;
	IWICBitmapDecoder* decoder = nullptr;
	IWICBitmapFrameDecode* frame = nullptr;
	IWICFormatConverter* converter = nullptr;

	// WIC 팩토리 생성
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) return hr;

	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
	if (FAILED(hr)) return hr;


	// 이미지 파일 디코딩
	hr = wicFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
	if (FAILED(hr)) return hr;


	hr = decoder->GetFrame(0, &frame);
	if (FAILED(hr)) return hr;

	// WIC 포맷 변환기 생성 (픽셀 포맷 변환)
	hr = wicFactory->CreateFormatConverter(&converter);
	if (FAILED(hr)) return hr;

	hr = converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
	if (FAILED(hr)) return hr;

	// 이미지 크기 가져오기
	UINT width, height;
	frame->GetSize(&width, &height);
	
	// 픽셀 데이터 로드
	BYTE* imageData = new BYTE[width * height * 4];
	hr = converter->CopyPixels(nullptr, width * 4, width * height * 4, imageData);
	if (FAILED(hr)) {
		delete[] imageData;
		return hr;
	}

	// DirectX 11 텍스처 생성
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = imageData;
	initData.SysMemPitch = width * 4;
	ID3D11Texture2D* Texture2D;
	hr = device->CreateTexture2D(&textureDesc, &initData, &Texture2D);
	delete[] imageData;
	if (FAILED(hr)) return hr;

	// Shader Resource View 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	ID3D11ShaderResourceView* TextureSRV;
	hr = device->CreateShaderResourceView(Texture2D, &srvDesc, &TextureSRV);

	// 리소스 해제
	wicFactory->Release();
	decoder->Release();
	frame->Release();
	converter->Release();

	FWString name = FWString(filename);

	Textures[name] = std::make_shared<FTexture>(TextureSRV, Texture2D, width, height, name);

	Console::GetInstance().AddLog(LogLevel::Warning, "Texture File Load Successs");

    // COM 언인니셜라이즈
    CoUninitialize();
	return hr;
}

HRESULT FResourceManager::LoadTextureFromDDS(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* filename)
{

	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* textureView = nullptr;

	HRESULT hr = DirectX::CreateDDSTextureFromFile(
		device, context,
		filename,
		&texture,
		&textureView
	);
	if (FAILED(hr) || texture == nullptr) abort();

#pragma region WidthHeight

	ID3D11Texture2D* texture2D = nullptr;
	hr = texture->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture2D);
	if (FAILED(hr) || texture2D == nullptr) {
		std::wcerr << L"Failed to query ID3D11Texture2D interface!" << std::endl;
		texture->Release();
		abort();
		return hr;
	}

	// 🔹 텍스처 크기 가져오기
	D3D11_TEXTURE2D_DESC texDesc;
	texture2D->GetDesc(&texDesc);
	uint32 width = static_cast<uint32>(texDesc.Width);
	uint32 height = static_cast<uint32>(texDesc.Height);

#pragma endregion WidthHeight

	FWString name = FWString(filename);

	Textures[name] = std::make_shared<FTexture>(textureView, texture2D, width, height, name);

	Console::GetInstance().AddLog(LogLevel::Warning, "Texture File Load Successs");

	return hr;
}
