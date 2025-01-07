// *********************************************************************
// Copyright 1998-2026 Tencent Games, Inc. All Rights Reserved.
// 作    者：gershonchen
// 创建日期：2024年05月29日
// 功能描述：Copy from PRacing SGLuaEventHelper
//          向Lua发送事件
// *********************************************************************

#pragma once

#include "CoreMinimal.h"
#include "GsLuaBaseClass.h"
#include "UnLuaEx.h"
#include "GsLuaEventMgr.generated.h"

UCLASS(BlueprintType)
class GsFRAMEWORK_API UGsLuaEventMgr : public UGsLuaBaseClass
{
	GENERATED_BODY()
public:
	virtual FString GetModuleName_Implementation() const override { return "GsCommon.Utils.Event.GsLuaEventMgr";}

	// 由小到大排序进行初始化
	virtual int SortOrder() const override;

	//首个参数为事件名String
	template <typename... T>
	void NotifyEventToLua(const char* EventName, T&&... Args)
	{
		lua_State* L = UnLua::GetState(this);
		if (L)
		{
			const UnLua::FLuaRetValues LuaRetValues = UnLua::Call(L, "ReceiveNotifyFromC", EventName, Forward<T>(Args)...);
		}
	}

	void NotifyEventToLua(const char* EventName);
};
