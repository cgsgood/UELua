// *********************************************************************
// Copyright 1998-2024 Tencent Games, Inc. All Rights Reserved.
//  作    者：gershonchen
//  创建日期：2024年05月28日
//  功能描述：Lua模块的工具类
// *********************************************************************

#include "LuaMdl/GsLuaApiTools.h"

#include "LowLevel.h"
#include "LuaEnv.h"
#include "GsGlobals.h"
#include "UnLuaEx.h"
#include "Engine/Console.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Framework/GsModuleEnum.h"
#include "Framework/GsModuleStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "LuaMdl/GsLuaEnum.h"
#include "LuaMdl/GsLuaMdl.h"

#include "lua.hpp"

// 迁移自 SGLuaInsUtils.cpp
BEGIN_EXPORT_REFLECTED_CLASS(UGsLuaApiTools)
	ADD_STATIC_CFUNTION(OnLuaGetValue)
END_EXPORT_CLASS(UGsLuaApiTools)
IMPLEMENT_EXPORTED_CLASS(UGsLuaApiTools)

EGsModuleNetMode UGsLuaApiTools::GetModuleNetMode() const
{
	return EGsModuleNetMode::Common;
}

void UGsLuaApiTools::Init()
{
	Super::Init();
}

void UGsLuaApiTools::UnInit()
{
	Super::UnInit();
}

int UGsLuaApiTools::SortOrder() const
{
	return static_cast<int>(EGsLuaClassOrder::Tools);
}

UGsLuaApiTools* UGsLuaApiTools::GetInstance(UObject* WorldContextObject)
{
	UGsLuaMdl* LuaMdl = UGsModuleStatics::GetModule<UGsLuaMdl>(WorldContextObject);
	if(LuaMdl)
		return LuaMdl->GetLuaApiTools();
	
	return nullptr;
}

void UGsLuaApiTools::OnConsoleCommand(const FText& CurCmd)
{
	if (auto ViewportConsole = (GEngine->GameViewport != nullptr) ? GEngine->GameViewport->ViewportConsole : nullptr)
	{
		ViewportConsole->ConsoleCommand(*CurCmd.ToString());
	}
	else
	{
		UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), CurCmd.ToString(), nullptr);
	}
}

bool UGsLuaApiTools::IsUObjectValid(UObject* ObjPtr)
{
	if (!ObjPtr || ObjPtr == UnLua::LowLevel::ReleasedPtr)
		return false;
	return (ObjPtr->GetFlags() & (RF_BeginDestroyed | RF_FinishDestroyed)) == 0 && ObjPtr->IsValidLowLevelFast();
}

void UGsLuaApiTools::AddObjectRef(UObject* Object, bool bLog)
{
	if (Object && IsUObjectValid(Object))
	{
		const auto L = UnLua::GetState(Object);
		auto& Env = UnLua::FLuaEnv::FindEnvChecked(L);
		Env.AddManualObjectReference(Object);
		if (bLog)
		{
			SGLogD(TEXT(" AddObjectRef %s (0x%p)"), *Object->GetName(), Object);
		}
	}
	else
	{
		SGFlowLogD(TEXT("Object not valid!!! (0x%p)"), Object);
	}
}

void UGsLuaApiTools::RemoveObjectRef(UObject* Object, bool bLog)
{
	if (Object && IsUObjectValid(Object))
	{
		const auto L = UnLua::GetState(Object);
		auto& Env = UnLua::FLuaEnv::FindEnvChecked(L);
		Env.RemoveManualObjectReference(Object);
		if (bLog)
		{
			SGLogD(TEXT(" RemoveObjectRef %s (0x%p)"), *Object->GetName(), Object);
		}
	}
	else
	{
		SGFlowLogD(TEXT("Object not valid!!! (0x%p)"), Object);
	}
}

FString UGsLuaApiTools::DumpLuaStack(UObject* WorldContextObject, int Count, bool bDumpTable)
{
	if(UnLua::FLuaEnv* LuaEnv = IUnLuaModule::Get().GetEnv(WorldContextObject))
	{
		return DumpLuaStack(LuaEnv->GetMainState(), Count, bDumpTable);
	}

	return FString();
}

static TSet<uint64> SDumpLuaVisitedAddrSet;

FString UGsLuaApiTools::DumpLuaStack(lua_State* L, int Count, bool bDumpTable)
{
	FString OutStr;

	SDumpLuaVisitedAddrSet.Reset();
	const int TopCount = lua_gettop(L);
	int DumpCount = TopCount;
	if(Count > 0 && Count < DumpCount)
	{
		DumpCount = Count;
	}
	
	const int Offset = TopCount - DumpCount;

	for(int i = 1; i <= DumpCount; ++i)
	{
		DumpLuaStackAt(L, i + Offset, OutStr, TEXT(""), bDumpTable);
		if(bDumpTable)
			OutStr.Append(",\n");
		else
			OutStr.Append(", ");
	}

	SDumpLuaVisitedAddrSet.Reset();
	return OutStr;
}

void UGsLuaApiTools::PrintLuaStack(UObject* WorldContextObject, int Count)
{
	FString Str = DumpLuaStack(WorldContextObject, Count);
	Gs_UE_LOG(WorldContextObject, LogUnLua, Display, TEXT("Dump Lua Stack:\n%s"), *Str);
}

void UGsLuaApiTools::DumpLuaStackAt(lua_State* LState, int AtIndex, FString& OutStr, const FString& Padding, bool bDumpTable)
{
	switch (lua_type(LState, AtIndex)) {
	case LUA_TNIL:
		OutStr.Append(TEXT("nil"));
		break;
	case LUA_TBOOLEAN:
		if(lua_toboolean(LState, AtIndex))
			OutStr.Append(TEXT("true"));
		else
			OutStr.Append(TEXT("false"));
		break;
	case LUA_TLIGHTUSERDATA:
		OutStr.Appendf(TEXT("lightuserdata 0x%lx"), reinterpret_cast<uint64>(lua_topointer(LState, AtIndex)));
		break;
	case LUA_TNUMBER:
		{
			const double ValDouble = lua_tonumber(LState, AtIndex);
			const int64 ValInt = static_cast<long long>(ValDouble);
			if(FMath::Abs(ValDouble - ValInt) < 1e-6)
			{
				OutStr.Appendf(TEXT("%lld"), ValInt);
			}
			else
			{
				OutStr.Appendf(TEXT("%g"), ValDouble);
			}
		}
		break;
	case LUA_TSTRING:
		{
			const char* LuaStr = lua_tostring(LState, AtIndex);
			OutStr.Appendf(TEXT("\"%s\""), UTF8_TO_TCHAR(LuaStr));
		}
		break;
	case LUA_TTABLE:
		if(bDumpTable)
		{
			DumpLuaTableAt(LState, AtIndex, OutStr, Padding + TEXT("  "), Padding);
		}
		else
		{
			uint64 TableAddr = reinterpret_cast<uint64>(lua_topointer(LState, AtIndex));
			OutStr.Appendf(TEXT("{table: 0x%lx}"), TableAddr);
		}
		break;
	case LUA_TFUNCTION:
		OutStr.Appendf(TEXT("function 0x%lx"), reinterpret_cast<uint64>(lua_topointer(LState, AtIndex)));
		break;
	case LUA_TUSERDATA:
		OutStr.Appendf(TEXT("userdata 0x%lx"), reinterpret_cast<uint64>(lua_topointer(LState, AtIndex)));
		break;
	case LUA_TTHREAD:
		OutStr.Appendf(TEXT("thread 0x%lx"), reinterpret_cast<uint64>(lua_topointer(LState, AtIndex)));
		break;
	default:
		OutStr.Appendf(TEXT("Unknow 0x%lx"), reinterpret_cast<uint64>(lua_topointer(LState, AtIndex)));
		break;
	}
}

void UGsLuaApiTools::DumpLuaTableAt(lua_State* LState, int AtIndex, FString& OutStr, const FString& Padding, const FString& ParentPadding)
{
	//printf("DumpTableAt begin: %s\n", ss.str().c_str());
	// 把table压入栈顶
	//printf("DumpTableAt top : %d, AtIndex : %d\n", lua_gettop(L), AtIndex);

	uint64 TableAddr = reinterpret_cast<uint64>(lua_topointer(LState, AtIndex));
	if(SDumpLuaVisitedAddrSet.Contains(TableAddr))
	{
		// 避免循环嵌套导致堆栈溢出
		OutStr.Appendf(TEXT("{table: 0x%lx}"), TableAddr);
		return;
	}

	SDumpLuaVisitedAddrSet.Add(TableAddr);
	
	lua_pushvalue(LState, AtIndex);
	//printf("DumpTableAt lua_pushvalue top : %d, AtIndex : %d\n", lua_gettop(L), AtIndex);

	OutStr.Appendf(TEXT("table: 0x%lx{\n"), TableAddr);
	lua_pushnil(LState); // 首先压入一个nil值作为初始键
	while (lua_next(LState, -2) != 0) { // 使用lua_next遍历table
		OutStr.Append(Padding);
		
		// 获取当前键
		DumpLuaStackAt(LState, -2, OutStr, Padding);

		OutStr.Append(TEXT(" = "));

		// 获取当前值
		DumpLuaStackAt(LState, -1, OutStr, Padding);

		OutStr.Append(TEXT(",\n"));
		
		lua_pop(LState, 1); // 弹出栈顶的值，保留键用于下一次迭代
	}
	OutStr.Append(ParentPadding).Append(TEXT("}"));

	//printf("DumpTableAt before pop top : %d, AtIndex : %d\n", lua_gettop(L), AtIndex);
	
	// 弹出栈顶的table
	lua_pop(LState, 1);

	//printf("DumpTableAt after pop top : %d, AtIndex : %d\n", lua_gettop(L), AtIndex);
}

const FString& UGsLuaApiTools::LuaTypToString(int LuaType)
{
	switch(LuaType)
	{
	case LUA_TNONE:
		{
			static FString RetStr(TEXT("LUA_TNONE"));
			return RetStr;
		}
	case LUA_TNIL:
		{
			static FString RetStr(TEXT("LUA_TNIL"));
			return RetStr;
		}
	case LUA_TBOOLEAN:
		{
			static FString RetStr(TEXT("LUA_TBOOLEAN"));
			return RetStr;
		}
	case LUA_TLIGHTUSERDATA:
		{
			static FString RetStr(TEXT("LUA_TLIGHTUSERDATA"));
			return RetStr;
		}
	case LUA_TNUMBER:
		{
			static FString RetStr(TEXT("LUA_TNUMBER"));
			return RetStr;
		}
	case LUA_TSTRING:
		{
			static FString RetStr(TEXT("LUA_TSTRING"));
			return RetStr;
		}
	case LUA_TTABLE:
		{
			static FString RetStr(TEXT("LUA_TTABLE"));
			return RetStr;
		}
	case LUA_TFUNCTION:
		{
			static FString RetStr(TEXT("LUA_TFUNCTION"));
			return RetStr;
		}
	case LUA_TUSERDATA:
		{
			static FString RetStr(TEXT("LUA_TUSERDATA"));
			return RetStr;
		}
	case LUA_TTHREAD:
		{
			static FString RetStr(TEXT("LUA_TTHREAD"));
			return RetStr;
		}
	default:
		{
			static FString RetStr(TEXT("LUA_Error_Unknown"));
			return RetStr;
		}
	}
}

int UGsLuaApiTools::OnLuaGetValue(lua_State* L)
{
	const auto Argc = lua_gettop(L);
	if (Argc == 2)
	{
		UGsLuaBaseClass* LuaBase = Cast<UGsLuaBaseClass>(UnLua::GetUObject(L, 1));
		if (!IsValid(LuaBase))
		{
			luaL_error(L, "call USGLuaBase::GetValue error, Instance in null");
			return 0;
		}

		if (!lua_isstring(L, 2))
		{
			luaL_error(L, "call USGLuaBase::GetValue error, second param is not a string type");
			return 0;
		}
		const char* Key = lua_tostring(L, 2);
		const bool Ret = LuaBase->OnGetValue(L, Key);
		return (Ret ? 1 : 0);
	}

	luaL_error(L, "call USGLuaBase::GetValue error, argc = %d", Argc);
	return 0;
}
