#pragma once
#include "StaticMeshComponent.h"

struct FSphereComponentInfo : public FStaticMeshComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FSphereComponentInfo);
    FSphereComponentInfo()
        : FStaticMeshComponentInfo()
    {
        InfoType = TEXT("FSphereComponentInfo");
        ComponentClass = TEXT("USphereComp");
    }
};

class USphereComp : public UStaticMeshComponent
{
    DECLARE_CLASS(USphereComp, UStaticMeshComponent)

public:
    USphereComp();
    virtual ~USphereComp() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
   

};
