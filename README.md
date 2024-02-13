# Stable-Sketch

Stable-Sketch is a memory-efficient sketch that achieves high detection accuracy and fast update speed. It uses a probabilistic strategy to substitute tracked items in buckets based on multidimensional features (item information and bucket stability). The Stable-Sketch implementation was developed based on the open-source code of MV-Sketch and we are thankful for the contributions made by the original authors.


## Compile and Run the examples
Stable-Sketch is implemented in C++. Below, we provide instructions for compiling the examples on Ubuntu using g++ and make.

### Requirements
Before proceeding, please ensure that the following requirements are met:

- g++ and make are installed on your system. Our experimental platform uses Ubuntu 20.04 and g++ 9.4.0.

- The libpcap library is installed on your system. Most Linux distributions include libpcap and can be installed using package managers such as apt-get in Ubuntu.

### Dataset

- You can download the pcap file and specify the path of each pcap file in the iptraces.txt file.

### Compile
- To compile the examples, use the following commands with make:

```
    $ make main_hitter
    $ make main_changer
```
  

### Run
- To execute the examples, run the following commands. The program will output statistics about the detection accuracy.

```
$ ./main_hitter
$ ./main_changer
```
