#pragma once
#include "HAL/PlatformType.h"

enum EViewModeIndex : uint8
{
    VMI_Lit_Goroud = 0,
    VMI_Lit_Lambert = 1,
    VMI_Lit_Phong = 2,
    VMI_Unlit = 3,
    VMI_Wireframe = 4,
    VMI_Depth = 5,
    VMI_Normal = 6,
    MAXCOUNT = 7,
};

enum ELevelTick : uint8
{
    /** Update the level time only. */
    LEVELTICK_TimeOnly = 0,
    /** Update time and viewports. */
    LEVELTICK_ViewportsOnly = 1,
    /** Update all. */
    LEVELTICK_All = 2,
    /** Delta time is zero, we are paused. Components don't tick. */
    LEVELTICK_PauseTick = 3,
};

enum ELevelViewportType : uint8
{
    LVT_Perspective = 0,
    /** Top */
    LVT_OrthoXY = 1,
    /** Bottom */
    LVT_OrthoNegativeXY = 2,
    /** Left */
    LVT_OrthoYZ = 3,
    /** Right */
    LVT_OrthoNegativeYZ = 4,
    /** Front */
    LVT_OrthoXZ = 5,
    /** Back */
    LVT_OrthoNegativeXZ = 6,
    LVT_MAX = 7,
    LVT_None = 255,
};

enum class EEditorState : uint8
{
    Editing,        // 기본 에디터
    PreparingPlay,  // 씬 복사 및 Playing 모드로 전환 전에 해야 할 일
    Playing,        // Playing 로직 
    Paused,         // 게임 일시 중단
    Resuming,       // Playing 재개하기 전 해야 할 일
    Stopped,        // Playing 씬 정리 및 에디팅 모드로 전환
};
