import TokenBucket.TokenBucket;
import java.io.*; 
import java.util.*; 

public class TBImplementation{

	public static void main(String[] args)
	{
		// Portin , Dest IP, Portout, maxPacketsize, bufferCap, bucketSize, bucketRate, fileName
		TokenBucket tb = new TokenBucket(50008, "localhost" ,50009, (int) (Math.pow(2,16) - 1), 100*1024, 10000, 5000, "bucket.txt");

		// Max packet size TB should be changed for video
		new Thread(tb).start();
	}
}