chmod +x build_trigger.sh
chmod +x build.sh
./build_trigger.sh
if [ $? != 0 ]; then
	echo "trigger failed"
	exit 1
fi

#./build.sh
