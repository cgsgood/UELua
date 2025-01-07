// *********************************************************************
// Copyright 1998-2026 Tencent Games, Inc. All Rights Reserved.
// 作    者：gershonchen
// 创建日期：2024年08月06日
// 功能描述：用于Lua相关的Editor工具类
// *********************************************************************

#include "GsLuaEditorTools.h"

#if WITH_EDITOR

void FGsLuaEditorTools::NormaliseLuaPath(FString& InOutLuaPath)
{
	// H:\GsDemoClient\MainClient\GsGame\Content\Script\GsClient\UI\ExamplePanel\ExamplePanelView.lua
	// GsClient/UI/ExamplePanel/ExamplePanelView.lua
	int Index;
	if(InOutLuaPath.FindChar('\\', Index) || InOutLuaPath.FindChar('/', Index)
		|| InOutLuaPath.Contains(TEXT(".lua")))
	{
		// H:\GsDemoClient\MainClient\GsGame\Content\Script\GsClient\UI\ExamplePanel\ExamplePanelView.lua
		// -> H:/GsDemoClient/MainClient/GsGame/Content/Script/GsClient/UI/ExamplePanel/ExamplePanelView.lua
		FString TmpLuaPath = InOutLuaPath.Replace(TEXT("\\"), TEXT("/"));
		Index = TmpLuaPath.Find(TEXT("Script/"));
		if (Index != INDEX_NONE)
		{
			// H:/GsDemoClient/MainClient/GsGame/Content/Script/GsClient/UI/ExamplePanel/ExamplePanelView.lua
			// -> GsClient/UI/ExamplePanel/ExamplePanelView.lua
			TmpLuaPath = TmpLuaPath.Mid(Index + 7);
		}

		if(TmpLuaPath.EndsWith(TEXT(".lua")))
		{
			// GsClient/UI/ExamplePanel/ExamplePanelView.lua
			// -> GsClient/UI/ExamplePanel/ExamplePanelView
			TmpLuaPath = TmpLuaPath.Left(TmpLuaPath.Len() - 4);
		}

		// GsClient/UI/ExamplePanel/ExamplePanelView
		// -> GsClient.UI.ExamplePanel.ExamplePanelView
		InOutLuaPath = TmpLuaPath.Replace(TEXT("/"), TEXT("."));
	}
}

#endif	// WITH_EDITOR


