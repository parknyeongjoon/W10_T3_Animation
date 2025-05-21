#pragma once
#include "Define.h"
#include "Components/SceneComponent.h"
#include "Engine/Texture.h"
#include "UObject/ObjectMacros.h"
#include <wrl/client.h> // Add this include for Microsoft::WRL::ComPtr
#include <Light/ShadowResource.h>
using Microsoft::WRL::ComPtr; // Add this using directive to use ComPtr
struct FLightComponentBaseInfo : public FSceneComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FLightComponentBaseInfo);

    FVector4 Color;
    float Intensity;
    bool bCastShadows;

    // ctor
    FLightComponentBaseInfo()
        : FSceneComponentInfo()
        , Color(FVector4(1, 1, 1, 1))
        , Intensity(1.0f)
        , bCastShadows(false)
    {
        InfoType = TEXT("FLightComponentBaseInfo");
        ComponentClass = TEXT("ULightComponentBase");
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FSceneComponentInfo::Serialize(ar);
        ar << Color << Intensity << bCastShadows;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FSceneComponentInfo::Deserialize(ar);
        ar >> Color >> Intensity >> bCastShadows;
    }
};

class ULightComponentBase : public USceneComponent
{
    DECLARE_CLASS(ULightComponentBase, USceneComponent)

public:
    ULightComponentBase();
    virtual ~ULightComponentBase() override;

    virtual void TickComponent(float DeltaTime) override;
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance);
    virtual void InitializeComponent() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    void InitializeLight();
    void SetColor(FVector4 newColor);
    FVector4 GetColor() const;

protected:
    FVector4 LightColor = { 1, 1, 1, 1 }; // RGBA
    float Intensity = 1.0f;
    bool bCastShadows = true;
    
public:
    FVector4 GetLightColor() const { return LightColor; }
    float GetIntensity() const { return Intensity; }
    void SetIntensity(float InIntensity) { Intensity = InIntensity; }
    bool CanCastShadows() const { return bCastShadows; }
    void SetCastShadows(const bool InbCastShadows) { bCastShadows = InbCastShadows; }

    //FTexture* GetShadowMap() const { return ShadowMap; }
    //ID3D11DepthStencilView* GetDSV() const { return DSV; }

    FTexture* GetLightMap() const { return LightMap; }
    ID3D11RenderTargetView* GetRTV() const { return LightRTV; }

    virtual FMatrix GetViewMatrix() const { return FMatrix::Identity; }
    virtual FMatrix GetProjectionMatrix() const { return FMatrix::Identity; }

    // virtual void CreateShadowMap();

    // duplictae
    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;
    
    virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;

protected:
    //FTexture* ShadowMap = nullptr;
    //ID3D11DepthStencilView* DSV = nullptr;

    FTexture* LightMap = nullptr;
    ID3D11RenderTargetView* LightRTV = nullptr;

protected:
    FShadowResource* ShadowResource = nullptr;
public:
    FShadowResource* GetShadowResource() const { return ShadowResource; }
};

