// Copyright Epic Games, Inc. All Rights Reserved.

#include "WFCModel.h"
#include "Engine/StaticMesh.h"

const FWFCOption FWFCOption::EmptyOption(FString(TEXT("/WFC/Core/SpecialOptions/Option_Empty.Option_Empty")));
const FWFCOption FWFCOption::BorderOption(FString(TEXT("/WFC/Core/SpecialOptions/Option_Border.Option_Border")));
const FWFCOption FWFCOption::VoidOption(FString(TEXT("/WFC/Core/SpecialOptions/Option_Void.Option_Void")));

void UWFCModel::AddConstraint(const FWFCOption& KeyOption, EWFCAdjacency Adjacency, const FWFCOption& AdjacentOption)
{
	Modify(true);
	// If the KeyOption key exists in Constraints
	if (FWFCAdjacencyToOptionsMap* AdjacencyToOptionsMap = Constraints.Find(KeyOption))
	{
		// If the Adjacency key exists in the AdjacencyToOptionsMap
		if (FWFCOptions* Options = AdjacencyToOptionsMap->AdjacencyToOptionsMap.Find(Adjacency))
		{
			// If the Options array does not contain the Option, create it
			if (!Options->Options.Contains(AdjacentOption))
			{
				Options->Options.Add(AdjacentOption);
				AdjacencyToOptionsMap->Contribution += 1;
			}
		}
		else // Create the Option and add it to the AdjacencyToOptionsMap
		{
			FWFCOptions NewOptions;
			NewOptions.Options.Add(AdjacentOption);
			AdjacencyToOptionsMap->AdjacencyToOptionsMap.Add(Adjacency, NewOptions);
			AdjacencyToOptionsMap->Contribution += 1;
		}
	}
	else // Create the Option, add it to the AdjacencyToOptionsMap, and add it the Constraints map
	{
		FWFCOptions NewOptions;
		NewOptions.Options.Add(AdjacentOption);
		FWFCAdjacencyToOptionsMap NewAdjacencyToOptionsMap;
		NewAdjacencyToOptionsMap.AdjacencyToOptionsMap.Add(Adjacency, NewOptions);
		Constraints.Add(KeyOption, NewAdjacencyToOptionsMap);
	}
	
}

FWFCOptions UWFCModel::GetOptions(const FWFCOption& KeyOption, EWFCAdjacency Adjacency) const
{
	// If KeyOption key exists
	if (const FWFCAdjacencyToOptionsMap* AdjacencyToOptionsMap = Constraints.Find(KeyOption))
	{
		// If Adjacency key exists, return FoundOptions
		if (const FWFCOptions* FoundOptions = AdjacencyToOptionsMap->AdjacencyToOptionsMap.Find(Adjacency))
		{
			return *FoundOptions;
		}
	}
	// If nothing is found above, return empty
	return FWFCOptions();
}

void UWFCModel::SetWeightsFromContributions()
{
	if (Constraints.IsEmpty())
	{
		return;
	}

	Modify(true);
	int32 SumOfContributions = 0;
	for (TPair<FWFCOption, FWFCAdjacencyToOptionsMap>& Constraint : Constraints)
	{
		SumOfContributions += Constraint.Value.Contribution;
	}

	for (TPair<FWFCOption, FWFCAdjacencyToOptionsMap>& Constraint : Constraints)
	{
		Constraint.Value.Weight = float(Constraint.Value.Contribution) / SumOfContributions;
	}
}

void UWFCModel::SetAllWeights(float Weight)
{
	if (Constraints.IsEmpty())
	{
		return;
	}

	Modify(true);
	for (TPair<FWFCOption, FWFCAdjacencyToOptionsMap>& Constraint : Constraints)
	{
		Constraint.Value.Weight = Weight;
	}
}

void UWFCModel::SetAllContributions(int32 Contribution)
{
	if (Constraints.IsEmpty())
	{
		return;
	}

	Modify(true);
	for (TPair<FWFCOption, FWFCAdjacencyToOptionsMap>& Constraint : Constraints)
	{
		Constraint.Value.Contribution = Contribution;
	}
}

void UWFCModel::SetOptionContribution(const FWFCOption& Option, int32 Contribution)
{
	if (Constraints.Contains(Option))
	{
		Modify(true);
		FWFCAdjacencyToOptionsMap& AdjacencyToOptionsMap = *Constraints.Find(Option);
		AdjacencyToOptionsMap.Contribution = Contribution;
	}
}

float UWFCModel::GetOptionWeight(const FWFCOption& Option) const
{
	if (const FWFCAdjacencyToOptionsMap* Constraint = Constraints.Find(Option))
	{
		return Constraint->Weight;
	}
	else
	{
		return 0;
	}
}

int32 UWFCModel::GetOptionContribution(const FWFCOption& Option) const
{
	if (const FWFCAdjacencyToOptionsMap* Constraint = Constraints.Find(Option))
	{
		return Constraint->Contribution;
	}
	else
	{
		return 0;
	}
}

int32 UWFCModel::GetConstraintCount() const
{
	int32 ConstraintCount = 0;
	for (const TPair<FWFCOption, FWFCAdjacencyToOptionsMap>& Constraint : Constraints)
	{
		for (const TPair<EWFCAdjacency, FWFCOptions>& AdjacencyToOptions : Constraint.Value.AdjacencyToOptionsMap)
		{
			ConstraintCount += AdjacencyToOptions.Value.Options.Num();
		}
	}
	return ConstraintCount;
}

void UWFCModel::SwapMeshes(TMap<UStaticMesh*, UStaticMesh*> SourceToTargetMeshMap)
{
	Modify(true);
	for (TPair<UStaticMesh*, UStaticMesh*>& SourceToTargetMesh : SourceToTargetMeshMap)
	{
		UStaticMesh* SourceMesh = SourceToTargetMesh.Key;
		UStaticMesh* TargetMesh = SourceToTargetMesh.Value;
		for (TPair<FWFCOption, FWFCAdjacencyToOptionsMap>& Constraint : Constraints)
		{
			if (Constraint.Key.BaseObject == SourceMesh)
			{
				Constraint.Key.BaseObject = TargetMesh;
			}

			for (TPair<EWFCAdjacency, FWFCOptions>& AdjacencyToOptions : Constraint.Value.AdjacencyToOptionsMap)
			{
				for (FWFCOption& Option : AdjacencyToOptions.Value.Options)
				{
					if (Option.BaseObject == SourceMesh)
					{
						Option.BaseObject = TargetMesh;
					}
				}
			}
		}
	}
}
