#!/bin/bash

q -t "select c1,c2,sum(c3) as [time], count(c1) as [count] from profile_data.txt group by c1,c2 order by time desc"
