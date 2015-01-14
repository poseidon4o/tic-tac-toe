# tic-tac-toe


## Usage:
 * Compile and run
 * Start the server
 * Start AiClient with 1st argument IP of the server and second optional argument the number of AI players (Optional step, only if you want AI players in the server)
 * Use TextClient or netcat or any text TCP client to connect to the server using port 8090 and play!


## TODO
 * Add logging - alot of it!
 * Fix minmax algorithim, it is ugly and inefficient - all possible boards can be precomputed and level can be simulated with random chance.


## Notex
 * There are two clients interfaces in the server so we can have very simple frontend client and support some other tools. This allows practically any application to talk to the server.
 * All computations across the server and the AI client(s) are simple enough to be done in one thread, however if you launch high enough ai clients from one instance then they will wait for each other. Same goes for the server - under high load delays are expected.