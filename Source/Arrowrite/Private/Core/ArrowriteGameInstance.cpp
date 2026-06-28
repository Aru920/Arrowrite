// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/ArrowriteGameInstance.h"

namespace
{
	constexpr int32 MaxPlayerNameLength = 20;
}

void UArrowriteGameInstance::SetDesiredPlayerName(const FString& PlayerName)
{
	DesiredPlayerName = MakeSafePlayerName(PlayerName);
}

FString UArrowriteGameInstance::MakeSafePlayerName(const FString& PlayerName)
{
	const FString TrimmedPlayerName = PlayerName.TrimStartAndEnd();
	FString SafePlayerName;
	SafePlayerName.Reserve(TrimmedPlayerName.Len());

	bool bLastWasSeparator = false;
	for (const TCHAR Character : TrimmedPlayerName)
	{
		if (FChar::IsAlnum(Character))
		{
			SafePlayerName.AppendChar(Character);
			bLastWasSeparator = false;
			continue;
		}

		if ((FChar::IsWhitespace(Character) || Character == TEXT('_') || Character == TEXT('-'))
			&& !bLastWasSeparator
			&& !SafePlayerName.IsEmpty())
		{
			SafePlayerName.AppendChar(TEXT('_'));
			bLastWasSeparator = true;
		}
	}

	while (SafePlayerName.EndsWith(TEXT("_")))
	{
		SafePlayerName.LeftChopInline(1);
	}

	if (SafePlayerName.IsEmpty())
	{
		return TEXT("Player");
	}

	return SafePlayerName.Left(MaxPlayerNameLength);
}
