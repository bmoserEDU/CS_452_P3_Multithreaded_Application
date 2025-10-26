# Project X

- Name: Bowen Moser
- Email: bowenmoser@u.boisestate.edu
- Class: 452-002

## Known Bugs or Issues

No known bugs or issues at this time

## Experience

This was an enjoyable but challening project. Since this is my second time ever writing
a multithreaded program, I referred back to my previous program to help remember how to 
structure the thread creation and joining of my threads back into the main thread. I did use Claude for ideas/help 
and for writing my tests since it has been helpful in the past when I encountered roadblocks.

I did a bit of research into the POSIX thread standard and found barriers. I worked on implementing one to 
try and ensure thread synchronization after all threads had succesfully merged their portion of the array. 
I'm not sure if it was really necessary, but I think it was a nice way of learning another concept and 
applying it to my program. My thought process was if I implement a barrier, I can make sure threads proceed to joining
in an "orderly" fashion with an added layer of control to their behavior, especially since it's hard (or practically impossible)
to verify with just unit tests. 

pthread_barrier doc: https://www.gnu.org/software/gnuastro/manual/html_node/Implementation-of-pthread_005fbarrier.html

## Analysis

Overall, my graph doesn't match the example one as threads increase. I've tried a few different things but it sort of stagnates as threads increase, 
rather than increase in time as the number of threads goes up. I think this might be one or more factors. The first is that I'm 
only locking when the main thread merges all other threads together. So it's really just dependent on the main thread merging rather
than each thread sorting and context switching between each one. I've tried to move the mutex lock around but the results have been 
essentially the same, so I'm not sure where else to implement the lock. The other is it may have something to do with my barrier
causing all threads to wait until each one is done? Again, I've tried a few different places to implement the barrier but I think
it makes the most sense where it is currently used. That being said, my results are pretty consistent (running on Onyx) and
the program is fastest between 14 and 16 threads before it begins to stagnate.

Were you able to generate something close to what the example showed? Why or why not?

Yes and no, the graph does start off by showing my program does run faster and faster as the number of threads increase. However, as the number of threads continue
to increase, the graph does not begin to substantially rise in terms of run time. Instead it begins to stagnate across the y axis. 
While this does indicate that there is a slowdown that correlates with the thread count and my program efficieny, 
it does not increase greatly in terms of my run time as we got closer to thirty two threads. 


Did you see a slowdown at some point? Why or why not?

From my graph, I do see an eventual slowdown. Generally the slowdown appears after about 20 to 25 threads. 
As seen in the graph, the total amount of time taken begins to plateau after this 20 to 25 thread mark.
I think this is because my main thread is really the one being tasked with doing all of the merging, and is
the only place a mutex lock is being used. I identified the merging portion as a critical section of code
and implemented this lock to protect the sorting action. 


Did your program run faster and faster when you added more threads? Why or why not?

Yes my program does run faster and faster as the number of threads for my program increases. This is because
I am partitioning the array into an evenly distributed amount of sections that is based on how many
threads I entered from main. I tried to replicate the second approach we discussed in class last week. 
Since I'm able to distribute smaller sections of the total array to each thread, each thread has to sort less
of the total array. This is a divide and conquer approach that I think works very well in this scenario where we
are giving the program a large, unsorted array to handle. The main thread then handles the final merge of each section 
of the list, returning it as one sorted array of integers. 


What was the optimum number of threads for your machine?

The optimum number of threads for my machine (Onyx) has been consistenly between 14 and 16 total threads.


What was the slowest number of threads for your machine?

Technically the slowest number of threads is when you just have one. But the law of diminishing returns
begins to take effect after 20 or so threads. The graph fluctuated slightly between script runs but that
may have to do more with Onyx resource utiliziation constantly changing which is making minor changes to the graph output.

![Example Image](scripts/student_plot.png)