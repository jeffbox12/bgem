#!/bin/sh -e

bgemRootDir=$(X= cd -- "$(dirname -- "$0")" && pwd -P)
PROJECT_ROOT="$bgemRootDir"

. "$bgemRootDir/scripts/bootstrap.sh"
