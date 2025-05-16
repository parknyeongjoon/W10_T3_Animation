#pragma once
#include "Core/HAL/PlatformType.h"

namespace EEndPlayReason
{
    enum Type : uint8
    {
        /** 명시적인 삭제가 일어났을 때, Destroy()등 */
        Destroyed,
        /** World가 바뀌었을 때 */
        WorldTransition,
        /** 프로그램을 종료했을 때 */
        Quit,
    };
}
namespace EWorldType
{
    enum Type
    {
        /** An untyped world, in most cases this will be the vestigial worlds of streamed in sub-levels */
        None,

        /** The game world */
        Game,

        /** A world being edited in the editor */
        Editor,

        /** A Play In Editor world */
        PIE,

        /** A preview world for an editor tool */
        EditorPreview,

        /** A preview world for a game */
        GamePreview,

        /** A minimal RPC world for a game */
        GameRPC,

        /** An editor world that was loaded but not currently being edited in the level editor */
        Inactive
    };
}
