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

	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, Category = "Model")
	FWFCModelData ModelData;

	void PopulateModel(UWaveFunctionCollapseModel* model) const
	{
		model->Constraints = ModelData.Constraints;
		model->SpawnExclusion = ModelData.SpawnExclusion;
	}

	void Serialize(FArchive& Ar) override
	{
		Super::Serialize(Ar);
	}
};