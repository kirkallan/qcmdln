echo Running on node_name $NODE_NAME with label $HOST and node $NODE
echo workspace is: $WORKSPACE
echo curdir is: $PWD
if [ "$NODE_NAME" = "$HOST" ]; then
	echo "$NODE_NAME = $HOST"
    echo "we're good to go"
    chmod +x build.sh
    ./build.sh
else
	echo "$NODE_NAME != $HOST"
    echo "need to exit"
fi
echo "build_trigger.sh finished"
