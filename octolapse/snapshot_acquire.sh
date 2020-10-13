#!/bin/bash
SNAPSHOT_NUMBER=$1
DELAY_SECONDS=$2
DATA_DIRECTORY=$3
SNAPSHOT_DIRECTORY=${4}
SNAPSHOT_FILENAME=$5
SNAPSHOT_FULL_PATH=${6}

if [ ! -d "${SNAPSHOT_DIRECTORY}" ];
then
  echo "Creating directory: ${SNAPSHOT_DIRECTORY}"
  mkdir -p "${SNAPSHOT_DIRECTORY}" || exit 1
fi

remote_tmp_file="/tmp/capture_octolapse.jpg"
wget -q tomrasp1:5000/api/v1/resources/dslr_snap -O "${SNAPSHOT_FULL_PATH}"
if [ ! -f "${SNAPSHOT_FULL_PATH}" ];
then
  echo "The snapshot was not found in the expected directory: '${SNAPSHOT_FULL_PATH}'." >&2
  exit 1
fi
