JC= javac
FLAG = -d
DIR = ..
dir = ./Source/
dirtok = ./TokenBucketSource/

default:
	$(JC) $(FLAG) . $(dir)TimeBytes.java $(dir)ReadFile.java $(dir)WriteFile.java
	$(JC) $(FLAG) . $(dir)Sender.java $(dir)Receiver.java 
	
token: 
	$(JC) $(FLAG) . $(dirtok)Bucket.java 
	$(JC) $(FLAG) . $(dirtok)Buffer.java
	$(JC) $(FLAG) . $(dirtok)TokenBucketSender.java
	$(JC) $(FLAG) . $(dirtok)TokenBucketReceiver.java
	$(JC) $(FLAG) . $(dirtok)TokenBucket.java
	$(JC) $(FLAG) . $(dirtok)TBImplementation.java

clean:
	rm -r -f *.class
	rm -r -f ./TokenBucket/*.class
