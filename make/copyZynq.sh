#!/bin/bash
LIBS=$(find -L ./ | grep "lib.*\.so")
ENVS=$(find -L ./ | grep "env\.sh" | grep -v "doc")

${ZYNQ_IP:?ZYNQ_IP=192.168.30.12}
#if [ ! -z "$ZYNQ_IP" ]; then
#    ZYNQ_IP=192.168.30.12
#fi

echo "Copying to ${ZYNQ_IP}"

ssh-copy-id root@${ZYNQ_IP}

scp $LIBS root@${ZYNQ_IP}:/work/lib

#BUTool bins
scp -r ./bin/* root@${ZYNQ_IP}:/work/bin

#Apollo bins
scp -r ./plugins/*/bin/* root@${ZYNQ_IP}:/work/bin


rm -f temp.env
touch temp.env
for file in $ENVS
do 
    cat $file >> temp.env
done
echo "PATH=\$PATH:\$PWD/bin/tool" >> temp.env
scp temp.env root@${ZYNQ_IP}:/work/env.sh
rm temp.env
