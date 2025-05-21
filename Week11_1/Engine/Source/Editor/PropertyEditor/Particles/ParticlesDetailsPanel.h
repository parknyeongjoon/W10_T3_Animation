#pragma once
#include "UnrealEd/EditorPanel.h"
#include "Classes/Distributions/Distribution.h"
#include "Classes/Distributions/DistributionFloat.h"
#include "Classes/Distributions/DistributionVector.h"

struct ImFont;
struct ImVec2;
class SLevelEditor;
class FParticlePreviewUI;

class ParticlesDetailsPanel : public UEditorPanel
{
public:
    void Initialize(SLevelEditor* LevelEditor, float InWidth, float InHeight);
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;
    void BindUI(FParticlePreviewUI* InUI) { UI = InUI; }

private:
    void RenderFileMenu() const;
    void RenderEditMenu() const;
    void RenderDistributionMenu(FRawDistribution* Distribution, bool bFloat);

private:
    SLevelEditor* activeLevelEditor;
    float Width = 30, Height = 300;
    bool bMenuOpenAny = false; // 아무 메뉴가 열려있는지 여부
    FParticlePreviewUI* UI = nullptr;
};

