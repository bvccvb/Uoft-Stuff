import java.io.*;
import java.util.*;


public class WriteFile
{

	PrintStream pout = null;
	FileOutputStream fout = null;

	public WriteFile(String dir)
	{
		try
		{
			fout = new FileOutputStream(dir);
			pout = new PrintStream (fout);		
		}
		catch(IOException e)
		{
			System.out.println("IOException: " + e.getMessage());
		}

	}

	void create_trace(Long period, Integer num_pckt, Integer size_pckt)
	{
		Integer count = 0;
    	Long time = 0L;
		Integer data_pts = 10000;

		while(count <= data_pts)
		{
			for(Integer i = 0; i < num_pckt; ++i){
				this.time_bytes(time, size_pckt);
				count ++;
			}

			time += period;
		}
	}


	void time_bytes(Long time, Integer bytes){pout.println(time + "\t" + bytes);}
	void close(){pout.close();}
}