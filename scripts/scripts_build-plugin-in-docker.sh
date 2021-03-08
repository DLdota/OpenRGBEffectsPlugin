#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

docker build -f "${DIR}/build.Dockerfile" -t openrgbeffectplugin-builder "${DIR}" &&\
docker run --rm -ti -v "${DIR}/../":/build openrgbeffectplugin-builder &&\
if [ -d ~/.config/OpenRGB/plugins ]; then cp libORGBEffectPlugin.so.1.0.0 ~/.config/OpenRGB/plugins/libORGBEffectPlugin.so; fi