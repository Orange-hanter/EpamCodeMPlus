# Task1. Copy tool.

## Algorithm
[  thread1   ]   <->         [  thread 2  ]

get_package      <->         check shared memory (should be empty)

set flag data_ready    <->   idle

wait flag data sended   <->  wake up

idle           <->           send

idle             <->         set flag data_sended

Algorithm could be outdate

## Test result 
* Attempt 0 7154 ms
* Attempt 1 7136 ms
* Attempt 2 6857 ms
* Attempt 3 6534 ms
* Attempt 4 6821 ms
* Attempt 5 7743 ms
* Attempt 6 6642 ms
* Attempt 7 6658 ms
* Attempt 8 6402 ms
* Attempt 9 6416 ms
* 
* Average time of copying is: 6836ms

## How to run
```sh
Clone.App.exe -s .\path\to\file -d C:\absolute\path\to\destination\folder
```


Average multithread time: 7.209716582298279
Average ipc time: 6.478101754188538