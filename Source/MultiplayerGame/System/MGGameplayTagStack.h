// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GameplayTags.h"
#include "MGGameplayTagStack.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMGGameplayTagStackContainer_TagStackChanged, FGameplayTag, Tag, int32, StackCount);

struct FMGGameplayTagStackContainer;
struct FNetDeltaSerializeInfo;

/**
 * FMGGameplayTagStack
 *
 *  Represents one stack of a gameplay tag (tag + count)
 */
USTRUCT(BlueprintType)
struct FMGGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FMGGameplayTagStack()
	{}

	FMGGameplayTagStack(FGameplayTag InTag, int32 InStackCount) : Tag(InTag), StackCount(InStackCount)
	{}

private:

	friend FMGGameplayTagStackContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

/**
 * FMGGameplayTagStackContainer
 *
 *  Container of gameplay tag stacks
 */
USTRUCT(BlueprintType)
struct FMGGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	FMGGameplayTagStackContainer()
	{}

public:
	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	void AddStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	int32 GetStackCount(FGameplayTag Tag) const
	{
		return TagToCountMap.FindRef(Tag);
	}

	// Returns true if there is at least one stack of the specified tag
	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}

	// Removes all tag stacks
	void RemoveAllStacks();

	//~FFastArraySerializer interface
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer interface

	void BroadcastStackChange(FGameplayTag Tag, int32 NewCount);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FMGGameplayTagStack, FMGGameplayTagStackContainer>(Stacks, DeltaParms, *this);
	}

public:

	UPROPERTY()
	FMGGameplayTagStackContainer_TagStackChanged OnGameplayTagStackChanged;

private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FMGGameplayTagStack> Stacks;
	
	// Accelerated list of tag stacks for queries
	TMap<FGameplayTag, int32> TagToCountMap;
};

template<>
struct TStructOpsTypeTraits<FMGGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FMGGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

