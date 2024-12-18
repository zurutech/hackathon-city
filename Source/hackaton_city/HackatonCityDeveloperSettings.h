#pragma once

#include "CoreMinimal.h"
#include "WFCSubsystem.h"
#include "Delegates/Delegate.h"
#include "Internationalization/Text.h"
#include "Templates/SharedPointer.h"
#include "UObject/NameTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "Engine/DeveloperSettings.h"

#include "HackatonCityDeveloperSettings.generated.h"

USTRUCT(BlueprintType)
struct FWFCModelData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveFunctionCollapse")
	float TileSize;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveFunctionCollapse")
	TMap<FWaveFunctionCollapseOption, FWaveFunctionCollapseAdjacencyToOptionsMap> Constraints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WaveFunctionCollapse", meta = (AllowedClasses = "StaticMesh, Blueprint"))
	TArray<FSoftObjectPath> SpawnExclusion;
};

UCLASS(config=Game, defaultconfig)
class UHackatonCityDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UHackatonCityDeveloperSettings(){}

	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, Category = "Speed")
	float Speed = 100.0f;

	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, Category = "Speed")
	FIntVector WFCResolution = FIntVector(5, 5, 1);
	
	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, Category = "Model")
	FSoftObjectPath BaseModel;

	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, Category = "Model")
	FWFCModelData ModelData;
	
	void PopulateModel(UWaveFunctionCollapseModel* model) const
	{
		model->TileSize = ModelData.TileSize;
		model->Constraints = ModelData.Constraints;
		model->SpawnExclusion = ModelData.SpawnExclusion;
	}
	
};