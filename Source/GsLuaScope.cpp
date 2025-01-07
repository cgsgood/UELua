// *********************************************************************
// Copyright 1998-2024 Tencent Games, Inc. All Rights Reserved.
//  作    者：gershonchen
//  创建日期：2024年06月24日
//  功能描述：lua_State退出作用域后，能够保持栈的平衡
// *********************************************************************


#include "LuaMdl/GsLuaScope.h"

#include "UnLuaBase.h"

FGsLuaScope::FGsLuaScope(lua_State* L)
	: LState(L)
	, OldTop(lua_gettop(L))
{
}

FGsLuaScope::~FGsLuaScope()
{
	const int Top = lua_gettop(LState);
	lua_pop(LState, Top - OldTop);
}

FGsLuaCheckStack::FGsLuaCheckStack(lua_State* L)
	: LState(L)
	, OldTop(lua_gettop(L))
{
}

FGsLuaCheckStack::~FGsLuaCheckStack()
{
	const int Top = lua_gettop(LState);
	if (Top != OldTop)
	{
		UE_LOG(LogUnLua, Error, TEXT("Lua Stack not Balance OldTop = %d NowTop = %d"), OldTop, Top);
	}
}
