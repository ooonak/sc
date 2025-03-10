#!/bin/sh

while true; do
  nc -lkU /tmp/sc_test.sock
  unlink /tmp/sc_test.sock
done

