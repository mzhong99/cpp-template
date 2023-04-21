#!/bin/bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
SCRIPT_NAME=$(basename ${SCRIPT_DIR})

function usage() {
    echo "${SCRIPT_NAME} [-h|--help] [-c|--clean] [-s|--skip-docker] [-u|--unittest]"
    echo "    [-o|--output <dir>] [-n|--container-name <name>]"
    exit 1
}

OUTPUT_DIR=${SCRIPT_DIR}/artifacts
SKIP_DOCKER_BUILD=0
DOCKER_IMAGE_NAME="fedora:cpp-image"
DOCKER_CONTAINER_NAME="cpp-container"

ARGSCOPY=$@
VALID_ARGS=$(getopt -o hcsuo:n: --long help,clean,skip-docker,unittest,output:,container-name: -- "$@")
if [[ $? -ne 0 ]]; then
    usage
fi

set -euo pipefail

eval set -- "$VALID_ARGS"
while [ : ]; do
    case "$1" in
        -h | --help)
            usage
            ;;
        -c | --clean)
            echo "Clean build directory"
            shift
            ;;
        -s | --ship-docker)
            SKIP_DOCKER_BUILD=1
            shift
            ;;
        -u | --unittest)
            echo "Running unit tests"
            shift
            ;;
        -o | --output)
            OUTPUT_DIR=$2
            shift 2
            ;;
        -n | --container-name)
            DOCKER_CONTAINER_NAME=$2
            shift 2
            ;;
        --) shift
            break
            ;;
    esac
done

pushd ${SCRIPT_DIR}

if [ -f /.dockerenv ]; then
    echo "Execute CMake Build Commands"
    echo "--------------------------------------------------------------------------------"

    chown -R $(whoami) ${SCRIPT_DIR}
    git config --global --add safe.directory ${SCRIPT_DIR}
    git submodule update --init --recursive

    mkdir -p ${OUTPUT_DIR}
    pushd ${OUTPUT_DIR}
    cmake ${SCRIPT_DIR}
    cmake --build . --parallel $(nproc)
    ${OUTPUT_DIR}/unittests --duration
    popd
else
    echo "Execute Docker Commands"
    echo "--------------------------------------------------------------------------------"
    if ! docker --version; then
        echo "Docker CLI missing. Please install Docker CLI first."
        exit 1
    fi

    if [[ SKIP_DOCKER_BUILD -eq 0 ]]; then
        docker build --file ${SCRIPT_DIR}/Dockerfile --tag ${DOCKER_IMAGE_NAME} .
        docker rm -f ${DOCKER_CONTAINER_NAME}
        docker run --interactive --name ${DOCKER_CONTAINER_NAME} --detach ${DOCKER_IMAGE_NAME}
    fi

    docker exec --interactive --tty ${DOCKER_CONTAINER_NAME} /home/validator/build.sh ${ARGSCOPY}
fi

popd
