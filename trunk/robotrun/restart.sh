echo "Restart Robot"
sleep 1

while [ 1 ] 
do
  PC=`ps -ex|grep robotrun|grep -v grep | awk '{print $1}'`
  if [ "$PC" == "" ]             # ���� ���μ����� ������
  then
    CMD=`./robotrun 192.168.1.112 &`
	break
  fi
  sleep 1
done


