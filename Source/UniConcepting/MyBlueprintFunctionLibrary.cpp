// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"

void UMyBlueprintFunctionLibrary::MinimizeWindow()
{
#if !WITH_EDITOR

	UGameEngine* gameEngine = Cast<UGameEngine>(GEngine);

	if (gameEngine)
	{
		TSharedPtr<SWindow> windowPtr = gameEngine->GameViewportWindow.Pin();
		SWindow* window = windowPtr.Get();

		if (window)
		{
			window->Minimize();
		}
	}

#endif // !WITH_EDITOR
}

void UMyBlueprintFunctionLibrary::MinimizeIfOutOfFocus(APlayerController* PlayerController)
{
	ULocalPlayer* LocPlayer = Cast<ULocalPlayer>(PlayerController->Player);
	if (!LocPlayer->ViewportClient->Viewport || !LocPlayer->ViewportClient->Viewport->IsForegroundWindow())
	{
		MinimizeWindow();
	}
}