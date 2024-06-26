# lamport-algo
Simulation of Lamport's Algorithm for Distributed Systems for the Course CS6L008 Cloud Computing in IIT Bhubaneswar

The structure of the system is as follows, where the number on the arrow is the PORT which is used for the connection. The code used basically creates a system which acts as a server for one of the connections and as a client for the other. TCP has been used and hence the terminology of server and client. The PORT's used here (6000, 6001 and 6002), have been hardcoded, but we can use other cyclic ports too, like 9002, 9003, 9004 etc.

<img width="535" alt="image" src="https://github.com/vedanta28/20CS02001-20CS01069-20CS01071-20CS01068/assets/74861233/497104e2-293d-48d1-9fa7-1b784f86c497">

# How to run?

1. First Compile system.cpp using `g++ -o system system.cpp` on three different systems.
2. Run the commands simultaneously on the systems as: 
   - On System 0, use the following command: `./system [System 2's IP] 6001 6002`
   - On System 1, use the following command: `./system [System 0's IP] 6000 6001`
   - On System 2, use the following command: `./system [System 1's IP] 6002 6000`
   Make sure to run these commands quicly, otherwise errors might show up, due to connection issues. To make it safe, you can increase the sleep before a connection is made.
4. Run the simulation for the desired duration by executing the command in the terminal. When you wish to stop the simulation, press `Control + C` in the terminal window.
# Video Simulation of the algorithm
Simulation of Lamport's Algorithm for Distributed Systems was done using three linux systems.
Three different linux devices were connected to the same WiFi network and the code given above was run separately on these systems.
The [video simulation](http://www.youtube.com/watch?v=0j5CC9oabvQ) of the same has been recorded by our group and updated on youtube.

# Team Members
- Vedanta Mohapatra (20CS02001)
- Himanshu Lal (20CS01069)
- Ramsundar Tanikella (20CS01071)
- Ayush Kashyap (20CS01068)
