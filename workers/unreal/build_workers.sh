#/bin/
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

if [ -z "$UNREAL_HOME" ]; then
	echo "ERROR: you must set the environment variable UNREAL_HOME to the location of your Unreal Engine source directory"
	exit 1
fi

BUILD_TOOL="$UNREAL_HOME""/Engine/Build/BatchFiles/RunUAT.bat"
PROJECT_PATH=$SCRIPTPATH"/"
PROJECT_NAME="unreal"

TEMP_DIR=$PROJECT_PATH"temp_worker_builds/"

if [ -d $TEMP_DIR ]; then
	rm -rf $TEMP_DIR # remove the output directory if already exists
fi

FSIM_DIR=$TEMP_DIR"fsim/"
CLIENT_DIR=$TEMP_DIR"client/"

echo "Building unreal fsim worker..."

eval \"$BUILD_TOOL\" BuildCookRun -project=\"$PROJECT_PATH$PROJECT_NAME.uproject\" -noP4 -platform=Win64 -clientconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory=\"$FSIM_DIR\"

echo "Building unreal client worker..."

eval \"$BUILD_TOOL\" BuildCookRun -project=\"$PROJECT_PATH$PROJECT_NAME.uproject\" -noP4 -platform=Win64 -clientconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory=\"$CLIENT_DIR\"

WORKER_ASSEMBLY_DIR=$PROJECT_PATH"../../build/assembly/worker/"

FSIM_NAME="UnrealFSim@Windows"
CLIENT_NAME="UnrealClient@Windows"

GENERATED_FOLDER="WindowsNoEditor/"
GENERATED_EXE=$PROJECT_NAME".exe"

echo "Zipping up Fsim..."

pushd $FSIM_DIR$GENERATED_FOLDER
mv $GENERATED_EXE $FSIM_NAME".exe"
jar -cMf $FSIM_NAME".zip" "."
popd

pushd $CLIENT_DIR$GENERATED_FOLDER
mv $GENERATED_EXE $CLIENT_NAME".exe"
jar -cMf $CLIENT_NAME".zip" "."
popd

mkdir -p $WORKER_ASSEMBLY_DIR

pushd $WORKER_ASSEMBLY_DIR

if [ -f $FSIM_NAME".zip" ]; then
	rm -rf $FSIM_NAME".zip"
fi

if [ -f $CLIENT_NAME".zip" ]; then
	rm -rf $CLIENT_NAME".zip"
fi

popd

mv $FSIM_DIR$GENERATED_FOLDER$FSIM_NAME".zip" $WORKER_ASSEMBLY_DIR$FSIM_NAME".zip"

mv $CLIENT_DIR$GENERATED_FOLDER$CLIENT_NAME".zip" $WORKER_ASSEMBLY_DIR$CLIENT_NAME".zip"

rm -rf $TEMP_DIR # remove the output directory as no longer need it

echo "Finished building workers"
