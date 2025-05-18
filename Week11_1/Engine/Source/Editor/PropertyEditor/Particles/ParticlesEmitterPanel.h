#pragma once
#include "UnrealEd/EditorPanel.h"

struct ImFont;
struct ImVec2;
class SLevelEditor;

class UParticleEmitter;
class UParticleModule;
class FParticlePreviewUI;

class ParticlesEmitterPanel : public UEditorPanel
{
public:
    void Initialize(SLevelEditor* LevelEditor, float InWidth, float InHeight);
    virtual void Render() override;
    void RenderModuleCell(const UParticleModule* Module) const;
    virtual void OnResize(HWND hWnd) override;
    void BindUI(FParticlePreviewUI* InUI) { UI = InUI; }

private:
    void RenderEmitter(const UParticleEmitter* Emitter) const;
    void RenderModuleTableRow(const UParticleModule* Module) const;
    //void RenderModule(const UParticleModule* Module) const;

private:
    SLevelEditor* activeLevelEditor;
    float Width = 30, Height = 300;
    bool bMenuOpenAny = false; // 아무 메뉴가 열려있는지 여부
    FParticlePreviewUI* UI = nullptr;
};

