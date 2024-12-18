// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "Engine/StaticMesh.h"
#include "WFCModel.generated.h"

UENUM(BlueprintType)
enum class EWFCAdjacency : uint8
{
	Front	UMETA(DisplayName = "X+ Front"),
	Back	UMETA(DisplayName = "X- Back"),
	Right	UMETA(DisplayName = "Y+ Right"),
	Left	UMETA(DisplayName = "Y- Left"),
	Up		UMETA(DisplayName = "Z+ Up"),
	Down	UMETA(DisplayName = "Z- Down")
};

/**
* Base Option Struct which holds an object, its orientation and scale
*/
USTRUCT(BlueprintType)
struct HACKATON_CITY_API FWFCOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC", meta = (AllowedClasses = "StaticMesh, Blueprint"))
	FSoftObjectPath BaseObject;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	FRotator BaseRotator = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	FVector BaseScale3D = FVector::OneVector;

	FWFCOption() = default;

	FWFCOption(FString Object, FRotator Rotator, FVector Scale3d)
	{
		BaseObject = FSoftObjectPath(Object);
		BaseRotator = Rotator;
		BaseScale3D = Scale3d;
	}

	FWFCOption(FString Object)
	{
		BaseObject = FSoftObjectPath(Object);
	}

	static const FWFCOption EmptyOption;
	static const FWFCOption BorderOption;
	static const FWFCOption VoidOption;

	friend uint32 GetTypeHash(FWFCOption Output)
	{
		uint32 OutputHash;
		OutputHash = HashCombine(GetTypeHash(Output.BaseRotator.Vector()), GetTypeHash(Output.BaseScale3D));
		OutputHash = HashCombine(OutputHash, GetTypeHash(Output.BaseObject));
		return OutputHash;
	}

	bool operator==(const FWFCOption& Rhs) const
	{
		return BaseObject == Rhs.BaseObject && BaseRotator.Equals(Rhs.BaseRotator) && BaseScale3D.Equals(Rhs.BaseScale3D);
	}

	bool operator!=(const FWFCOption& Rhs) const
	{
		return BaseObject != Rhs.BaseObject || !BaseRotator.Equals(Rhs.BaseRotator) || !BaseScale3D.Equals(Rhs.BaseScale3D);
	}

	bool IsBaseObject(FString ObjectPath)
	{
		return (BaseObject == FSoftObjectPath(ObjectPath));
	}
};

/**
* Container struct for array of Options
*/
USTRUCT(BlueprintType)
struct HACKATON_CITY_API FWFCOptions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	TArray<FWFCOption> Options;
};

/**
* Container struct for AdjacencyToOptionsMap
* Stores the weight and contribution of an option
*/
USTRUCT(BlueprintType)
struct HACKATON_CITY_API FWFCAdjacencyToOptionsMap
{
	GENERATED_BODY()

	/**
	* The amount of times an option is present when deriving a model.
	* This value is used to calculate its weight.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC", meta = (ClampMin = "0"))
	int32 Contribution = 1;

	/**
	* The weight of an option calculated by dividing this Contribution by the sum of all contributions of all options.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC", meta = (ClampMin = "0", ClampMax = "1"))
	float Weight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	TMap<EWFCAdjacency, FWFCOptions> AdjacencyToOptionsMap;
};

/**
* A Model of WFC constraints.
* This data asset should contain all necessary data to allow for a WFC solve of an arbitrary grid size.
*/
UCLASS(Blueprintable, BlueprintType)
class HACKATON_CITY_API UWFCModel : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	* Grid Tile Size in cm^3
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	float TileSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	TMap<FWFCOption, FWFCAdjacencyToOptionsMap> Constraints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC", meta = (AllowedClasses = "StaticMesh, Blueprint"))
	TArray<FSoftObjectPath> SpawnExclusion;

	/**
	* Create a constraint
	* @param KeyOption Key option
	* @param Adjacency Adjacency from KeyOption to AdjacentOption
	* @param AdjacentOption Adjacent option
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	void AddConstraint(const FWFCOption& KeyOption, EWFCAdjacency Adjacency, const FWFCOption& AdjacentOption);

	/**
	* Get all options for a given key option in a given adjacency
	* @param KeyOption Key option
	* @param Adjacency Adjacency from KeyOption to AdjacentOption
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	FWFCOptions GetOptions(const FWFCOption& KeyOption, EWFCAdjacency Adjacency) const;

	/**
	* Set the weights of key objects based on their contribution values
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	void SetWeightsFromContributions();

	/**
	* Set the weights of key objects to a given value
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	void SetAllWeights(float Weight);

	/**
	* Set the contribution values of key objects to a given value
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	void SetAllContributions(int32 Contribution);

	/**
	* Set the contribution value of a key object to a given value
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	void SetOptionContribution(const FWFCOption& Option, int32 Contribution);

	/**
	* Get the weight value of an option
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	float GetOptionWeight(const FWFCOption& Option) const;

	/**
	* Get the contribution value of an option
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	int32 GetOptionContribution(const FWFCOption& Option) const;

	/**
	* Get the total count of constraints in this model
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	int32 GetConstraintCount() const;

	/**
	* Swap meshes in the model with other meshes based on a map.  
	* This is useful when working with template meshes that need to be swapped.
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	void SwapMeshes(TMap<UStaticMesh*, UStaticMesh*> SourceToTargetMeshMap);
};

/**
* Base Tile Struct which holds an array of remaining Options and its Shannon Entropy value
*/
USTRUCT(BlueprintType)
struct HACKATON_CITY_API FWFCTile
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	float ShannonEntropy;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	TArray<FWFCOption> RemainingOptions;

	FWFCTile() = default;

	FWFCTile(const TArray<FWFCOption>& Options, float Entropy)
	{
		RemainingOptions = Options;
		ShannonEntropy = Entropy;
	}

	// constructor with only one option
	FWFCTile(const FWFCOption& Option, float Entropy)
	{
		RemainingOptions.Add(Option);
		ShannonEntropy = Entropy;
	}
};

/**
* A helper struct used for queuing during Observation and Propagation phases
*/
USTRUCT(BlueprintType)
struct HACKATON_CITY_API FWFCQueueElement
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	int32 CenterObjectIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	EWFCAdjacency Adjacency = EWFCAdjacency::Front;

	FWFCQueueElement() = default;

	FWFCQueueElement(int32 CenterObjectIndexInput, EWFCAdjacency AdjacencyInput)
	{
		CenterObjectIndex = CenterObjectIndexInput;
		Adjacency = AdjacencyInput;
	}
};