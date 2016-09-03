echo "Restart Robot"
sleep 1

while [ 1 ] 
do
	CNT=$(ps -ef|grep robotrun|grep -v grep|wc -l)
	if [ $CNT -le 0 ]
	then
		./robotrun 192.168.1.112 &
		exit
	fi
	sleep 1
done


