#!/bin/bash
rsync -avzhe ssh --progress --delete ${TARGET_HOST}:/usr/include ${DEVICE_ROOT_FS}/usr/
rsync -avzhe ssh --progress --delete ${TARGET_HOST}:/usr/lib ${DEVICE_ROOT_FS}/usr/
