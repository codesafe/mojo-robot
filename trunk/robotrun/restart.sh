echo "Restart Robot"
sleep 1

while [ 1 ] 
do
  PC=`ps -ex|grep robotrun|grep -v grep | awk '{print $1}'`
  if [ "$PC" == "" ]             # 실행 프로세스가 없으면
  then
    CMD=`./robotrun 192.168.1.112 &`
	break
  fi
  sleep 1
done


