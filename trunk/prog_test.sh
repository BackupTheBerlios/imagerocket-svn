#!/bin/bash
for ((a=1; a <= 300; a++))
do
  if LD_LIBRARY_PATH=. ./imagerocket test_images
  then
    echo '*** Done ***'
  else
    echo '*** Error ***'
  fi
  echo `ps -A|grep imagerocket`
done

