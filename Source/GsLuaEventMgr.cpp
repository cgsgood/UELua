// *********************************************************************
// Copyright 1998-2026 Tencent Games, Inc. All Rights Reserved.
// 作    者：gershonchen
// 创建日期：2024年05月29日
// 功能描述：Copy from PRacing SGLuaEventHelper
//          向Lua发送事件
// *********************************************************************

#include "LuaMdl/GsLuaEventMgr.h"

#include "LuaMdl/GsLuaEnum.h"

void UGsLuaEventMgr::NotifyEventToLua(const char* EventName)
{
	lua_State* L = UnLua::GetState(this);
	if (L)
	{
		const UnLua::FLuaRetValues LuaRetValues = UnLua::Call(L, "ReceiveNotifyFromC", EventName);
	}
}

int UGsLuaEventMgr::SortOrder() const
{
	return static_cast<int>(EGsLuaClassOrder::EventMgr);
}
