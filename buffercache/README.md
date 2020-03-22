# Buffer Cache
## Introduction
This is a simulation of the buffer cache in unix systems.
## Working
The webpage first shows you a button to upload a file. After the user uploads a file, the contents of the file are taken 
line by line till the buffer cache is filled. You can retrieve any line number using the textbox provided. If the line
number is already on the buffer cache, then that data is shown. Otherwise, the data is read from the file and stored in
a buffer then shown on the screen. This is done using the algorithm ```getblk```. The UI shows the hash queues and the free list.
