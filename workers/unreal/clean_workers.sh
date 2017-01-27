#/bin/
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

PROJECT_PATH=$SCRIPTPATH"/"
PROJECT_NAME="unreal"

OUTPUT_DIR=$PROJECT_PATH"temp_worker_builds/"

echo "Removing worker build output directory: " $OUTPUT_DIR

if [ -d $OUTPUT_DIR ]; then
	rm -rf $OUTPUT_DIR # remove the output directory if already exists
fi

rm -rf $OUTPUT_DIR # remove the output directory if already exists

WORKER_ASSEMBLY_DIR=$PROJECT_PATH"../../build/assembly/worker/"
FSIM_ZIP="UnrealFSim@Windows.zip"
CLIENT_FOLDER="UnrealClient@Windows"

echo "Removing worker builds... "

if [ -d $WORKER_ASSEMBLY_DIR ]; then
	pushd $WORKER_ASSEMBLY_DIR

	if [ -d $CLIENT_FOLDER ]; then
		rm -rf $CLIENT_FOLDER
	fi

	if [ -f $FSIM_ZIP ]; then
		rm -rf $FSIM_ZIP
	fi

	popd
fi
