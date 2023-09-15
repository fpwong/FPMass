// Fill out your copyright notice in the Description page of Project Settings.

#include "FPProjectileProcessors.h"

#include "FPProjectileEvents.h"
#include "FPProjectileFragments.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassSignalSubsystem.h"
#include "ProjectileSim/MassProjectileHitInterface.h"
#include "ProjectileSim/Fragments/MSProjectileFragments.h"

UFPProjectileProcessors::UFPProjectileProcessors()
{
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Movement);
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::Server);
}

void UFPProjectileProcessors::ConfigureQueries()
{
	LineTraceFromPreviousPosition.AddRequirement<FFPProjectileStateFragment>(EMassFragmentAccess::ReadWrite);

	LineTraceFromPreviousPosition.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
	LineTraceFromPreviousPosition.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);

	LineTraceFromPreviousPosition.AddConstSharedRequirement<FFPProjectileEventsFragment>();
	LineTraceFromPreviousPosition.AddConstSharedRequirement<FFPProjectileCollisionParameters>();

	LineTraceFromPreviousPosition.AddTagRequirement<FFPMassTag_Projectile>(EMassFragmentPresence::All);

	LineTraceFromPreviousPosition.RegisterWithProcessor(*this);
}

void UFPProjectileProcessors::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	LineTraceFromPreviousPosition.ForEachEntityChunk(EntityManager, Context, [&](FMassExecutionContext& Context)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_MASS_LineTraceFromPreviousPosition);

		const TArrayView<FFPProjectileStateFragment> ProjectileStateList = Context.GetMutableFragmentView<FFPProjectileStateFragment>();
		TConstArrayView<FMassVelocityFragment> Velocities = Context.GetFragmentView<FMassVelocityFragment>();
		TConstArrayView<FTransformFragment> Transforms = Context.GetFragmentView<FTransformFragment>();

		const FFPProjectileCollisionParameters& CollisionParams = Context.GetConstSharedFragment<FFPProjectileCollisionParameters>();
		const FFPProjectileEventsFragment& ProjectileEvents = Context.GetConstSharedFragment<FFPProjectileEventsFragment>();

		int32 NumEntities = Context.GetNumEntities();

		FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
		for (int32 i = 0; i < NumEntities; ++i)
		{
			FFPProjectileStateFragment& ProjectileState = ProjectileStateList[i];
			FVector CurrentLocation = Transforms[i].GetTransform().GetTranslation();
			FVector Velocity = Velocities[i].Value;

			TEnumAsByte<ECollisionChannel> CollisionChannel = CollisionParams.Channel;

			QueryParams.ClearIgnoredActors();

			//todo-perf: somehow keep the query params around as this is fairly evil to do
			if (ProjectileStateList.Num() > 0)
			{
				for (auto HitActor : ProjectileState.HitActors)
				{
					if (HitActor.IsValid())
					{
						QueryParams.AddIgnoredActor(HitActor.Get());
					}
				}
			}

			TArray<FHitResult> HitResults;

			// DrawDebugLine(GetWorld(), CurrentLocation - (Velocity * Context.GetDeltaTimeSeconds()), CurrentLocation, FColor::Red, false, 0.0f, 0, 12.0f);

			FVector PreviousLocation = CurrentLocation - (Velocity * Context.GetDeltaTimeSeconds());

			if (GetWorld()->LineTraceMultiByChannel(HitResults, PreviousLocation, CurrentLocation, CollisionChannel, QueryParams))
			{
				for (const FHitResult& HitResult : HitResults)
				{
					if (ProjectileStateList.Num() > 0)
					{
						ProjectileState.HitActors.Add(HitResult.GetActor());
					}

					FMassEntityHandle Entity = Context.GetEntity(i);

					if (IsValid(ProjectileEvents.ProjectileEvents))
					{
						Context.Defer().PushCommand<FMassDeferredSetCommand>([ProjectileEvents, Entity, HitResult](FMassEntityManager& Manager)
						{
							ProjectileEvents.ProjectileEvents.GetDefaultObject()->OnHit(Manager.GetWorld(), FMSEntityViewBPWrapper(Manager, Entity), HitResult);
						});
					}
				}
			}
		}
	});
}
