#!/bin/bash
sshfs -o allow_other -o IdentityFile=~/.ssh/id_rsa ${TARGET_HOST} ${DEVICE_ROOT_FS}
