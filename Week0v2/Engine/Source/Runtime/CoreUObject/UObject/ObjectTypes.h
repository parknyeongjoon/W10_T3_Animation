#pragma once


enum EObjects
{
    OBJ_ACTOR,
    OBJ_GAMEPLAYER,
    OBJ_CUBE,
    OBJ_SPHERE,
    OBJ_CAPSULE,
    OBJ_SPOTLIGHT,
    OBJ_POINTLIGHT,
    OBJ_DIRECTIONALLIGHT,
    OBJ_PARTICLE,
    OBJ_TEXT,
    OBJ_FOG,
    OBJ_CAR,
    OBJ_SKYSPHERE,
    OBJ_YEOUL,
    OBJ_SKELETAL,
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
