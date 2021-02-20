import TokenBucket.TokenBucket;
import java.io.*; 
import java.util.*; 


public class TBImplementation{

	public static void main(String[] args)
	{
		/* Lab2b
			1. Bucket size = 100
			2. Bucket size = 500
			3. Bucket size = 100
		*/

		// Portin , Dest IP, Portout, maxPacketsize, bufferCap, bucketSize (bytes), bucketRate (tokens / sec), fileName
		TokenBucket tb = new TokenBucket(Integer.parseInt(args[0]), "localhost" , Integer.parseInt(args[1]), 
			(int) (Math.pow(2,16) - 1), 100*1024, 1518, (int) (1 *1000000000/8), "../L3/MatlabE3/bucket.txt");

		// Max packet size TB should be changed for video
		new Thread(tb).start();
	}
}
