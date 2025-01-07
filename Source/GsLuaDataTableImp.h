// *********************************************************************
// Copyright 1998-2026 Tencent Games, Inc. All Rights Reserved.
// 作   者：gershonchen
// 创建日期：2024年07月24日
// 功能描述：实现Lua表格的读取
// *********************************************************************

#pragma once
#include "LuaValue.h"

namespace UnLua
{
	struct FLuaTable;
	class FLuaEnv;
}

class FGsLuaDataTableImp
{
public:
	FGsLuaDataTableImp(const TWeakObjectPtr<UObject>& InWorldContext, const FString& InTableName,
		UnLua::FLuaEnv* InLuaEnv, const TSharedPtr<UnLua::FLuaTable>& InLuaTable, bool IsTableRefBySelf);

	FGsLuaDataTableImp(const TWeakObjectPtr<UObject>& InWorldContext, FString&& InTableName,
		UnLua::FLuaEnv* InLuaEnv, TSharedPtr<UnLua::FLuaTable>&& InLuaTable, bool IsTableRefBySelf);
	
	~FGsLuaDataTableImp();
	
	const FString& GetTableName() const;

	const TWeakObjectPtr<UObject>& GetWorldContext() const;

	UnLua::FLuaEnv* GetLuaEnv() const;

	void Reset();

	// ----------table作为字典，通过FString Key访问----------begin
	// 读取表格中的Key对应的Value作为表格使用
	TSharedPtr<FGsLuaDataTableImp> ReadKeyAsTable(const FString& Key) const;

	// 读取表格中的Key对应的Value作为string使用
	void ReadKeyAs(const FString& Key, FString& OutValue) const;
	
	// 读取表格中的Key对应的Value作为float使用
	void ReadKeyAs(const FString& Key, float& OutValue) const;

	// 读取表格中的Key对应的Value作为int使用
	void ReadKeyAs(const FString& Key, int& OutValue) const;

	// 读取表格中的Key对应的Value作为bool使用
	void ReadKeyAs(const FString& Key, bool& OutValue) const;

	void ReadKeyAs(const FString& Key, TArray<FString>& OutValue) const;

	void ReadKeyAs(const FString& Key, TArray<int>& OutValue) const;

	void ReadKeyAs(const FString& Key, TArray<float>& OutValue) const;

	void ReadKeyAs(const FString& Key, TArray<bool>& OutValue) const;
	
	// ----------table作为字典，通过FString Key访问----------end

	// ----------table作为数据，通过int Index访问----------begin
	// 读取列表型表格中的Key对应的Value作为表格使用
	TSharedPtr<FGsLuaDataTableImp> ReadIndexAsTable(int Index) const;

	// 读取列表型表格中的Index对应的value作为string使用
	void ReadIndexAs(int Index, FString& OutValue) const;
	
	// 读取列表型表格中的Index对应的value作为float使用
	void ReadIndexAs(int Index, float& OutValue) const;

	// 读取列表型表格中的Index对应的value作为int使用
	void ReadIndexAs(int Index, int& OutValue) const;

	// 读取列表型表格中的Index对应的value作为bool使用
	void ReadIndexAs(int Index, bool& OutValue) const;

	void ReadIndexAs(int Index, TArray<FString>& OutValue) const;
	void ReadIndexAs(int Index, TArray<int>& OutValue) const;
	void ReadIndexAs(int Index, TArray<float>& OutValue) const;
	void ReadIndexAs(int Index, TArray<bool>& OutValue) const;
	// ----------table作为数据，通过int Index访问----------begin

	
protected:
	TWeakObjectPtr<UObject> WorldContext;
	FString TableName;
	UnLua::FLuaEnv* LuaEnv = nullptr;
	TSharedPtr<UnLua::FLuaTable> LuaTable;
	bool bTableRefBySelf = false;
};