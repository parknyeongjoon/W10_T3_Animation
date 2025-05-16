#pragma once


enum EObjects : uint8
{
    OBJ_ACTOR,
    OBJ_GAMEPLAYER,
    OBJ_SPHERE,
    OBJ_CAPSULE,
    OBJ_CUBE,
    OBJ_SPOTLIGHT,
    OBJ_POINTLIGHT,
    OBJ_DIRECTIONALLIGHT,
    OBJ_AMBIENTLIGHT,
    OBJ_PARTICLE,
    OBJ_TEXT,
    OBJ_FIREBALL,
    OBJ_TRIANGLE,
    OBJ_CAMERA,
    OBJ_PLAYER,
    OBJ_FOG,
    OBJ_BOX_COLLISION,
    OBJ_SPHERE_COLLISION,
    OBJ_CAPSULE_COLLISION,
    OBJ_SKYSPHERE,
    OBJ_SKELETAL,
    OBJ_CHARACTER,

    OBJ_CAR,
    OBJ_FISH,
    OBJ_PLATFORM,
    OBJ_GOALPLATFORM,
    OBJ_COIN,
    OBJ_TRIGGERBOX,
    OBJ_SKELETALMESH,
    OBJ_SEQUENCERPLAYER,
    OBJ_END
};

enum EArrowDirection : uint8
{
    AD_X,
    AD_Y,
    AD_Z,
    AD_END
};

enum EControlMode : uint8
{
    CM_TRANSLATION,
    CM_ROTATION,
    CM_SCALE,
    CM_END
};
enum ECoordiMode : uint8
{
    CDM_WORLD,
    CDM_LOCAL,
    CDM_END
};
enum EPrimitiveColor : uint8
{
    RED_X,
    GREEN_Y,
    BLUE_Z,
    NONE,
    RED_X_ROT,
    GREEN_Y_ROT,
    BLUE_Z_ROT
};
