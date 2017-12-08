# networkTicTacToe
assignment for computer networks class, TicTacToe over C network

# COSC 522 Project 4 Documentation and Instructions
## Muhammad Sohaib Arif    E01536826 
Compiled and tested on on Ubuntu 16.04 using gcc
### Compile Instructions:
compile game using gcc my_tic_tac_toe.c -o TicTacToe
compile server using gcc TicTacToeServer.c -o TicTacToeServer
compile client using gcc TicTacToeClient.c -o TicTacToeClient
### Usage Instructions:
1. Start the server with ./TicTacToeServer 25220
2. Then start the client with ./TicTacToeClient 127.0.0.1 25220
3. Login using any id
4. follow instructions on client
##### NOTE: Debug information also appears on server

### Bugs:
Sometimes segfaults randomly. Restarting the client or the client and server usually fixes it.
I was unable to find why this was happening

