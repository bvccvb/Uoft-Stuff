CC = g++  
P = -pthread
TM = -fgnu-tm 
CFLAGS = -O3

all: randtrack randtrack_global_lock randtrack_tm randtrack_list_lock randtrack_element_lock randtrack_reduction

randtrack: list.h hash.h defs.h randtrack.cc 
	$(CC) $(P) $(CFLAGS) randtrack.cc -o randtrack

randtrack_global_lock: list.h hash.h defs.h r1.cc 
	$(CC) $(P) $(CFLAGS) r1.cc -o randtrack_global_lock

randtrack_tm: list.h hash.h defs.h r2.cc 
	$(CC) $(TM) $(CFLAGS) r2.cc -o randtrack_tm

randtrack_list_lock: list.h hash2.h defs.h r3.cc 
	$(CC) $(P) $(CFLAGS) r3.cc -o randtrack_list_lock

randtrack_element_lock: list.h hash2.h defs.h r4.cc 
	$(CC) $(P) $(CFLAGS) r4.cc -o randtrack_element_lock

randtrack_reduction: list.h hash2.h defs.h r5.cc 
	$(CC) $(P) $(CFLAGS) r5.cc -o randtrack_reduction

clean:
	rm -f *.o randtrack randtrack_global_lock randtrack_tm randtrack_list_lock randtrack_element_lock randtrack_reduction
