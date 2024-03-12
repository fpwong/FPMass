// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSlideMovementProcessor.h"

#include "AbilitySystemComponent.h"
#include "FPAbilitySystemFragments.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "AbilitySystem/FPGATypes.h"
#include "FPMass/AI/FPSimpleEnemyProcessor.h"

#define FP_DRAW_DEBUG 0

UFPSlideMovementProcessor::UFPSlideMovementProcessor()
{
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Movement;
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Avoidance);

#if FP_DRAW_DEBUG
	bRequiresGameThreadExecution = true;
#endif
}

void UFPSlideMovementProcessor::ConfigureQueries()
{
	MovementEntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	MovementEntityQuery.AddRequirement<FMassForceFragment>(EMassFragmentAccess::ReadOnly);
	MovementEntityQuery.AddRequirement<FAgentRadiusFragment>(EMassFragmentAccess::ReadOnly);
	MovementEntityQuery.AddRequirement<FFPSimpleMovementFragment>(EMassFragmentAccess::ReadOnly);
	MovementEntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	MovementEntityQuery.AddRequirement<FFPAbilitySystemFragment>(EMassFragmentAccess::ReadOnly);
	MovementEntityQuery.AddRequirement<FFPSimpleEnemyStateFragment>(EMassFragmentAccess::ReadOnly);
	MovementEntityQuery.RegisterWithProcessor(*this);
}

void UFPSlideMovementProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	MovementEntityQuery.ForEachEntityChunk(EntityManager, Context, [](FMassExecutionContext& Context)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_FPSlideMovementProcessor);

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

		const int32 NumEntities = Context.GetNumEntities();

		const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();
		const TArrayView<FFPSimpleMovementFragment> SimpleMovementList = Context.GetMutableFragmentView<FFPSimpleMovementFragment>();
		const TConstArrayView<FMassForceFragment> ForceList = Context.GetFragmentView<FMassForceFragment>();
		const TConstArrayView<FAgentRadiusFragment> RadiusList = Context.GetFragmentView<FAgentRadiusFragment>();
		const TArrayView<FMassVelocityFragment> VelocityList = Context.GetMutableFragmentView<FMassVelocityFragment>();
		const TConstArrayView<FFPAbilitySystemFragment> AbilitySystemList = Context.GetFragmentView<FFPAbilitySystemFragment>();
		const TConstArrayView<FFPSimpleEnemyStateFragment> EnemyStateList = Context.GetFragmentView<FFPSimpleEnemyStateFragment>();

		float DeltaTime = FMath::Min(0.1f, Context.GetDeltaTimeSeconds()); // cap delta time

		auto World = Context.GetWorld();

		float GravityZ = World->GetGravityZ();

		for (int32 EntityIndex = 0; EntityIndex < NumEntities; ++EntityIndex)
		{
			FTransform& Transform = TransformList[EntityIndex].GetMutableTransform();
			FVector Force = ForceList[EntityIndex].Value;
			const float& Radius = RadiusList[EntityIndex].Radius;
			const auto& AbilitySystem = AbilitySystemList[EntityIndex].AbilitySystem;
			const auto& EnemyState = EnemyStateList[EntityIndex];
			const auto& MovementState = SimpleMovementList[EntityIndex];

			bool bCanMove = true;

			// block movement if our max speed is zero
			if (MovementState.MaxSpeed <= 0.0f)
			{
				VelocityList[EntityIndex].Value = FVector::ZeroVector;
				continue;
			}

			// block movement if we cannot move
			if (AbilitySystem.IsValid() && AbilitySystem->Implements<UFPGameplayInterface>())
			{
				if (IFPGameplayInterface* Interface = Cast<IFPGameplayInterface>(AbilitySystem.Get()))
				{
					bCanMove = Interface->CanMove(AbilitySystem.Get());
					if (!bCanMove)
					{
						VelocityList[EntityIndex].Value = FVector::ZeroVector;
						continue;
					}
				}
			}

			FSlide Slide;
			Slide.Shape = FCollisionShape::MakeSphere(Radius);

			float MaxSpeed = SimpleMovementList[EntityIndex].MaxSpeed;

			bool bIsInAir = false;

			{
				FVector GroundTraceStart = Transform.GetLocation();
				GroundTraceStart.Z -= 1; 

				FVector GroundTraceEnd = Transform.GetLocation();

				FHitResult GroundHit;
				if (World->SweepSingleByObjectType(GroundHit, GroundTraceStart, GroundTraceEnd, FQuat::Identity, ObjectParams, Slide.Shape))
				{
					float DistToGround = GroundHit.Location.Z - Transform.GetLocation().Z;
					bIsInAir = DistToGround > Radius;
#if FP_DRAW_DEBUG
					DrawDebugPoint(World, GroundHit.ImpactPoint, 4.0f, FColor::Red, false, 0.5f);
#endif
				}
				else
				{
					bIsInAir = true;
				}
			}

			// ignore external forces when in the air or when unable to move
			if (bIsInAir || !bCanMove)
			{
				Force = FVector(0, 0, GravityZ);
			}

			FVector& Velocity = VelocityList[EntityIndex].Value;
			Velocity += Force * DeltaTime;

			FVector2D XYVelocity = FVector2D(Velocity);
			if (XYVelocity.SquaredLength() >= MaxSpeed * MaxSpeed)
			{
				XYVelocity = XYVelocity.GetSafeNormal() * MaxSpeed;
				Velocity = FVector(XYVelocity, Velocity.Z);
			}

			FVector Delta = Velocity * DeltaTime;
#if FP_DRAW_DEBUG
			if (bIsInAir)
			{
				DrawDebugString(Context.GetWorld(), Transform.GetLocation(), "FALL", nullptr, FColor::White, 0.0f);
			}
#endif

			// UE_LOG(LogTemp, Warning, TEXT("%s %s %d"), *Force.ToCompactString(), *Velocity.ToCompactString(), (int)bIsInAir);

			// perform the slide

			Slide.Position = Transform.GetLocation();
			Slide.Remainder = Delta;
			Slide.QueryParams = FCollisionQueryParams(FName("SlideMovementProcessor"), false, nullptr);

			bool bFailed = false;
			
			// cap out at three impacts
			FHitResult Hit;
			for (int It = 0; It < 3 && Slide.TryStep(World, Hit, false); ++It)
			{
				if (Hit.bStartPenetrating)
				{
					// TODO considering teleporting a bit above the ground to resolve collision if this happens often
					// UE_LOG(LogTemp, Error, TEXT("Entity is stuck!"));
					Slide.Remainder = Delta;
					Slide.TryStep(World, Hit, true); // force the result even if we fail!
					bFailed = true;

#if FP_DRAW_DEBUG
					// Stuck! Draw a red warning sphere
					DrawDebugSphere(World, Slide.Position, Slide.Shape.GetSphereRadius() + 5.0f, 16, FColor::Red);
#endif
					break;
				}
				else
				{
					// Impact! Draw the surface normal
#if FP_DRAW_DEBUG
					// DrawDebugDirectionalArrow(World, Hit.ImpactPoint, Hit.ImpactPoint + 65.0f * Hit.Normal, 10.0f, FColor::Green, false, 0.5f);
#endif
				}
			}

			FVector OldLocation = Transform.GetLocation();

			FVector NewLocation = Slide.Position;

			if (bFailed) // if we fail, try to snap back onto the ground after doing our movement
			{
				FVector GroundTraceStart = NewLocation;
				GroundTraceStart.Z = FMath::Max(NewLocation.Z, Transform.GetLocation().Z);
			
				FVector GroundTraceEnd = NewLocation;
				GroundTraceEnd.Z = FMath::Min(NewLocation.Z, Transform.GetLocation().Z);

				FHitResult GroundHit;
				if (World->SweepSingleByObjectType(GroundHit, GroundTraceStart, GroundTraceEnd + Radius, FQuat::Identity, ObjectParams, Slide.Shape))
				{
					float DistToGround = (GroundHit.Location - Transform.GetLocation()).Size();
					if (DistToGround <= Radius)
					{
						NewLocation.Z = GroundHit.Location.Z + Radius + 10.0f;
					}
				}
			}

			Transform.SetLocation(NewLocation);

			// Recalc velocity (is this correct?)
			Velocity = (Slide.Position - OldLocation) / DeltaTime;
		}
	});
}

// ==========
// FSlide
// ==========

bool FSlide::TryStep(const UWorld* World, FHitResult& Hit, bool bForce)
{
	// early out?
	if (Remainder.IsNearlyZero(KINDA_SMALL_NUMBER))
	{
		return false;
	}

	// sweep forward
	const bool bHit = DepenAndSweep(World, Hit, Position, Remainder, Rotation, Channel, Shape, QueryParams);
	if (!bHit)
	{
		Position = Hit.TraceEnd;
		Remainder = FVector(0.f);
		return false;
	}

	// stuck?
	bool bStuck = false;
	if (Hit.bStartPenetrating)
	{
		bStuck = true;
		if (!bForce)
		{
			return true;
		}
	}

	// update position
	Position = Hit.TraceStart + PullBackMove(Hit.Location - Hit.TraceStart);

	// remainder is either projected onto the impact plane, or the seam between two acute impact-planes
	// dot-product to check the angle
	// cross product to calcuate the seam
	const FVector Seam = (PrevNormal | Hit.Normal) < KINDA_SMALL_NUMBER ? (PrevNormal ^ Hit.Normal) : FVector(0.0f);
	Remainder *= (1.f - Hit.Time);
	Remainder = Seam.IsNearlyZero(0.1f) ?
		FVector::VectorPlaneProject(Remainder, Hit.Normal) :
		Remainder.ProjectOnTo(Seam);
	PrevNormal = Hit.Normal;

	return true;
}

FVector FSlide::PullBackMove(const FVector& Move)
{
	// magic number I found elsewhere in the Unreal Engine source code
	const float Epsilon = 0.125f;
	const float Dist = Move.Size();
	return Dist > Epsilon ? Move * ((Dist - Epsilon) / Dist) : FVector(0.0f);
}

FCollisionShape FSlide::InflateShape(const FCollisionShape& InShape, float Amount)
{
	// helper function to add a "skin" to a shape
	switch (InShape.ShapeType)
	{
		case ECollisionShape::Capsule:
			return FCollisionShape::MakeCapsule(InShape.GetCapsuleRadius() + Amount, InShape.GetCapsuleHalfHeight() + Amount);
		case ECollisionShape::Sphere:
			return FCollisionShape::MakeSphere(InShape.GetSphereRadius() + Amount);
		case ECollisionShape::Box:
			return FCollisionShape::MakeBox(InShape.GetBox() + Amount);
		default:
			return InShape;
	}
}

bool FSlide::DepenAndSweep(const UWorld* World, FHitResult& Hit, const FVector& Start, const FVector& Delta, const FQuat& Rot, ECollisionChannel InChannel, const FCollisionShape& InShape, const FCollisionQueryParams& Params)
{
	Hits.Reset();

	// sweep with inflated skin
	FCollisionShape Inflated = InflateShape(InShape, 0.25f);
	const bool bSkinHit = World->SweepMultiByChannel(Hits, Start, Start + Delta, Rot, InChannel, Inflated, Params);

	// no hits, even with skin?
	if (!bSkinHit)
	{
		Hit.Init();
		Hit.TraceStart = Start;
		Hit.TraceEnd = Start + Delta;
		Hit.Location = Start + Delta;
		Hit.Time = 1.0f;
		return false;
	}

	// have initial overlaps?
	int NumInitialOverlaps = 0;
	for (const FHitResult& It : Hits)
	{
		if (It.bStartPenetrating)
		{
			++NumInitialOverlaps;
		}
	}

	if (NumInitialOverlaps == 0)
	{
		Hit = Hits.Last();
		return bSkinHit;
	}

	// iteratively resolve penetration
	FVector Fixup(0.0f);
	for (int Iter = 0; Iter < 16; ++Iter)
	{
		float ErrorSum = 0.0f;
		for (const FHitResult& It : Hits)
		{
			if (It.bStartPenetrating)
			{
				// take the dot-product of the Fixup and the Normal to determine how much
				// of the penetration has already been taken care of.
				const float Error = FMath::Max(0.0f, (It.PenetrationDepth + 0.125f) - (Fixup | It.Normal));
				ErrorSum += Error;
				Fixup += Error * It.Normal;
			}
		}
		// If we've found a solution, stop iterating.
		if (ErrorSum < KINDA_SMALL_NUMBER)
		{
			break;
		}
	}

	// resweep from new start
	return World->SweepSingleByChannel(Hit, Start + Fixup, Start + Fixup + Delta, Rot, InChannel, InShape, Params);
}
