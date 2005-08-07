for ((a=1; a <= 300; a++))
do
  if LD_LIBRARY_PATH=. app/imagerocket test
  then
    echo '*** Done ***'
  else
    echo '*** Error ***'
  fi
  echo `ps -A|grep imagerocket`
done

