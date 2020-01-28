temp_arg=$1
for i in $(ls | grep maze); do
	sed -i "s/^[0-4]/$temp_arg/g" $i;
done;
