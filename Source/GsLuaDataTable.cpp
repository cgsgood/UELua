#include "GsLuaDataTable.h"

#include "GsLuaDataTableImp.h"
#include "UnLuaModule.h"
#include "GsStr.h"
#include "Log/GsLog.h"

UGsLuaDataTable* UGsLuaDataTable::GetLuaDataTable(UObject* WorldContext, const FString& TableName)
{
	if (!IsValid(WorldContext))
		return nullptr;

	UGsLuaDataTable* LuaTableObj = NewObject<UGsLuaDataTable>(WorldContext, *TableName);

	UnLua::FLuaEnv* LuaEnv = IUnLuaModule::Get().GetEnv(WorldContext);
	LuaTableObj->Imp = MakeShared<FGsLuaDataTableImp>(WorldContext, TableName, LuaEnv, nullptr, false);

	return LuaTableObj;
}

// 以A.B.C的形式，读取lua表格中的值
// A、B、C可以是字符串，也可以是字符串整数，内部会自动转化为lua的整数索引
template <typename TValue>
static void ReadLuaDataTableAsByKeyChain(
	UObject* WorldContext, const TSharedPtr<FGsLuaDataTableImp>& Imp, const FString& KeyChain, TValue& OutValue)
{
	TArray<TSharedPtr<FGsLuaDataTableImp>> ImpStack{Imp};

	int BeginIndex = 0;
	int EndIndex = 0;
	const auto KeyChainCStr = StringCast<UTF8CHAR>(*KeyChain);

	int PartID = 0;
	while (true)
	{
		const char& ItrCh = KeyChainCStr.Get()[EndIndex];
		if (ItrCh == '\0')
			break;

		if (ItrCh == '.')
		{
			TOptional<int> OptIndex = UGsStr::TryParseInt(KeyChainCStr.Get(), BeginIndex, EndIndex - 1);

			const TSharedPtr<FGsLuaDataTableImp>& TopImp = ImpStack[ImpStack.Num() - 1];
			TSharedPtr<FGsLuaDataTableImp> NextImp;
			if (OptIndex.IsSet())
			{
				NextImp = TopImp->ReadIndexAsTable(OptIndex.GetValue());
			}
			else
			{
				FString SubKey = FString::ConstructFromPtrSize(KeyChainCStr.Get() + BeginIndex, EndIndex - BeginIndex);
				NextImp = TopImp->ReadKeyAsTable(SubKey);
			}

			BeginIndex = EndIndex + 1;
			EndIndex = BeginIndex;

			if (!NextImp)
			{
				Gs_UE_LOG(WorldContext, LogUnLua, Error,
							TEXT("%s Read Lua Table ['%s'] Key Chain '%s' fail at part %d"),
							*WorldContext->GetFullName(), *Imp->GetTableName(), *KeyChain, PartID);
				return;
			}

			ImpStack.Emplace(NextImp);
			++PartID;
		}
		else
		{
			++EndIndex;
		}
	}

	const TSharedPtr<FGsLuaDataTableImp>& TopImp = ImpStack[ImpStack.Num() - 1];
	TOptional<int> OptIndex = UGsStr::TryParseInt(KeyChainCStr.Get(), BeginIndex, EndIndex - 1);

	if (OptIndex.IsSet())
	{
		TopImp->ReadIndexAs(OptIndex.GetValue(), OutValue);
	}
	else
	{
		FString SubKey = FString::ConstructFromPtrSize(KeyChainCStr.Get() + BeginIndex, EndIndex - BeginIndex);
		TopImp->ReadKeyAs(SubKey, OutValue);
	}
}

template <typename TValue>
static void ReadLuaTableValueByKey(UObject* WorldContext, const FString& TableName,
									const FString& MainKey, const FString& KeyChain, TValue& OutValue)
{
	if (!IsValid(WorldContext))
		return;

	UnLua::FLuaEnv* LuaEnv = IUnLuaModule::Get().GetEnv(WorldContext);
	FGsLuaDataTableImp TableImp(WorldContext, TableName, LuaEnv, nullptr, false);
	TSharedPtr<FGsLuaDataTableImp> Imp = TableImp.ReadKeyAsTable(MainKey);
	return ReadLuaDataTableAsByKeyChain<TValue>(WorldContext, Imp, KeyChain, OutValue);
}

FString UGsLuaDataTable::ReadLuaDataTableStringByKeyChain(UObject* WorldContext, const FString& TableName,
															const FString& MainKey, const FString& KeyChain)
{
	FString Value;
	return ReadLuaTableValueByKey<FString>(WorldContext, TableName, MainKey, KeyChain, Value), Value;
}

int UGsLuaDataTable::ReadLuaDataTableIntByKeyChain(UObject* WorldContext, const FString& TableName,
													const FString& MainKey, const FString& KeyChain)
{
	int Value = 0;
	return ReadLuaTableValueByKey<int>(WorldContext, TableName, MainKey, KeyChain, Value), Value;
}

float UGsLuaDataTable::ReadLuaDataTableFloatByKeyChain(UObject* WorldContext, const FString& TableName,
														const FString& MainKey, const FString& KeyChain)
{
	float Value = 0;
	return ReadLuaTableValueByKey<float>(WorldContext, TableName, MainKey, KeyChain, Value), Value;
}

bool UGsLuaDataTable::ReadLuaDataTableBoolByKeyChain(UObject* WorldContext, const FString& TableName,
													const FString& MainKey, const FString& KeyChain)
{
	bool Value = false;
	return ReadLuaTableValueByKey<bool>(WorldContext, TableName, MainKey, KeyChain, Value), Value;
}


void UGsLuaDataTable::ReadLuaDataTableStringArrayByKeyChain(UObject* WorldContext, const FString& TableName,
															const FString& MainKey, const FString& KeyChain,
															TArray<FString>& OutArray)
{
	ReadLuaTableValueByKey(WorldContext, TableName, MainKey, KeyChain, OutArray);
}

void UGsLuaDataTable::ReadLuaDataTableIntArrayByKeyChain(UObject* WorldContext, const FString& TableName,
														const FString& MainKey, const FString& KeyChain,
														TArray<int>& OutArray)
{
	ReadLuaTableValueByKey(WorldContext, TableName, MainKey, KeyChain, OutArray);
}

void UGsLuaDataTable::ReadLuaDataTableFloatArrayByKeyChain(UObject* WorldContext, const FString& TableName,
															const FString& MainKey, const FString& KeyChain,
															TArray<float>& OutArray)
{
	ReadLuaTableValueByKey(WorldContext, TableName, MainKey, KeyChain, OutArray);
}

void UGsLuaDataTable::ReadLuaDataTableBoolArrayByKeyChain(UObject* WorldContext, const FString& TableName,
															const FString& MainKey, const FString& KeyChain,
															TArray<bool>& OutArray)
{
	ReadLuaTableValueByKey(WorldContext, TableName, MainKey, KeyChain, OutArray);
}

template <typename TValue>
static void ReadLuaTableValueByIndex(UObject* WorldContext, const FString& TableName,
									int MainIndex, const FString& KeyChain, TValue& OutValue)
{
	if (!IsValid(WorldContext))
		return;

	UnLua::FLuaEnv* LuaEnv = IUnLuaModule::Get().GetEnv(WorldContext);
	FGsLuaDataTableImp TableImp(WorldContext, TableName, LuaEnv, nullptr, false);
	TSharedPtr<FGsLuaDataTableImp> Imp = TableImp.ReadIndexAsTable(MainIndex);
	ReadLuaDataTableAsByKeyChain<TValue>(WorldContext, Imp, KeyChain, OutValue);
}

FString UGsLuaDataTable::ReadLuaDataTableStringByIndexChain(UObject* WorldContext, const FString& TableName,
															int MainIndex, const FString& KeyChain)
{
	FString Value;
	return ReadLuaTableValueByIndex<FString>(WorldContext, TableName, MainIndex, KeyChain, Value), Value;
}

int UGsLuaDataTable::ReadLuaDataTableIntByIndexChain(UObject* WorldContext, const FString& TableName, int MainIndex,
													const FString& KeyChain)
{
	int Value = 0;
	return ReadLuaTableValueByIndex<int>(WorldContext, TableName, MainIndex, KeyChain, Value), Value;
}

float UGsLuaDataTable::ReadLuaDataTableFloatByIndexChain(UObject* WorldContext, const FString& TableName,
														int MainIndex, const FString& KeyChain)
{
	float Value = 0;
	return ReadLuaTableValueByIndex<float>(WorldContext, TableName, MainIndex, KeyChain, Value), Value;
}

bool UGsLuaDataTable::ReadLuaDataTableBoolByIndexChain(UObject* WorldContext, const FString& TableName, int MainIndex,
														const FString& KeyChain)
{
	bool Value = false;
	return ReadLuaTableValueByIndex<bool>(WorldContext, TableName, MainIndex, KeyChain, Value), Value;
}

void UGsLuaDataTable::ReadLuaDataTableStringArrayByIndexChain(UObject* WorldContext, const FString& TableName,
																int MainIndex, const FString& KeyChain,
																TArray<FString>& OutArray)
{
	ReadLuaTableValueByIndex(WorldContext, TableName, MainIndex, KeyChain, OutArray);
}

void UGsLuaDataTable::ReadLuaDataTableIntArrayByIndexChain(UObject* WorldContext, const FString& TableName,
															int MainIndex, const FString& KeyChain,
															TArray<int>& OutArray)
{
	ReadLuaTableValueByIndex(WorldContext, TableName, MainIndex, KeyChain, OutArray);
}

void UGsLuaDataTable::ReadLuaDataTableFloatArrayByIndexChain(UObject* WorldContext, const FString& TableName,
															int MainIndex, const FString& KeyChain,
															TArray<float>& OutArray)
{
	ReadLuaTableValueByIndex(WorldContext, TableName, MainIndex, KeyChain, OutArray);
}

void UGsLuaDataTable::ReadLuaDataTableBoolArrayByIndexChain(UObject* WorldContext, const FString& TableName,
															int MainIndex, const FString& KeyChain,
															TArray<bool>& OutArray)
{
	ReadLuaTableValueByIndex(WorldContext, TableName, MainIndex, KeyChain, OutArray);
}

UGsLuaDataTable* UGsLuaDataTable::BeginReadKeyAsTable(const FString& Key)
{
	TSharedPtr<FGsLuaDataTableImp> SubImp = Imp->ReadKeyAsTable(Key);
	if (!SubImp)
		return nullptr;

	UGsLuaDataTable* LuaTableObj = NewObject<UGsLuaDataTable>(GetOuter(), *SubImp->GetTableName());
	LuaTableObj->Imp = SubImp;
	LuaTableStack.Emplace(LuaTableObj);

	return LuaTableObj;
}

FString UGsLuaDataTable::ReadKeyAsString(const FString& Key) const
{
	FString OutValue;
	Imp->ReadKeyAs(Key, OutValue);
	return OutValue;
}

float UGsLuaDataTable::ReadKeyAsFloat(const FString& Key) const
{
	float OutValue = 0;
	Imp->ReadKeyAs(Key, OutValue);
	return OutValue;
}

int UGsLuaDataTable::ReadKeyAsInt(const FString& Key) const
{
	int OutValue = 0;
	Imp->ReadKeyAs(Key, OutValue);
	return OutValue;
}

bool UGsLuaDataTable::ReadKeyAsBool(const FString& Key) const
{
	bool OutValue = false;
	Imp->ReadKeyAs(Key, OutValue);
	return OutValue;
}

void UGsLuaDataTable::ReadKeyAsStringArray(const FString& Key, TArray<FString>& OutArray) const
{
	Imp->ReadKeyAs(Key, OutArray);
}

void UGsLuaDataTable::ReadKeyAsIntArray(const FString& Key, TArray<int>& OutArray) const
{
	Imp->ReadKeyAs(Key, OutArray);
}

void UGsLuaDataTable::ReadKeyAsFloatArray(const FString& Key, TArray<float>& OutArray) const
{
	Imp->ReadKeyAs(Key, OutArray);
}

void UGsLuaDataTable::ReadKeyAsBoolArray(const FString& Key, TArray<bool>& OutArray) const
{
	Imp->ReadKeyAs(Key, OutArray);
}

UGsLuaDataTable* UGsLuaDataTable::BeginReadIndexAsTable(int Index)
{
	TSharedPtr<FGsLuaDataTableImp> SubImp = Imp->ReadIndexAsTable(Index);
	if (!SubImp)
		return nullptr;

	UGsLuaDataTable* LuaTableObj = NewObject<UGsLuaDataTable>(GetOuter(), *SubImp->GetTableName());
	LuaTableObj->Imp = SubImp;
	LuaTableStack.Emplace(LuaTableObj);

	return LuaTableObj;
}

FString UGsLuaDataTable::ReadIndexAsString(int Index) const
{
	FString OutValue;
	Imp->ReadIndexAs(Index, OutValue);
	return OutValue;
}

float UGsLuaDataTable::ReadIndexAsFloat(int Index) const
{
	float OutValue = 0;
	Imp->ReadIndexAs(Index, OutValue);
	return OutValue;
}

int UGsLuaDataTable::ReadIndexAsInt(int Index) const
{
	int OutValue = 0;
	Imp->ReadIndexAs(Index, OutValue);
	return OutValue;
}

bool UGsLuaDataTable::ReadIndexAsBool(int Index) const
{
	bool OutValue = false;
	Imp->ReadIndexAs(Index, OutValue);
	return OutValue;
}

void UGsLuaDataTable::ReadIndexAsStringArray(int Index, TArray<FString>& OutArray) const
{
	Imp->ReadIndexAs(Index, OutArray);
}

void UGsLuaDataTable::ReadIndexAsIntArray(int Index, TArray<int>& OutArray) const
{
	Imp->ReadIndexAs(Index, OutArray);
}

void UGsLuaDataTable::ReadIndexAsFloatArray(int Index, TArray<float>& OutArray) const
{
	Imp->ReadIndexAs(Index, OutArray);
}

void UGsLuaDataTable::ReadIndexAsBoolArray(int Index, TArray<bool>& OutArray) const
{
	Imp->ReadIndexAs(Index, OutArray);
}

void UGsLuaDataTable::EndReadAsTable()
{
	// 结束读取时，要把读取过的所有都结束掉
	if (LuaTableStack.Num() > 0)
	{
		for (int i = LuaTableStack.Num() - 1; i >= 0; --i)
		{
			if (LuaTableStack[i].IsValid())
			{
				LuaTableStack[i]->EndReadAsTable();
			}
		}
		LuaTableStack.Reset();
	}

	Imp->Reset();
}
