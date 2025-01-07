// *********************************************************************
// Copyright 1998-2026 Tencent Games, Inc. All Rights Reserved.
// 作      者：gershonchen
// 创建日期：2024年07月18日
// 功能描述：能够读取lua表格，用于蓝图中读表
// *********************************************************************

#pragma once
#include "CoreMinimal.h"
#include "GsLuaDataTable.generated.h"

namespace UnLua
{
	class FLuaEnv;
	struct FLuaTable;
}


class FGsLuaDataTableImp;

UCLASS(BlueprintType)
class UGsLuaDataTable : public UObject
{
	GENERATED_BODY()
	
public:
	// 读取lua表格
	// @param TableName 表格名称，在Content\Script\GsCommon\data\下，不包含.lua后缀
	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static UGsLuaDataTable* GetLuaDataTable(UObject* WorldContext, const FString& TableName);

	// 读取lua表格中的字符串
	// @param TableName 表格名称，在Content\Script\GsCommon\data\下，不包含.lua后缀
	// @param MainKey, 字符串主键，用于字典类lua表格
	// @param KeyChain, 字符串子键，以'.'分隔，如"a.b.c"或'1.2.3'
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static FString ReadLuaDataTableStringByKeyChain(UObject* WorldContext, const FString& TableName, const FString& MainKey, const FString& KeyChain);

	// 读取lua表格中的整数
	// @param TableName 表格名称，在Content\Script\GsCommon\data\下，不包含.lua后缀
	// @param MainKey, 字符串主键，用于字典类lua表格
	// @param KeyChain, 字符串子键，以'.'分隔，如"a.b.c"或'1.2.3'
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static int ReadLuaDataTableIntByKeyChain(UObject* WorldContext, const FString& TableName, const FString& MainKey, const FString& KeyChain);

	// 读取lua表格中的浮点数
	// @param TableName 表格名称，在Content\Script\GsCommon\data\下，不包含.lua后缀
	// @param MainKey, 字符串主键，用于字典类lua表格
	// @param KeyChain, 字符串子键，以'.'分隔，如"a.b.c"或'1.2.3'
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static float ReadLuaDataTableFloatByKeyChain(UObject* WorldContext, const FString& TableName, const FString& MainKey, const FString& KeyChain);

	// 读取lua表格中的bool
	// @param TableName 表格名称，在Content\Script\GsCommon\data\下，不包含.lua后缀
	// @param MainKey, 字符串主键，用于字典类lua表格
	// @param KeyChain, 字符串子键，以'.'分隔，如"a.b.c"或'1.2.3'
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static bool ReadLuaDataTableBoolByKeyChain(UObject* WorldContext, const FString& TableName, const FString& MainKey, const FString& KeyChain);

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static void ReadLuaDataTableStringArrayByKeyChain(UObject* WorldContext, const FString& TableName,
		const FString& MainKey, const FString& KeyChain, TArray<FString>& OutArray);

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static void ReadLuaDataTableIntArrayByKeyChain(UObject* WorldContext, const FString& TableName,
		const FString& MainKey, const FString& KeyChain, TArray<int>& OutArray);

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static void ReadLuaDataTableFloatArrayByKeyChain(UObject* WorldContext, const FString& TableName,
		const FString& MainKey, const FString& KeyChain, TArray<float>& OutArray);

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static void ReadLuaDataTableBoolArrayByKeyChain(UObject* WorldContext, const FString& TableName,
		const FString& MainKey, const FString& KeyChain, TArray<bool>& OutArray);

	
	// 读取lua表格中的字符串
	// @param TableName 表格名称，在Content\Script\GsCommon\data\下，不包含.lua后缀
	// @param MainIndex, 整数主键，用于列表类lua表格
	// @param KeyChain, 字符串子键，以'.'分隔，如"a.b.c"或'1.2.3'
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static FString ReadLuaDataTableStringByIndexChain(UObject* WorldContext, const FString& TableName, int MainIndex, const FString& KeyChain);

	// 读取lua表格中的整数
	// @param TableName 表格名称，在Content\Script\GsCommon\data\下，不包含.lua后缀
	// @param MainIndex, 整数主键，用于列表类lua表格
	// @param KeyChain, 字符串子键，以'.'分隔，如"a.b.c"或'1.2.3'
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static int ReadLuaDataTableIntByIndexChain(UObject* WorldContext, const FString& TableName, int MainIndex, const FString& KeyChain);

	// 读取lua表格中的浮点数
	// @param TableName 表格名称，在Content\Script\GsCommon\data\下，不包含.lua后缀
	// @param MainIndex, 整数主键，用于列表类lua表格
	// @param KeyChain, 字符串子键，以'.'分隔，如"a.b.c"或'1.2.3'
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static float ReadLuaDataTableFloatByIndexChain(UObject* WorldContext, const FString& TableName, int MainIndex, const FString& KeyChain);

	// 读取lua表格中的bool
	// @param TableName 表格名称，在Content\Script\GsCommon\data\下，不包含.lua后缀
	// @param MainIndex, 整数主键，用于列表类lua表格
	// @param KeyChain, 字符串子键，以'.'分隔，如"a.b.c"或'1.2.3'
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static bool ReadLuaDataTableBoolByIndexChain(UObject* WorldContext, const FString& TableName, int MainIndex, const FString& KeyChain);

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static void ReadLuaDataTableStringArrayByIndexChain(UObject* WorldContext, const FString& TableName,
		int MainIndex, const FString& KeyChain, TArray<FString>& OutArray);

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static void ReadLuaDataTableIntArrayByIndexChain(UObject* WorldContext, const FString& TableName,
		int MainIndex, const FString& KeyChain, TArray<int>& OutArray);

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static void ReadLuaDataTableFloatArrayByIndexChain(UObject* WorldContext, const FString& TableName,
		int MainIndex, const FString& KeyChain, TArray<float>& OutArray);

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable", meta = (HidePin="WorldContext", DefaultToSelf="WorldContext"))
	static void ReadLuaDataTableBoolArrayByIndexChain(UObject* WorldContext, const FString& TableName,
		int MainIndex, const FString& KeyChain, TArray<bool>& OutArray);
	
	// ----------table作为字典，通过FString Key访问----------begin
	// 读取表格中的Key对应的Value作为表格使用
	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable")
	UGsLuaDataTable* BeginReadKeyAsTable(const FString& Key);

	// 读取表格中的Key对应的Value作为string使用
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable")
	FString ReadKeyAsString(const FString& Key) const;

	// 读取表格中的Key对应的Value作为float使用
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable")
	float ReadKeyAsFloat(const FString& Key) const;

	// 读取表格中的Key对应的Value作为int使用
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable")
	int ReadKeyAsInt(const FString& Key) const;

	// 读取表格中的Key对应的Value作为bool使用
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable")
	bool ReadKeyAsBool(const FString& Key) const;

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable")
	void ReadKeyAsStringArray(const FString& Key, TArray<FString>& OutArray) const;
	
	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable")
	void ReadKeyAsIntArray(const FString& Key, TArray<int>& OutArray) const;
	
	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable")
	void ReadKeyAsFloatArray(const FString& Key, TArray<float>& OutArray) const;

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable")
	void ReadKeyAsBoolArray(const FString& Key, TArray<bool>& OutArray) const;
	// ----------table作为字典，通过FString Key访问----------end

	// ----------table作为数据，通过int Index访问----------begin
	// 读取列表型表格中的Key对应的Value作为表格使用
	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable")
	UGsLuaDataTable* BeginReadIndexAsTable(int Index);

	// 读取列表型表格中的Key对应的value作为string使用
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable")
	FString ReadIndexAsString(int Index) const;

	// 读取列表型表格中的Key对应的value作为float使用
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable")
	float ReadIndexAsFloat(int Index) const;

	// 读取列表型表格中的Key对应的value作为int使用
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable")
	int ReadIndexAsInt(int Index) const;

	// 读取列表型表格中的Key对应的value作为bool使用
	UFUNCTION(BlueprintPure, Category = "GsLuaDataTable")
	bool ReadIndexAsBool(int Index) const;

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable")
	void ReadIndexAsStringArray(int Index, TArray<FString>& OutArray) const;
	
	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable")
	void ReadIndexAsIntArray(int Index, TArray<int>& OutArray) const;
	
	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable")
	void ReadIndexAsFloatArray(int Index, TArray<float>& OutArray) const;

	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable")
	void ReadIndexAsBoolArray(int Index, TArray<bool>& OutArray) const;
	// ----------table作为数据，通过int Index访问----------begin

	// 结束Table的访问，必须要调用，否则lua的栈会有问题
	UFUNCTION(BlueprintCallable, Category = "GsLuaDataTable")
	void EndReadAsTable();
	
protected:
	TSharedPtr<FGsLuaDataTableImp> Imp;
	
	TArray<TWeakObjectPtr<UGsLuaDataTable>> LuaTableStack;
};


