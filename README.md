# libjson-examples
libjson (json-c) usage examples (Tested on Ubuntu 16.04 LTS)

**Dependencies**

1.On Ubuntu 16.04 LTS 

    sudo apt-get install libjson0 libjson0-dev

**Run**

1.To compile run the following from the terminal
    
    make

2.Execute as follows

    ./json_test1
    ./json_test2
 
3.Execute as follows in two different terminals to see how tcp server and tcp client interact using JSON RPC.

    ./json_server  #terminal/tab 1
    ./json_client  #terminal/tab 2

4.To remove binaries run the following from the terminal. 

    make clean
    
    
