// Copyright Epic Games, Inc. All Rights Reserved.

#include "WFCBPLibrary.h"
#include "Engine/StaticMesh.h"
#include "WFCSubsystem.h"
#include "Engine/StaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"

UWFCBPLibrary::UWFCBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

float UWFCBPLibrary::CalculateShannonEntropy(const TArray<FWFCOption>& Options, UWFCModel* WFCModel)
{
	if (Options.IsEmpty())
	{
		UE_LOG(LogTemp, Display, TEXT("Cannot calculate shannon entropy because the options are empty."));
		return -1;
	}

	float SumWeights = 0;
	float SumWeightXLogWeight = 0;
	for (const FWFCOption& Option : Options)
	{
		if (WFCModel->Constraints.Contains(Option))
		{
			const float& Weight = WFCModel->Constraints.FindRef(Option).Weight;
			SumWeights += Weight;
			SumWeightXLogWeight += (Weight * log(Weight));
		}
	}

	if (SumWeights == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Cannot calculate shannon entropy because the sum of weights equals zero."));
		return -1;
	}

	return log(SumWeights) - (SumWeightXLogWeight / SumWeights);
}

int32 UWFCBPLibrary::PositionAsIndex(FIntVector Position, FIntVector Resolution)
{
	return Position.X + (Position.Y * Resolution.X) + (Position.Z * Resolution.X * Resolution.Y);
}

FIntVector UWFCBPLibrary::IndexAsPosition(int32 Index, FIntVector Resolution)
{
	FIntVector Position;
	Position.Z = floor(Index / (Resolution.X * Resolution.Y));
	Position.Y = floor((Index - (Position.Z * Resolution.X * Resolution.Y)) / Resolution.X);
	Position.X = Index - (Position.Y * Resolution.X) - (Position.Z * Resolution.X * Resolution.Y);
	return Position;
}

FWFCTile UWFCBPLibrary::BuildInitialTile(UWFCModel* WFCModel)
{
	FWFCTile InitialTile;
	for (TPair<FWFCOption, FWFCAdjacencyToOptionsMap>& Constraint : WFCModel->Constraints)
	{
		InitialTile.RemainingOptions.AddUnique(Constraint.Key);
	}
	InitialTile.ShannonEntropy = UWFCBPLibrary::CalculateShannonEntropy(InitialTile.RemainingOptions, WFCModel);
	return InitialTile;
}

TMap<int32, EWFCAdjacency> UWFCBPLibrary::GetAdjacentIndices(int32 Index, FIntVector Resolution)
{
	TMap<int32, EWFCAdjacency> AdjacentIndices;
	FIntVector Position = IndexAsPosition(Index, Resolution);
	if (Position.X + 1 < Resolution.X) 
	{ 
		AdjacentIndices.Add(PositionAsIndex(Position + FIntVector(1, 0, 0), Resolution),EWFCAdjacency::Front);
	}
	if (Position.X - 1 >= 0) 
	{ 
		AdjacentIndices.Add(PositionAsIndex(Position + FIntVector(-1, 0, 0), Resolution), EWFCAdjacency::Back); 
	}
	if (Position.Y + 1 < Resolution.Y) 
	{ 
		AdjacentIndices.Add(PositionAsIndex(Position + FIntVector(0, 1, 0), Resolution), EWFCAdjacency::Right); 
	}
	if (Position.Y - 1 >= 0) 
	{ 
		AdjacentIndices.Add(PositionAsIndex(Position + FIntVector(0, -1, 0), Resolution), EWFCAdjacency::Left); 
	}
	if (Position.Z + 1 < Resolution.Z) 
	{ 
		AdjacentIndices.Add(PositionAsIndex(Position + FIntVector(0, 0, 1), Resolution), EWFCAdjacency::Up); 
	}
	if (Position.Z - 1 >= 0) 
	{ 
		AdjacentIndices.Add(PositionAsIndex(Position + FIntVector(0, 0, -1), Resolution), EWFCAdjacency::Down); 
	}

	return AdjacentIndices;
}

TMap<FIntVector, EWFCAdjacency> UWFCBPLibrary::GetAdjacentPositions(FIntVector Position, FIntVector Resolution)
{
	TMap<FIntVector, EWFCAdjacency> AdjacentPositions;
	if (Position.X + 1 < Resolution.X) 
	{ 
		AdjacentPositions.Add(Position + FIntVector(1, 0, 0), EWFCAdjacency::Front); 
	}
	if (Position.X - 1 >= 0) 
	{ 
		AdjacentPositions.Add(Position + FIntVector(-1, 0, 0), EWFCAdjacency::Back); 
	}
	if (Position.Y + 1 < Resolution.Y) 
	{ 
		AdjacentPositions.Add(Position + FIntVector(0, 1, 0), EWFCAdjacency::Right); 
	}
	if (Position.Y - 1 >= 0) 
	{ 
		AdjacentPositions.Add(Position + FIntVector(0, -1, 0), EWFCAdjacency::Left); 
	}
	if (Position.Z + 1 < Resolution.Z) 
	{ 
		AdjacentPositions.Add(Position + FIntVector(0, 0, 1), EWFCAdjacency::Up); 
	}
	if (Position.Z - 1 >= 0) 
	{ 
		AdjacentPositions.Add(Position + FIntVector(0, 0, -1), EWFCAdjacency::Down); 
	}

	return AdjacentPositions;
}

bool UWFCBPLibrary::IsOptionContained(const FWFCOption& Option, const TArray<FWFCOption>& Options)
{
	for (const FWFCOption& CheckAgainst : Options)
	{
		if (Option == CheckAgainst)
		{
			return true;
		}
	}
	return false;
}

EWFCAdjacency UWFCBPLibrary::GetOppositeAdjacency(EWFCAdjacency Adjacency)
{
	switch (Adjacency)
	{
		case EWFCAdjacency::Front:
			return EWFCAdjacency::Back;
		case EWFCAdjacency::Back:
			return EWFCAdjacency::Front;
		case EWFCAdjacency::Right:
			return EWFCAdjacency::Left;
		case EWFCAdjacency::Left:
			return EWFCAdjacency::Right;
		case EWFCAdjacency::Up:
			return EWFCAdjacency::Down;
		case EWFCAdjacency::Down:
			return EWFCAdjacency::Up;
		default:
			return Adjacency;
	}
}

EWFCAdjacency UWFCBPLibrary::GetNextZAxisClockwiseAdjacency(EWFCAdjacency Adjacency)
{
	switch (Adjacency)
	{
	case EWFCAdjacency::Front:
		return EWFCAdjacency::Right;
	case EWFCAdjacency::Right:
		return EWFCAdjacency::Back;
	case EWFCAdjacency::Back:
		return EWFCAdjacency::Left;
	case EWFCAdjacency::Left:
		return EWFCAdjacency::Front;
	case EWFCAdjacency::Up:
		return EWFCAdjacency::Up;
	case EWFCAdjacency::Down:
		return EWFCAdjacency::Down;
	default:
		return Adjacency;
	}
}

void UWFCBPLibrary::AddToAdjacencyToOptionsMap(UPARAM(ref) FWFCAdjacencyToOptionsMap& AdjacencyToOptionsMap, EWFCAdjacency OptionAdjacency, FWFCOption OptionObject)
{
	if (FWFCOptions* Options = AdjacencyToOptionsMap.AdjacencyToOptionsMap.Find(OptionAdjacency))
	{
		// If the Options array does not contain the Option, create it
		if (!Options->Options.Contains(OptionObject))
		{
			Options->Options.Add(OptionObject);
		}
	}
	else // Create the Option and add it to the AdjacencyToOptionsMap
	{
		FWFCOptions NewOptions;
		NewOptions.Options.Add(OptionObject);
		AdjacencyToOptionsMap.AdjacencyToOptionsMap.Add(OptionAdjacency, NewOptions);
	}
}

bool UWFCBPLibrary::IsSoftObjPathEqual(const FSoftObjectPath& SoftObjectPathA, const FSoftObjectPath& SoftObjectPathB)
{
	return SoftObjectPathA == SoftObjectPathB;
}

FRotator UWFCBPLibrary::SanitizeRotator(FRotator Rotator)
{
	FRotator OutputRotator;
	OutputRotator = Rotator;

	// Round to orthogonal values
	OutputRotator.Roll = FMath::RoundHalfToEven(OutputRotator.Roll / 90) * 90;
	OutputRotator.Pitch = FMath::RoundHalfToEven(OutputRotator.Pitch / 90) * 90;
	OutputRotator.Yaw = FMath::RoundHalfToEven(OutputRotator.Yaw / 90) * 90;
	OutputRotator.Normalize();

	// Convert from Rotator to Quaternion to Matrix and back To Rotator to ensure single representation of rotation value
	FTransform MyTransform;
	MyTransform.SetRotation(FQuat(OutputRotator));
	FMatrix MyMatrix = MyTransform.ToMatrixNoScale();
	OutputRotator= MyMatrix.Rotator();

	// Ensure that -180 values are adjusted to 180
	OutputRotator.Normalize();
	OutputRotator.Roll = FMath::RoundHalfToEven(OutputRotator.Roll);
	if (OutputRotator.Roll == -180) 
	{ 
		OutputRotator.Roll = 180; 
	}
	OutputRotator.Pitch = FMath::RoundHalfToEven(OutputRotator.Pitch);
	if (OutputRotator.Pitch == -180) 
	{ 
		OutputRotator.Pitch = 180; 
	}
	OutputRotator.Yaw = FMath::RoundHalfToEven(OutputRotator.Yaw);
	if (OutputRotator.Yaw == -180) 
	{ 
		OutputRotator.Yaw = 180; 
	}

	return OutputRotator;
}

void UWFCBPLibrary::DeriveModelFromActors(UPARAM(ref) const TArray<AActor*>& InputActors, 
	UWFCModel* WFCModel, 
	float TileSize, 
	bool bIsBorderEmptyOption, 
	bool bIsMinZFloorOption,
	bool bUseUniformWeightDistribution,
	bool bAutoDeriveZAxisRotationConstraints,
	const TArray<FSoftObjectPath>& SpawnExclusionAssets,
	const TArray<FSoftObjectPath>& IgnoreRotationAssets)
{
	// Check if there are valid actors
	if (InputActors.IsEmpty())
	{
		return;
	}

	// Check if Model is valid
	if (!WFCModel)
	{
		return;
	}

	// Initialize Variables
	TMap<FIntVector, FWFCOption> PositionToOptionWithBorderMap;
	FIntVector ResolutionWithBorder;

	// Set TileSize
	WFCModel->Modify();
	WFCModel->TileSize = TileSize;

	// Create all Empty Option adjacency constraints
	WFCModel->AddConstraint(FWFCOption::EmptyOption, EWFCAdjacency::Front, FWFCOption::EmptyOption);
	WFCModel->AddConstraint(FWFCOption::EmptyOption, EWFCAdjacency::Back, FWFCOption::EmptyOption);
	WFCModel->AddConstraint(FWFCOption::EmptyOption, EWFCAdjacency::Left, FWFCOption::EmptyOption);
	WFCModel->AddConstraint(FWFCOption::EmptyOption, EWFCAdjacency::Right, FWFCOption::EmptyOption);
	WFCModel->AddConstraint(FWFCOption::EmptyOption, EWFCAdjacency::Up, FWFCOption::EmptyOption);
	WFCModel->AddConstraint(FWFCOption::EmptyOption, EWFCAdjacency::Down, FWFCOption::EmptyOption);
	
	// Derive Grid from Actors
	FIntVector MinPosition = FIntVector::ZeroValue;
	FIntVector MaxPosition = FIntVector::ZeroValue;
	FVector TmpOrigin = InputActors[0]->GetActorLocation(); // Set TmpOrigin based on first actor in the array
	TMap<FIntVector, FWFCOption> TmpPositionToOptionMap;
	for (int32 InputActorIndex = 0; InputActorIndex < InputActors.Num(); InputActorIndex++)
	{
		AStaticMeshActor* InputStaticMeshActor = Cast<AStaticMeshActor>(InputActors[InputActorIndex]);
		if (InputStaticMeshActor)
		{
			UStaticMesh* InputStaticMesh = InputStaticMeshActor->GetStaticMeshComponent()->GetStaticMesh();
			if (!InputStaticMesh)
			{
				UE_LOG(LogTemp, Display, TEXT("%s contains invalid StaticMesh, skipping."), *InputStaticMeshActor->GetFName().ToString());
				continue;
			}

			// Find CurrentGridPosition
			FVector CurrentGridPositionFloat = (InputStaticMeshActor->GetActorLocation() - TmpOrigin) / TileSize;
			FIntVector CurrentGridPosition = FIntVector(FMath::RoundHalfFromZero(CurrentGridPositionFloat.X), FMath::RoundHalfFromZero(CurrentGridPositionFloat.Y), FMath::RoundHalfFromZero(CurrentGridPositionFloat.Z));
			if (TmpPositionToOptionMap.Contains(CurrentGridPosition))
			{
				UE_LOG(LogTemp, Display, TEXT("%s is in an overlapping position, skipping."), *InputStaticMeshActor->GetFName().ToString());
				continue;
			}

			// Find min/max positions
			MinPosition.X = FMath::Min(MinPosition.X, CurrentGridPosition.X);
			MinPosition.Y = FMath::Min(MinPosition.Y, CurrentGridPosition.Y);
			MinPosition.Z = FMath::Min(MinPosition.Z, CurrentGridPosition.Z);
			MaxPosition.X = FMath::Max(MaxPosition.X, CurrentGridPosition.X);
			MaxPosition.Y = FMath::Max(MaxPosition.Y, CurrentGridPosition.Y);
			MaxPosition.Z = FMath::Max(MaxPosition.Z, CurrentGridPosition.Z);

			// Add to Temporary Position to Option Map
			FRotator CurrentRotator = FRotator::ZeroRotator;
			if (!IgnoreRotationAssets.Contains(FSoftObjectPath(InputStaticMesh->GetPathName())))
			{
				CurrentRotator = SanitizeRotator(InputStaticMeshActor->GetActorRotation());
			}
			FWFCOption CurrentOption(InputStaticMesh->GetPathName(), CurrentRotator, InputStaticMeshActor->GetActorScale3D());
			TmpPositionToOptionMap.Add(CurrentGridPosition, CurrentOption);
		}
	}

	// Re-order grid from 0,0,0 and add border
	for (TPair<FIntVector, FWFCOption>& TmpPositionToOption : TmpPositionToOptionMap)
	{
		PositionToOptionWithBorderMap.Add(TmpPositionToOption.Key - MinPosition + FIntVector(1), TmpPositionToOption.Value);
	}
	TmpPositionToOptionMap.Empty();

	// Set Resolution with Border
	ResolutionWithBorder = MaxPosition - MinPosition + FIntVector(3);
	
	// Derive Constraints
	for (TPair<FIntVector, FWFCOption>& PositionToOptionWithBorder : PositionToOptionWithBorderMap)
	{
		TMap<FIntVector, EWFCAdjacency> PositionToAdjacenciesMap = GetAdjacentPositions(PositionToOptionWithBorder.Key, ResolutionWithBorder);
		for (TPair<FIntVector, EWFCAdjacency>& PositionToAdjacencies : PositionToAdjacenciesMap)
		{
			FIntVector PositionToCheck = PositionToAdjacencies.Key;

			// if Adjacent Option exists, add constraint to model
			if (PositionToOptionWithBorderMap.Contains(PositionToCheck))
			{
				WFCModel->AddConstraint(PositionToOptionWithBorder.Value, PositionToAdjacencies.Value, PositionToOptionWithBorderMap.FindRef(PositionToCheck));
			}
			// if MinZ and bIsMinZFloorOption, add Border Option constraint
			else if (bIsMinZFloorOption && PositionToCheck.Z == 0)
			{
				WFCModel->AddConstraint(FWFCOption::BorderOption, GetOppositeAdjacency(PositionToAdjacencies.Value), PositionToOptionWithBorder.Value);
			}
			// if ExteriorBorder and bIsBorderEmptyOption, add Empty Option and Inverse constraints
			else if (PositionToCheck.X == 0
				|| PositionToCheck.Y == 0
				|| PositionToCheck.Z == 0
				|| PositionToCheck.X == ResolutionWithBorder.X - 1
				|| PositionToCheck.Y == ResolutionWithBorder.Y - 1
				|| PositionToCheck.Z == ResolutionWithBorder.Z - 1)
			{
				if (bIsBorderEmptyOption)
				{
					WFCModel->AddConstraint(PositionToOptionWithBorder.Value, PositionToAdjacencies.Value, FWFCOption::EmptyOption);
					WFCModel->AddConstraint(FWFCOption::EmptyOption, GetOppositeAdjacency(PositionToAdjacencies.Value), PositionToOptionWithBorder.Value);
				}
			}
			// otherwise it is an empty space, add Empty Option and Inverse constraints
			else
			{
				WFCModel->AddConstraint(PositionToOptionWithBorder.Value, PositionToAdjacencies.Value, FWFCOption::EmptyOption);
				WFCModel->AddConstraint(FWFCOption::EmptyOption, GetOppositeAdjacency(PositionToAdjacencies.Value), PositionToOptionWithBorder.Value);
			}
		}
	}
	PositionToOptionWithBorderMap.Empty();

	// Auto Derive ZAxis Rotation Constraints
	if (bAutoDeriveZAxisRotationConstraints)
	{
		TArray<FWFCOption> NewKeyOptions;
		TArray<EWFCAdjacency> NewAdjacencies;
		TArray<FWFCOption> NewAdjacentOptions;

		for (const TPair<FWFCOption, FWFCAdjacencyToOptionsMap>& Constraint : WFCModel->Constraints)
		{
			for (const TPair<EWFCAdjacency, FWFCOptions>& AdjacencyToOptionsPair : Constraint.Value.AdjacencyToOptionsMap)
			{
				for (const FWFCOption& AdjacentOption : AdjacencyToOptionsPair.Value.Options)
				{
					FWFCOption NewKeyOption = Constraint.Key;
					FWFCOption NewAdjacentOption = AdjacentOption;
					EWFCAdjacency NewAdjacency = AdjacencyToOptionsPair.Key;

					int32 KeyOptionRotationMultiplier = 0;
					if (!(IgnoreRotationAssets.Contains(NewKeyOption.BaseObject)
						|| NewKeyOption == FWFCOption::EmptyOption
						|| NewKeyOption == FWFCOption::BorderOption
						|| NewKeyOption == FWFCOption::VoidOption))
					{
						KeyOptionRotationMultiplier = 1;
					}

					int32 AdjacentOptionRotationMultiplier = 0;
					if (!(IgnoreRotationAssets.Contains(NewAdjacentOption.BaseObject)
						|| NewAdjacentOption == FWFCOption::EmptyOption
						|| NewAdjacentOption == FWFCOption::BorderOption
						|| NewAdjacentOption == FWFCOption::VoidOption))
					{
						AdjacentOptionRotationMultiplier = 1;
					}

					for (int32 RotationIncrement = 1; RotationIncrement <= 3; RotationIncrement++)
					{
						NewKeyOption.BaseRotator.Yaw += (90.0f * KeyOptionRotationMultiplier);
						NewKeyOption.BaseRotator = SanitizeRotator(NewKeyOption.BaseRotator);
						NewAdjacentOption.BaseRotator.Yaw += (90.0f * AdjacentOptionRotationMultiplier);
						NewAdjacentOption.BaseRotator = SanitizeRotator(NewAdjacentOption.BaseRotator);
						NewAdjacency = GetNextZAxisClockwiseAdjacency(NewAdjacency);

						// Store new constraints
						NewKeyOptions.Add(NewKeyOption);
						NewAdjacencies.Add(NewAdjacency);
						NewAdjacentOptions.Add(NewAdjacentOption);
					}
				}
			}
		}

		// Add stored new constraints to the model
		for (int32 Index = 0; Index < NewKeyOptions.Num(); Index++)
		{
			WFCModel->AddConstraint(NewKeyOptions[Index], NewAdjacencies[Index], NewAdjacentOptions[Index]);
		}
	}
	
	// If Empty->OptionA constraint and Empty<-OptionB exist, store OptionA->OptionB constraint
	TArray<FWFCOption> EmptyVoidAdjacentKeyOptions;
	TArray<EWFCAdjacency> EmptyVoidAdjacentAdjacencies;
	TArray<FWFCOption> EmptyVoidAdjacentAdjacentOptions;
	const FWFCAdjacencyToOptionsMap& EmptyOptionConstraints = WFCModel->Constraints.FindRef(FWFCOption::EmptyOption);
	for (const TPair<EWFCAdjacency, FWFCOptions>& AdjacencyToOptionsPair : EmptyOptionConstraints.AdjacencyToOptionsMap)
	{
		for (const FWFCOption& EmptyAdjacentOption : AdjacencyToOptionsPair.Value.Options)
		{
			if (EmptyAdjacentOption == FWFCOption::EmptyOption)
			{
				continue;
			}

			if (const FWFCOptions* OppositeEmptyAdjacenctOptions = EmptyOptionConstraints.AdjacencyToOptionsMap.Find(GetOppositeAdjacency(AdjacencyToOptionsPair.Key)))
			{
				for (const FWFCOption& OppositeEmptyAdjacentOption : OppositeEmptyAdjacenctOptions->Options)
				{
					if (OppositeEmptyAdjacentOption == FWFCOption::EmptyOption)
					{
						continue;
					}

					EmptyVoidAdjacentKeyOptions.Add(EmptyAdjacentOption);
					EmptyVoidAdjacentAdjacencies.Add(GetOppositeAdjacency(AdjacencyToOptionsPair.Key));
					EmptyVoidAdjacentAdjacentOptions.Add(OppositeEmptyAdjacentOption);
				}
			}
		}
	}

	// If Void->OptionA constraint and Void<-OptionB exist, store OptionA->OptionB constraint
	const FWFCAdjacencyToOptionsMap& VoidOptionConstraints = WFCModel->Constraints.FindRef(FWFCOption::VoidOption);
	for (const TPair<EWFCAdjacency, FWFCOptions>& AdjacencyToOptionsPair : VoidOptionConstraints.AdjacencyToOptionsMap)
	{
		for (const FWFCOption& VoidAdjacentOption : AdjacencyToOptionsPair.Value.Options)
		{
			if (VoidAdjacentOption == FWFCOption::VoidOption)
			{
				continue;
			}

			if (const FWFCOptions* OppositeVoidAdjacenctOptions = VoidOptionConstraints.AdjacencyToOptionsMap.Find(GetOppositeAdjacency(AdjacencyToOptionsPair.Key)))
			{
				for (const FWFCOption& OppositeVoidAdjacentOption : OppositeVoidAdjacenctOptions->Options)
				{
					if (VoidAdjacentOption == FWFCOption::VoidOption)
					{
						continue;
					}

					EmptyVoidAdjacentKeyOptions.Add(VoidAdjacentOption);
					EmptyVoidAdjacentAdjacencies.Add(GetOppositeAdjacency(AdjacencyToOptionsPair.Key));
					EmptyVoidAdjacentAdjacentOptions.Add(OppositeVoidAdjacentOption);
				}
			}
		}
	}

	// Add stored new constraints to the model
	for (int32 Index = 0; Index < EmptyVoidAdjacentKeyOptions.Num(); Index++)
	{
		WFCModel->AddConstraint(EmptyVoidAdjacentKeyOptions[Index], EmptyVoidAdjacentAdjacencies[Index], EmptyVoidAdjacentAdjacentOptions[Index]);
	}
	
	// Set Contributions
	if (bUseUniformWeightDistribution)
	{
		WFCModel->SetAllContributions(1);
	}
	WFCModel->SetOptionContribution(FWFCOption::BorderOption, 0);
	WFCModel->SetWeightsFromContributions();

	// Append to SpawnExlusion
	for (FSoftObjectPath SpawnExclusionAsset : SpawnExclusionAssets)
	{
		if (SpawnExclusionAsset.IsValid())
		{
			WFCModel->SpawnExclusion.AddUnique(SpawnExclusionAsset);
		}
	}
}

bool UWFCBPLibrary::GetPositionToOptionMapFromActor(AActor* Actor, float TileSize, UPARAM(ref) TMap<FIntVector, FWFCOption>& PositionToOptionMap)
{
	// Check if Model is valid
	if (!Actor)
	{
		UE_LOG(LogTemp, Display, TEXT("GetPositionToOptionMapFromActor called with a null Actor"));
		return false;
	}

	// Derive Grid from Actors
	FIntVector MinPosition = FIntVector::ZeroValue;
	FIntVector MaxPosition = FIntVector::ZeroValue;
	TMap<FIntVector, FWFCOption> TmpPositionToOptionMap;
	
	// Gather ISMComponents
	TInlineComponentArray<UInstancedStaticMeshComponent*> ISMComponents;
	Actor->GetComponents(ISMComponents);
	for (int32 ISMComponentIndex = 0; ISMComponentIndex < ISMComponents.Num(); ISMComponentIndex++)
	{
		UInstancedStaticMeshComponent* ISMComponent = ISMComponents[ISMComponentIndex];
		UStaticMesh* InputStaticMesh = ISMComponent->GetStaticMesh();
		if (!InputStaticMesh)
		{
			UE_LOG(LogTemp, Display, TEXT("%s.%s contains invalid StaticMesh, skipping."), *Actor->GetFName().ToString(), *ISMComponent->GetFName().ToString());
			continue;
		}
		else
		{
			for (int32 InstanceIndex = 0; InstanceIndex < ISMComponent->GetInstanceCount(); InstanceIndex++)
			{
				FTransform InstanceTransform;
				ISMComponent->GetInstanceTransform(InstanceIndex, InstanceTransform);

				// Find CurrentGridPosition
				FVector CurrentGridPositionFloat = InstanceTransform.GetLocation() / TileSize;
				FIntVector CurrentGridPosition = FIntVector(FMath::RoundHalfFromZero(CurrentGridPositionFloat.X), FMath::RoundHalfFromZero(CurrentGridPositionFloat.Y), FMath::RoundHalfFromZero(CurrentGridPositionFloat.Z));
				if (TmpPositionToOptionMap.Contains(CurrentGridPosition))
				{
					UE_LOG(LogTemp, Display, TEXT("%s.%s Index %d is in an overlapping position, skipping."), *Actor->GetFName().ToString(), *ISMComponent->GetFName().ToString(), InstanceIndex);
					continue;
				}

				// Find min/max positions
				MinPosition.X = FMath::Min(MinPosition.X, CurrentGridPosition.X);
				MinPosition.Y = FMath::Min(MinPosition.Y, CurrentGridPosition.Y);
				MinPosition.Z = FMath::Min(MinPosition.Z, CurrentGridPosition.Z);
				MaxPosition.X = FMath::Max(MaxPosition.X, CurrentGridPosition.X);
				MaxPosition.Y = FMath::Max(MaxPosition.Y, CurrentGridPosition.Y);
				MaxPosition.Z = FMath::Max(MaxPosition.Z, CurrentGridPosition.Z);

				// Add to Temporary Position to Option Map
				FWFCOption CurrentOption(InputStaticMesh->GetPathName(), InstanceTransform.Rotator(), InstanceTransform.GetScale3D());
				TmpPositionToOptionMap.Add(CurrentGridPosition, CurrentOption);
			}
		}
	}

	// Re-order grid 
	for (TPair<FIntVector, FWFCOption>& TmpPositionToOption : TmpPositionToOptionMap)
	{
		PositionToOptionMap.Add(TmpPositionToOption.Key - MinPosition, TmpPositionToOption.Value);
	}
	
	return !PositionToOptionMap.IsEmpty();
}

FWFCOption UWFCBPLibrary::MakeEmptyOption()
{
	return FWFCOption::EmptyOption;
}

FWFCOption UWFCBPLibrary::MakeBorderOption()
{
	return FWFCOption::BorderOption;
}

FWFCOption UWFCBPLibrary::MakeVoidOption()
{
	return FWFCOption::VoidOption;
}
