# FPMass

Suggested you use this for reference only: I do not know what I'm doing!

The plugin will not build without including some of my other plugins and one of these is private.
I may remove this requirement in the future or fix up

Requirements
- [FPGameplayAbilities](https://github.com/fpwong/FPGameplayAbilities)
- [MassSample](https://github.com/Megafunk/MassSample)
- AnimToTexture (Engine plugin)
- GameplayAbilities (Engine plugin)
- FPVertexAnimationTextures (my private repo, remove any references to this you find in the plugin source)

## What is this plugin doing with mass?

Using MASS to communicate with GameplayAbility system
- Mass handles the projectile and unit movement
- Actors are required to handle the projectile collision and have a GAS component attached to them
- Vertex Animated Textures for characters
- No multiplayer (however...) 

# Mass Replication

* Check out arkena00's implementation here: https://github.com/arkena00/sample.mass
* I have tested their code (with basically no changes) on 5.3, which you can find on [this branch](https://github.com/fpwong/FPMass/tree/mass-replication-53) (however I suggest you pull arkena00's repo since it has blueprints and stuff setup).
* It should be noted there are rumors of a *secret* internal project at Unreal with Mass (and using Mass Replication with Iris). So things are likely to change in the future regarding this. But it is fun to see it in action right now!