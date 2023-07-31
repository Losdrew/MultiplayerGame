// Copyright Artem Volovyi. All Rights Reserved.


#include "System/MGGameplayTagStack.h"

void FMGGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to AddStack"), ELogVerbosity::Warning);
		return;
	}

	if (StackCount > 0)
	{
		for (FMGGameplayTagStack& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const int32 NewCount = Stack.StackCount + StackCount;
				Stack.StackCount = NewCount;
				TagToCountMap[Tag] = NewCount;
				MarkItemDirty(Stack);
				BroadcastStackChange(Stack.Tag, Stack.StackCount);
				return;
			}
		}

		FMGGameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, StackCount);
		MarkItemDirty(NewStack);
		TagToCountMap.Add(Tag, StackCount);
		BroadcastStackChange(Tag, StackCount);
	}
}

void FMGGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to RemoveStack"), ELogVerbosity::Warning);
		return;
	}

	if (StackCount > 0)
	{
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FMGGameplayTagStack& Stack = *It;
			if (Stack.Tag == Tag)
			{
				if (Stack.StackCount <= StackCount)
				{
					It.RemoveCurrent();
					TagToCountMap.Remove(Tag);
					MarkArrayDirty();
				}
				else
				{
					const int32 NewCount = Stack.StackCount - StackCount;
					Stack.StackCount = NewCount;
					TagToCountMap[Tag] = NewCount;
					MarkItemDirty(Stack);
					BroadcastStackChange(Stack.Tag, Stack.StackCount);
				}
				return;
			}
		}
	}
}

void FMGGameplayTagStackContainer::RemoveAllStacks()
{
	for (auto It = Stacks.CreateIterator(); It; ++It)
	{
		FMGGameplayTagStack& Stack = *It;
		It.RemoveCurrent();
		TagToCountMap.Remove(Stack.Tag);
		MarkArrayDirty();
		BroadcastStackChange(Stack.Tag, Stack.StackCount);
	}
}

void FMGGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FGameplayTag Tag = Stacks[Index].Tag;
		TagToCountMap.Remove(Tag);
		BroadcastStackChange(Stacks[Index].Tag, Stacks[Index].StackCount);
	}
}

void FMGGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FMGGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap.Add(Stack.Tag, Stack.StackCount);
		BroadcastStackChange(Stack.Tag, Stack.StackCount);
	}
}

void FMGGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FMGGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap[Stack.Tag] = Stack.StackCount;
		BroadcastStackChange(Stack.Tag, Stack.StackCount);
	}
}

void FMGGameplayTagStackContainer::BroadcastStackChange(FGameplayTag Tag, int32 NewCount)
{
	OnGameplayTagStackChanged.Broadcast(Tag, NewCount);
}
