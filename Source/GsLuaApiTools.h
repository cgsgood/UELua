// *********************************************************************
// Copyright 1998-2024 Tencent Games, Inc. All Rights Reserved.
//  作    者：gershonchen
//  创建日期：2024年05月28日
//  功能描述：Lua模块的工具类
// *********************************************************************

#pragma once

#include "CoreMinimal.h"
#include "GsLuaBaseClass.h"
#include "UObject/Object.h"
#include "GsLuaApiTools.generated.h"

// Lua模块的工具类，一些不便于分类的可以放此处
UCLASS(BlueprintType)
class GsFRAMEWORK_API UGsLuaApiTools : public UGsLuaBaseClass
{
	GENERATED_BODY()

public:
	// ----------override方法----------begin
	//覆写Lua路径
	virtual FString GetModuleName_Implementation() const override { return "GsCommon.Native.LuaApiTools"; }

	virtual EGsModuleNetMode GetModuleNetMode() const override;

	//初始化函数
	virtual void Init() override;

	//反初始化函数
	virtual void UnInit() override;

	// 由小到大排序进行初始化
	int SortOrder() const override;
	// ----------override方法----------end

	// ----------外部方法----------begin
	UFUNCTION()
	static UGsLuaApiTools* GetInstance(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable)
	void OnConsoleCommand(const FText& CurCmd);

	UFUNCTION()
	static bool IsUObjectValid(UObject* ObjPtr);
	
	UFUNCTION()
	static void AddObjectRef(UObject* Object, bool bLog = true);

	UFUNCTION()
	static void RemoveObjectRef(UObject* Object, bool bLog = true);

	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf= "WorldContextObject"))
	static FString DumpLuaStack(UObject* WorldContextObject, int Count = 1, bool bDumpTable = true);

	static FString DumpLuaStack(lua_State* L, int Count = 1, bool bDumpTable = true);

	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf= "WorldContextObject"))
	static void PrintLuaStack(UObject* WorldContextObject, int Count = 1);

	static void DumpLuaStackAt(lua_State* LState, int AtIndex, FString& OutStr,
		const FString& Padding = TEXT(""), bool bDumpTable = true);
	static void DumpLuaTableAt(lua_State* LState, int AtIndex, FString& OutStr,
		const FString& Padding = TEXT(""), const FString& ParentPadding = TEXT(""));

	// Lua类型转化为string类型名字
	// @param LuaType 类型，参见：LUA_TNIL, LUA_TBOOLEAN, ...
	static const FString& LuaTypToString(int LuaType);
	
	// Lua端触发使用，不要在C++端调用
	// 迁移自SGLuaInsUtils.h OnLuaGetValue
	static int OnLuaGetValue(lua_State* L);
	// ----------外部方法----------end
};
