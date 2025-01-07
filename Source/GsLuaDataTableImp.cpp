#include "GsLuaDataTableImp.h"

#include "lua.h"
#include "LuaEnv.h"
#include "LuaValue.h"
#include "GsLuaApiTools.h"
#include "UnLuaLegacy.h"
#include "Log/GsLog.h"

#if WITH_EDITOR
static TAutoConsoleVariable<bool> CVarDebugLuaStack(
		TEXT("GsLuaDataTable.DebugStack"),
		false,
		TEXT("Debug GsLuaDataTable Stack When Read Lua Table"),
		ECVF_Default
	);

// 使用了开关CVarDebugLuaStack，用于调试lua堆栈
#define LOG_LUA_STACK_IMP(STR_FMT) \
	if(CVarDebugLuaStack.GetValueOnAnyThread()) \
	{\
		FString StrStack0 = UGsLuaApiTools::DumpLuaStack(LuaEnv->GetMainState(), 0, false);\
		int Top0 = lua_gettop(LuaEnv->GetMainState());\
		Gs_UE_LOG(WorldContext.Get(), LogUnLua, Warning, TEXT(STR_FMT), *StrStack0, Top0);\
	}

// 不使用开关CVarDebugLuaStack，方便debug时看到local变量
#define LOG_LUA_STACK_IDX_IMP(STR_FMT, IDX) \
	FString StrStack##IDX = UGsLuaApiTools::DumpLuaStack(LuaEnv->GetMainState(), 0, false);\
	int Top##IDX = lua_gettop(LuaEnv->GetMainState());\
	Gs_UE_LOG(WorldContext.Get(), LogUnLua, Warning, TEXT(STR_FMT), *StrStack##IDX, Top##IDX);\

#define LOG_LUA_STACK_IDX(STR_FMT, IDX) LOG_LUA_STACK_IMP(STR_FMT)
// 如果要看到内存中lua堆栈的内容，可以把下面打开，在断点时就能够比较方便看内容中的内容
// #define LOG_LUA_STACK_IDX(STR_FMT, IDX) LOG_LUA_STACK_IDX_IMP(STR_FMT)

#else
#define LOG_LUA_STACK_IDX(STR_FMT, IDX)
#endif

FGsLuaDataTableImp::FGsLuaDataTableImp(const TWeakObjectPtr<UObject>& InWorldContext, const FString& InTableName, UnLua::FLuaEnv* InLuaEnv,
	const TSharedPtr<UnLua::FLuaTable>& InLuaTable, bool IsTableRefBySelf)
	: WorldContext(InWorldContext)
	, TableName(InTableName)
	, LuaEnv(InLuaEnv)
	, LuaTable(InLuaTable)
	, bTableRefBySelf(IsTableRefBySelf)
{
}

FGsLuaDataTableImp::FGsLuaDataTableImp(const TWeakObjectPtr<UObject>& InWorldContext, FString&& InTableName,
	UnLua::FLuaEnv* InLuaEnv, TSharedPtr<UnLua::FLuaTable>&& InLuaTable, bool IsTableRefBySelf)
	: WorldContext(InWorldContext)
	, TableName(MoveTemp(InTableName))
	, LuaEnv(InLuaEnv)
	, LuaTable(MoveTemp(InLuaTable))
	, bTableRefBySelf(IsTableRefBySelf)
{
}

FGsLuaDataTableImp::~FGsLuaDataTableImp()
{
	Reset();
}

const FString& FGsLuaDataTableImp::GetTableName() const
{
	return TableName;
}

const TWeakObjectPtr<UObject>& FGsLuaDataTableImp::GetWorldContext() const
{
	return WorldContext;
}

UnLua::FLuaEnv* FGsLuaDataTableImp::GetLuaEnv() const
{
	return LuaEnv;
}

void FGsLuaDataTableImp::Reset()
{
	if(LuaTable)
	{
		LOG_LUA_STACK_IDX("[FGsLuaDataTableImp]Reset Before %s Top %d", 0)
		
		LuaTable = nullptr;	// 这里会把LuaTable[Key]的每一次访问的Value弹出栈

		LOG_LUA_STACK_IDX("[FGsLuaDataTableImp]Reset Mid %s Top %d", 1)

		if(bTableRefBySelf)
		{
			lua_State* L = LuaEnv->GetMainState();
			lua_pop(L, 1);	// 对应于构造函数中的lua_pushvalue(L, -1);，把自己table的弹出栈
			bTableRefBySelf = false;

			LOG_LUA_STACK_IDX("[FGsLuaDataTableImp]Reset After %s Top %d", 2)
		}
	}
}

TSharedPtr<FGsLuaDataTableImp> FGsLuaDataTableImp::ReadKeyAsTable(const FString& Key) const
{
	LOG_LUA_STACK_IDX("[FGsLuaDataTableImp]ReadKeyAsTable Before Read %s Top %d", 0)
	
	TSharedPtr<UnLua::FLuaTable> LuaTableOfKey;

	lua_State* L = LuaEnv->GetMainState();
	bool IsTableRefBySelf = false;
	if(!LuaTable)
	{
		// ReadCfgRet = Value, ErrMsg
		UnLua::FLuaRetValues ReadCfgRet = UnLua::Call(L, "ReadCfg", TableName, Key);

		LOG_LUA_STACK_IDX("[FGsLuaDataTableImp]ReadKeyAsTable ReadCfg %s Top %d", 1)
		
		if(ReadCfgRet[1].GetType() == LUA_TSTRING)
		{
			// 有错误
			FString ErrorMsg = ReadCfgRet[1];
			Gs_UE_LOG(WorldContext.Get(), LogUnLua, Error, TEXT("[FGsLuaDataTableImp]Error %s"), *ErrorMsg);
			return nullptr;
		}
		
		if(ReadCfgRet[0].GetType() != LUA_TTABLE)
		{
			Gs_UE_LOG(WorldContext.Get(), LogUnLua, Error, TEXT("[FGsLuaDataTableImp]Error Table'%s'[Key'%s'] not is a table"),
				*TableName, *Key);
			return nullptr;
		}

		//								  Value, ErrMsg
		lua_pushvalue(L, -1);	// -> Value, ErrMsg, ErrMsg

		LOG_LUA_STACK_IDX("[FGsLuaDataTableImp]ReadKeyAsTable pushvalue %s Top %d", 2)
		
		ReadCfgRet.Pop();			// -> Value (弹出了2个ErrMsg）

		LOG_LUA_STACK_IDX("[FGsLuaDataTableImp]ReadKeyAsTable ReadCfgRet.Pop() %s Top %d", 3)
		
		LuaTableOfKey = MakeShared<UnLua::FLuaTable>(L, lua_gettop(L));
		IsTableRefBySelf = true;
	}
	else
	{
		// LuaTable
		
		UnLua::FLuaValue TableValue = (*LuaTable)[TCHAR_TO_UTF8(*Key)];
		// -> LuaTable, LuaTable[Key]
		if(TableValue.GetType() != LUA_TTABLE)
		{
			Gs_UE_LOG(WorldContext.Get(), LogUnLua, Error, TEXT("[FGsLuaDataTableImp]Error Table'%s'[Key'%s'] not is a table"),
				*TableName, *Key);
			return nullptr;
		}

		LuaTableOfKey = MakeShared<UnLua::FLuaTable>(L, TableValue);
	}

	FString SubTableName = FString::Format(TEXT("{0}.{1}"), { TableName, Key });
	
	LOG_LUA_STACK_IDX("[FGsLuaDataTableImp]ReadKeyAsTable After Read %s Top %d", 4)
	
	return MakeShared<FGsLuaDataTableImp>(WorldContext, MoveTemp(SubTableName), LuaEnv, MoveTemp(LuaTableOfKey), IsTableRefBySelf);
}

template<typename TValue>
static TValue ReadKeyAsImp(const FGsLuaDataTableImp& Imp, const TSharedPtr<UnLua::FLuaTable>& LuaTable, const FString& Key, int LuaType)
{
	if(LuaTable)
	{
		UnLua::FLuaValue RetValue = (*LuaTable)[TCHAR_TO_UTF8(*Key)];
		if(RetValue.GetType() == LuaType)
		{
			return RetValue;
		}

		Gs_UE_LOG(Imp.GetWorldContext().Get(), LogUnLua, Error,
			TEXT("[FGsLuaDataTableImp]ReadKeyAsImp Error Table'%s'[Key'%s'] is %s, not %s"), *Imp.GetTableName(), *Key,
			*UGsLuaApiTools::LuaTypToString(RetValue.GetType()), *UGsLuaApiTools::LuaTypToString(LuaType));
	}

	return TValue();
}

template<typename TValue>
static void ReadKeyAsArrayImp(const FGsLuaDataTableImp& Imp, const TSharedPtr<UnLua::FLuaTable>& LuaTable,
	const FString& Key, int LuaType, TArray<TValue>& OutValues)
{
	if(LuaTable)
	{
		UnLua::FLuaValue SubTableValue = (*LuaTable)[TCHAR_TO_UTF8(*Key)];
		if(SubTableValue.GetType() != LUA_TTABLE)
		{
			Gs_UE_LOG(Imp.GetWorldContext().Get(), LogUnLua, Error,
			TEXT("[FGsLuaDataTableImp]Error Table'%s'[Key'%s'] is %s, not a table"), *Imp.GetTableName(), *Key,
			*UGsLuaApiTools::LuaTypToString(SubTableValue.GetType()));
			return;
		}

		UnLua::FLuaTable LuaArray(Imp.GetLuaEnv(), SubTableValue);
		const int32 TableLen = LuaArray.Length();
		for(int i = 1; i <= TableLen; ++i)
		{
			UnLua::FLuaValue Value = LuaArray[i];
			if(Value.GetType() == LuaType)
			{
				OutValues.Add(Value);
			}
			else
			{
				Gs_UE_LOG(Imp.GetWorldContext().Get(), LogUnLua, Error,
					TEXT("[FGsLuaDataTableImp]ReadKeyAsArrayImp Error Table'%s'[Key'%s'] at %d is %s, not %s"), *Imp.GetTableName(), *Key,
					i, *UGsLuaApiTools::LuaTypToString(Value.GetType()), *UGsLuaApiTools::LuaTypToString(LuaType));
			}
		}
	}
}

void FGsLuaDataTableImp::ReadKeyAs(const FString& Key, FString& OutValue) const
{
	OutValue = ReadKeyAsImp<FString>(*this, LuaTable, Key, LUA_TSTRING);
}

void FGsLuaDataTableImp::ReadKeyAs(const FString& Key, float& OutValue) const
{
	OutValue = ReadKeyAsImp<float>(*this, LuaTable, Key, LUA_TNUMBER);
}

void FGsLuaDataTableImp::ReadKeyAs(const FString& Key, int& OutValue) const
{
	OutValue = static_cast<int>(ReadKeyAsImp<float>(*this, LuaTable, Key, LUA_TNUMBER));
}

void FGsLuaDataTableImp::ReadKeyAs(const FString& Key, bool& OutValue) const
{
	OutValue = ReadKeyAsImp<bool>(*this, LuaTable, Key, LUA_TBOOLEAN);
}

void FGsLuaDataTableImp::ReadKeyAs(const FString& Key, TArray<FString>& OutValue) const
{
	ReadKeyAsArrayImp(*this, LuaTable, Key, LUA_TSTRING, OutValue);
}

void FGsLuaDataTableImp::ReadKeyAs(const FString& Key, TArray<int>& OutValue) const
{
	ReadKeyAsArrayImp(*this, LuaTable, Key, LUA_TNUMBER, OutValue);
}

void FGsLuaDataTableImp::ReadKeyAs(const FString& Key, TArray<float>& OutValue) const
{
	ReadKeyAsArrayImp(*this, LuaTable, Key, LUA_TNUMBER, OutValue);
}

void FGsLuaDataTableImp::ReadKeyAs(const FString& Key, TArray<bool>& OutValue) const
{
	ReadKeyAsArrayImp(*this, LuaTable, Key, LUA_TBOOLEAN, OutValue);
}

TSharedPtr<FGsLuaDataTableImp> FGsLuaDataTableImp::ReadIndexAsTable(int Index) const
{
	TSharedPtr<UnLua::FLuaTable> LuaTableOfIndex;

	lua_State* L = LuaEnv->GetMainState();
	bool IsTableRefBySelf = false;
	if(!LuaTable)
	{
		// ReadCfgRet = Value, ErrMsg
		UnLua::FLuaRetValues ReadCfgRet = UnLua::Call(L, "ReadCfg", TableName, Index);
		if(ReadCfgRet[1].GetType() == LUA_TSTRING)
		{
			// 有错误
			const FString ErrorMsg = ReadCfgRet[1];
			Gs_UE_LOG(WorldContext.Get(), LogUnLua, Error, TEXT("[FGsLuaDataTableImp]Error %s"), *ErrorMsg);
			return nullptr;
		}
		
		if(ReadCfgRet[0].GetType() != LUA_TTABLE)
		{
			Gs_UE_LOG(WorldContext.Get(), LogUnLua, Error, TEXT("[FGsLuaDataTableImp]Error Table'%s'[Index'%d'] not is a table"),
				*TableName, Index);
			return nullptr;
		}

		//								  Value, ErrMsg
		lua_pushvalue(L, -1);	// -> Value, ErrMsg, ErrMsg
		ReadCfgRet.Pop();			// -> Value (弹出了2个ErrMsg）
		
		LuaTableOfIndex = MakeShared<UnLua::FLuaTable>(L, lua_gettop(L));
		IsTableRefBySelf = true;
	}
	else
	{
		// Value
		UnLua::FLuaValue TableValue = (*LuaTable)[Index];
		if(TableValue.GetType() != LUA_TTABLE)
		{
			Gs_UE_LOG(WorldContext.Get(), LogUnLua, Error, TEXT("[FGsLuaDataTableImp]Error Table'%s'[Index'%d'] not is a table"),
				*TableName, Index);
			return nullptr;
		}

		LuaTableOfIndex = MakeShared<UnLua::FLuaTable>(L, TableValue);
	}
	
	FString SubTableName = FString::Format(TEXT("{0}.{1}"), { TableName, Index });
	return MakeShared<FGsLuaDataTableImp>(WorldContext, MoveTemp(SubTableName), LuaEnv, MoveTemp(LuaTableOfIndex), IsTableRefBySelf);
}

template<typename TValue>
static TValue ReadIndexAsImp(const FGsLuaDataTableImp& Imp, const TSharedPtr<UnLua::FLuaTable>& LuaTable, int Index, int LuaType)
{
	if(LuaTable)
	{
		UnLua::FLuaValue RetValue = (*LuaTable)[Index];
		if(RetValue.GetType() == LuaType)
		{
			return RetValue;
		}

		Gs_UE_LOG(Imp.GetWorldContext().Get(), LogUnLua, Error,
			TEXT("[FGsLuaDataTableImp]Error Table'%s'[Key'%d'] is %s, not %s"), *Imp.GetTableName(), Index,
			*UGsLuaApiTools::LuaTypToString(RetValue.GetType()), *UGsLuaApiTools::LuaTypToString(LuaType));
	}

	return TValue();
}

template<typename TValue>
static void ReadIndexAsArrayImp(const FGsLuaDataTableImp& Imp, const TSharedPtr<UnLua::FLuaTable>& LuaTable,
	int Index, int LuaType, TArray<TValue>& OutValues)
{
	if(LuaTable)
	{
		UnLua::FLuaValue SubTableValue = (*LuaTable)[Index];
		if(SubTableValue.GetType() != LUA_TTABLE)
		{
			Gs_UE_LOG(Imp.GetWorldContext().Get(), LogUnLua, Error,
			TEXT("[FGsLuaDataTableImp]Error Table'%s'[Index'%d'] is %s, not a table"), *Imp.GetTableName(), Index,
			*UGsLuaApiTools::LuaTypToString(SubTableValue.GetType()));
			return;
		}

		UnLua::FLuaTable LuaArray(Imp.GetLuaEnv(), SubTableValue);
		const int32 TableLen = LuaArray.Length();
		for(int i = 1; i <= TableLen; ++i)
		{
			UnLua::FLuaValue Value = LuaArray[i];
			if(Value.GetType() == LuaType)
			{
				OutValues.Add(Value);
			}
			else
			{
				Gs_UE_LOG(Imp.GetWorldContext().Get(), LogUnLua, Error,
					TEXT("[FGsLuaDataTableImp]ReadKeyAsArrayImp Error Table'%s'[Index'%d'] at %d is %s, not %s"),
					*Imp.GetTableName(), Index, i,
					*UGsLuaApiTools::LuaTypToString(Value.GetType()), *UGsLuaApiTools::LuaTypToString(LuaType));
			}
		}
	}
}

void FGsLuaDataTableImp::ReadIndexAs(int Index, FString& OutValue) const
{
	OutValue = ReadIndexAsImp<FString>(*this, LuaTable, Index, LUA_TSTRING);
}

void FGsLuaDataTableImp::ReadIndexAs(int Index, float& OutValue) const
{
	OutValue = ReadIndexAsImp<float>(*this, LuaTable, Index, LUA_TNUMBER);
}

void FGsLuaDataTableImp::ReadIndexAs(int Index, int& OutValue) const
{
	OutValue = static_cast<int>(ReadIndexAsImp<float>(*this, LuaTable, Index, LUA_TNUMBER));
}

void FGsLuaDataTableImp::ReadIndexAs(int Index, bool& OutValue) const
{
	OutValue = ReadIndexAsImp<bool>(*this, LuaTable, Index, LUA_TBOOLEAN);
}

void FGsLuaDataTableImp::ReadIndexAs(int Index, TArray<FString>& OutValue) const
{
	ReadIndexAsArrayImp(*this, LuaTable, Index, LUA_TSTRING, OutValue);
}

void FGsLuaDataTableImp::ReadIndexAs(int Index, TArray<int>& OutValue) const
{
	ReadIndexAsArrayImp(*this, LuaTable, Index, LUA_TNUMBER, OutValue);
}

void FGsLuaDataTableImp::ReadIndexAs(int Index, TArray<float>& OutValue) const
{
	ReadIndexAsArrayImp(*this, LuaTable, Index, LUA_TNUMBER, OutValue);
}

void FGsLuaDataTableImp::ReadIndexAs(int Index, TArray<bool>& OutValue) const
{
	ReadIndexAsArrayImp(*this, LuaTable, Index, LUA_TBOOLEAN, OutValue);
}

#undef LOG_LUA_STACK
