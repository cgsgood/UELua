// *********************************************************************
// Copyright 1998-2024 Tencent Games, Inc. All Rights Reserved.
//  作    者：gershonchen
//  创建日期：2024年06月24日
//  功能描述：lua_State退出作用域后，能够保持栈的平衡
// *********************************************************************

#pragma once

#include "CoreMinimal.h"
#include "lua.hpp"
#include "UObject/Object.h"

// 记录lua_State的栈顶，在退出作用域时，自动弹出所有新增的栈，
// 保证lua_State的栈不残留
class GsFRAMEWORK_API FGsLuaScope
{
public:
	FGsLuaScope(lua_State *L);

	~FGsLuaScope();

protected:
	lua_State *LState;
	int OldTop;
};

class GsFRAMEWORK_API FGsLuaCheckStack
{
public:
	FGsLuaCheckStack(lua_State *L);
	~FGsLuaCheckStack();

protected:
	lua_State *LState;
    int OldTop;
};

// 能够自动pop增加的栈
#define Gs_LUA_SCOPE(L) FGsLuaScope __Gs_lua_scope(L)

#define Gs_LUA_CHECK_STACK(L) FGsLuaCheckStack __Gs_lua_check_stack(L)