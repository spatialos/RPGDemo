# Rough setup guide for an end user, brain-dumped by Jared

* Clone UnrealEngine, `git clone git@github.com:EpicGames/UnrealEngine.git --branch=4.14`. It is necessary to build from source because we use the Server build configuration, which is not included in the Epic installer.
* Apply `0001-Make-linux-work-with-WorkerSdk.patch` to the UnrealEngine source to fix a couple of issues that prevent SpatialOS Linux workers from building and running properly.
* Set an environment variable `UNREAL_HOME` to the root folder where you cloned the UnrealEngine.
