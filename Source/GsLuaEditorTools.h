// *********************************************************************
// Copyright 1998-2026 Tencent Games, Inc. All Rights Reserved.
// 作    者：gershonchen
// 创建日期：2024年08月06日
// 功能描述：用于Lua相关的Editor工具类
// *********************************************************************

#pragma once

#if WITH_EDITOR

class GsFRAMEWORK_API FGsLuaEditorTools
{
public:
	// 把路径转化成lua的路径类型 a.b.c 形式
	static void NormaliseLuaPath(FString& InOutLuaPath);
};

#endif