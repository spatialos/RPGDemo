# Unreal Top Down C++ SpatialOS Demo

This example project is the Unreal Top Down Character (C++) template project (available from the Epic Unreal Engine launcher) integrated with SpatialOS. With a few easy steps, we've converted this simple single player Unreal project into an online multiplayer game with NPCs attached to AIControllers that run on managed Unreal workers.

To see the demo in action:
- Ensure that the environment variable `UNREAL_HOME` is set to the root directory of your Unreal Installation
- Clone the repo: `git clone https://github.com/improbable-public/unreal-top-down.git`
- Move into the directory: `cd unreal-top-down`
- Build the project: `spatial build`
- Run: `spatial local start`
- Open the World Viewer to see the NPCs and the spooled up managed Unreal AI worker moving them all about: `http://localhost:5050`
- Connect multiple player clients: `spatial worker launch unreal client`

## Project Structure

Since this project is adapted from the template Top Down C++ project, all of the logic for `Player` entities and the player controller is in C++. However, to exemplify blueprint integration with SpatialOS, the logic for `Npc` entities and NPC controllers is all contained in blueprints.

Important things to note:
- The prefab name of a nature, as defined in the Gsim, must correspond to the name of a blueprint in the `EntityBlueprints` folder in the root of the Content Browser in the Unreal Editor. Note that, even though all of our `Player` logic is in C++, specifically in the `AunrealCharacter` class, we have created a blueprint child of this class called `Player` that resides in the `EntityBlueprints` folder.
- The `TransformSender` (`UTransformSender`) and `TransformReceiver` (`UTransformReceiver`) components can be added to Actors to sync the `Transform` state between Unreal workers and SpatialOS.
- A `TransformReceiver` simply reads values from the `TransformState` and exposes them in public methods for consumption by your game-specific logic.
- A `TransformSender` will read the Actor's location and rotation every frame, and send `TransformState` updates to SpatialOS, but only if it is running on an Unreal worker that is authoritative over the transform of that entity.
- For now, the same code runs on an Unreal Fsim / managed worker and an Unreal Client. When we build our project, both a client and fsim are generated in the `build/assembly/worker` folder, with the only difference being that the former connects to SpatialOS with `WorkerType = "UnrealClient"` and the latter with `WorkerType = "UnrealFsim"`. See the Gsim bridge settings and the player lifecycle manager for how we distinguish between the two in the Gsim.