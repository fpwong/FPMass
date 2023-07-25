# FPMass

Suggested you use this for reference only: I do not know what I'm doing!

The plugin will not build without including some of my other plugins and one of these is private.
I may remove this requirement in the future or fix up

Requirements
- [FPGameplayAbilities](https://github.com/fpwong/FPGameplayAbilities)
- [MassSample](https://github.com/Megafunk/MassSample)
- AnimToTexture (Inbuilt)
- GameplayAbilities (Inbuilt)
- FPVertexAnimationTextures (private repo, try to remove any references to this)

## What is this plugin doing with mass?

Using MASS to communicate with GameplayAbility system
- Mass handles the projectile and unit movement
- Actors are required to handle the projectile collision and have a GAS component attached to them
- Vertex Animated Textures for representation