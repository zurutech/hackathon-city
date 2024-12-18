// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "WFCModel.h"

#include "WFCBPLibrary.generated.h"

USTRUCT(BlueprintType)
struct HACKATON_CITY_API FWFCNeighbor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	EWFCAdjacency Adjacency = EWFCAdjacency::Front;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	int32 Step = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC", meta = (AllowedClasses = "StaticMesh, Blueprint"))
	FSoftObjectPath NeighborObject;
};

USTRUCT(BlueprintType)
struct HACKATON_CITY_API FWFCNeighborRule
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC", meta = (AllowedClasses = "StaticMesh, Blueprint"))
	FSoftObjectPath KeyObject;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	TArray<FWFCNeighbor> Neighbors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC", meta = (AllowedClasses = "StaticMesh, Blueprint"))
	TArray<FSoftObjectPath> SpawnObjects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC")
	FTransform SpawnRelativeTransform;

	/**
	* SpawnChance 1 = 100 % chance to spawn, SpawnChance 0 = 0 % chance to spawn
	*/ 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WFC", meta = (ClampMin = "0", ClampMax = "1"))
	float SpawnChance = 1.0f;
};

UCLASS()
class HACKATON_CITY_API UWFCBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/**
	* Calculates Shannon Entropy from an array of options and a given model
	* @param Options Array of options
	* @param WFCModel WFCModel that stores weights for options 
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	static float CalculateShannonEntropy(const TArray<FWFCOption>& Options, UWFCModel* WFCModel);

	/**
	* Convert 3D grid position to 2D array index
	* @param Position 3D grid position
	* @param Resolution Grid resolution
	* @return 2D array index
	*/
	UFUNCTION(BlueprintPure, Category = "WFC")
	static int32 PositionAsIndex(FIntVector Position, FIntVector Resolution);

	/**
	* Convert 2D array index to 3D grid position
	* @param Index 2D array index
	* @param Resolution
	* @return 3D grid position
	*/
	UFUNCTION(BlueprintPure, Category = "WFC")
	static FIntVector IndexAsPosition(int32 Index, FIntVector Resolution);

	/**
	* Builds the initial tile which adds every unique option in a model to its RemainingOptions array and calculates its entropy
	* @param WFCModel WFCModel
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC", meta = (ScriptMethod))
	static FWFCTile BuildInitialTile(UWFCModel* WFCModel);

	/**
	* Get adjacent indices of a given index and its adjacency
	* @param Index 2D array index
	* @param Resolution
	* @return Map of valid adjacent indices and its adjacency in relation to the given index
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	static TMap<int32, EWFCAdjacency> GetAdjacentIndices(int32 Index, FIntVector Resolution);

	/**
	* Get adjacent positions of a given position and its adjacency
	* @param Position 3D grid position
	* @param Resolution Grid resolution
	* @return Map of valid adjacent positions and its adjacency in relation to the given position
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	static TMap<FIntVector, EWFCAdjacency> GetAdjacentPositions(FIntVector Position, FIntVector Resolution);

	/**
	* Is the option contained in the given options array
	* @param Option Option to check for
	* @param Options Array of options
	*/
	UFUNCTION(BlueprintPure, Category = "WFC")
	static bool IsOptionContained(const FWFCOption& Option, const TArray<FWFCOption>& Options);

	/**
	* Get the opposite adjacency for a given adjacency.  For example GetOppositeAdjacency(Front) will return Back.
	* @param Adjacency Adjacency direction
	*/
	UFUNCTION(BlueprintPure, Category = "WFC")
	static EWFCAdjacency GetOppositeAdjacency(EWFCAdjacency Adjacency);
	
	/**
	* Get the next adjacency in clockwise direction on a Z-axis for a given adjacency.
	* For example GetNextZAxisClockwiseAdjacency(Front) will return Right.
	* Up or Down will return the original adjacency.
	* @param Adjacency Adjacency direction
	*/
	UFUNCTION(BlueprintPure, Category = "WFC")
	static EWFCAdjacency GetNextZAxisClockwiseAdjacency(EWFCAdjacency Adjacency);

	/**
	* Add an entry to an AdjacencyToOptionsMap
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	static void AddToAdjacencyToOptionsMap(UPARAM(ref) FWFCAdjacencyToOptionsMap& AdjacencyToOptionsMap, EWFCAdjacency Adjacency, FWFCOption Option);

	UFUNCTION(BlueprintPure, Category = "WFC")
	static bool IsSoftObjPathEqual(const FSoftObjectPath& SoftObjectPathA, const FSoftObjectPath& SoftObjectPathB);

	/**
	* Converts Rotator to Matrix and back to sanitize multiple representations of the same rotation to a common Rotator value
	*/
	UFUNCTION(BlueprintPure, Category = "WFC")
	static FRotator SanitizeRotator(FRotator Rotator);

	/**
	* Derive constraints from a given layout of actors and append them to a model
	* @param Actors array of actors to evaluate
	* @param WFCModel to add constraints to
	* @param TileSize distance between tiles
	* @param bIsBorderEmptyOption should the border be considered EmptyOption
	* @param bIsMinZFloorOption should the minimum Z actors be considered floor options (nothing can go below it)
	* @param bAutoDeriveZAxisRotationConstraints should it auto derive z-axis rotational variants
	* @param SpawnExclusionAssets assets to exclude when spawning
	* @param IgnoreRotationAssets assets to ignore rotation variants
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	static void DeriveModelFromActors(UPARAM(ref) const TArray<AActor*>& Actors, 
		UWFCModel* WFCModel, 
		float TileSize, 
		bool bIsBorderEmptyOption, 
		bool bIsMinZFloorOption, 
		bool bUseUniformWeightDistribution,
		bool bAutoDeriveZAxisRotationConstraints,
		const TArray<FSoftObjectPath>& SpawnExclusionAssets,
		const TArray<FSoftObjectPath>& IgnoreRotationAssets);

	/**
	* Get PositionToOptionsMap from a given actor that has ISM components.
	* This is useful when you want to derive neighboring tile data from a WFC-solved actor to be used for post processing.
	* This will only evaluate ISM components.
	* @param Actor Actor with ISM components
	* @param TileSize distance between tiles
	* @param PositionToOptionMap 
	*/
	UFUNCTION(BlueprintCallable, Category = "WFC")
	static bool GetPositionToOptionMapFromActor(AActor* Actor, float TileSize, UPARAM(ref) TMap<FIntVector, FWFCOption>& PositionToOptionMap);

	/**
	* Make FWFCOption of type: EmptyOption
	*/
	UFUNCTION(BlueprintPure, Category = "WFC")
	static FWFCOption MakeEmptyOption();

	/**
	* Make FWFCOption of type: BorderOption
	*/
	UFUNCTION(BlueprintPure, Category = "WFC")
	static FWFCOption MakeBorderOption();
	
	/**
	* Make FWFCOption of type: VoidOption
	*/
	UFUNCTION(BlueprintPure, Category = "WFC")
	static FWFCOption MakeVoidOption();
};
