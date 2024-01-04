// Fill out your copyright notice in the Description page of Project Settings.

#include "FPISMRepresentationFragments.h"

void FFPISMRepresentationFragment::ForEachActiveISMDescription(TFunctionRef<void(const FFPISMDescription& Description)> Func) const
{
	for (const auto& Elem : Layers)
	{
		if (Elem.Value.Layers.Num())
		{
			Func(Elem.Value.Layers[0].Description);
		}
	}
}