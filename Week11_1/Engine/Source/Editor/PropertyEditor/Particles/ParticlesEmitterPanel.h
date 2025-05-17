#pragma once
#include "UnrealEd/EditorPanel.h"

struct ImFont;
struct ImVec2;
class SLevelEditor;

class UParticleEmitter;
class UParticleModule;

class ParticlesEmitterPanel : public UEditorPanel
{
public:
    void Initialize(SLevelEditor* LevelEditor, float InWidth, float InHeight);
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

private:
    void RenderEmitter(UParticleEmitter* Emitter) const;
    void RenderModule(UParticleModule* Module) const;

private:
    SLevelEditor* activeLevelEditor;
    float Width = 30, Height = 300;
    bool bMenuOpenAny = false; // 아무 메뉴가 열려있는지 여부
};

