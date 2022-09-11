Copy tool.

[  thread1   ]            [  thread 2  ]

get_package                 check shared memory (should be empty)

set flag data_ready         idle

wait flag data sended      wake up

idle                        send

idle                        set flag data_sended