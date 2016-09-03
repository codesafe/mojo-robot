echo "Restart Robot"
sleep 1

while [ 1 ] 
do
	CNT=$(ps -ef|grep robotrun|grep -v grep|wc -l)
	if [ $CNT -le 0 ]
	then
		CMD='./robotrun 192.168.1.112 &'
		break
	fi
	sleep 1
done


