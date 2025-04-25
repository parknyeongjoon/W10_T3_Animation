#pragma once
#include "Delegate.h"

#define DECLARE_DELEGATE(DelegateName) FUNC_DECLARE_DELEGATE(DelegateName, void)
#define DECLARE_MULTICAST_DELEGATE(DelegateName) FUNC_DECLARE_MULTICAST_DELEGATE(DelegateName, void)

#define DECLARE_DELEGATE_OneParam(DelegateName, Param1Type) FUNC_DECLARE_DELEGATE(DelegateName, void, Param1Type)
#define DECLARE_MULTICAST_DELEGATE_OneParam(DelegateName, Param1Type) FUNC_DECLARE_MULTICAST_DELEGATE(DelegateName, void, Param1Type)

#define DECLARE_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type) FUNC_DECLARE_DELEGATE(DelegateName, void, Param1Type, Param2Type)
#define DECLARE_MULTICAST_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type) FUNC_DECLARE_MULTICAST_DELEGATE(DelegateName, void, Param1Type, Param2Type)

#define DECLARE_DELEGATE_ThreeParams(DelegateName, Param1Type, Param2Type, Param3Type) FUNC_DECLARE_DELEGATE(DelegateName, void, Param1Type, Param2Type, Param3Type)
#define DECLARE_MULTICAST_DELEGATE_ThreeParams(DelegateName, Param1Type, Param2Type, Param3Type) FUNC_DECLARE_MULTICAST_DELEGATE( DelegateName, void, Param1Type, Param2Type, Param3Type)
