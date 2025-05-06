# Better Spawn Loop
When 2.2 released, a better way to create spawn loops was found, using the Spawn Ordered feature. This reduces the object and group usage down to 2 objects and 1 group for any size of spawn loop.
However, the create loop button uses the old style of creating spawn loops. This mod fixes that. 

To change the spawn loop after creation, you just need to place triggers where you want them to activate, set them to spawn triggered and multi-triggered, and add them to the group that the other triggers are a part of.
To change the length of the loop, you need to change the "Delay" in the lower spawn trigger.

This mod also supports loading some boilerplate triggers. These include:
- an initialization group that you can add objects to in order to only run when the loop is initialized
- an initialization trigger that can be easily copied to wherever you want the loop to start
- a color trigger that is touch-triggered and does not activate, but is useful as its duration line can show how long the loop runs for
- 2 stop triggers that when used together, will stop the loop no matter where it is. These can also be copied to wherever you want the loop to end. 

These are very useful for creating effect loops in platformer mode levels