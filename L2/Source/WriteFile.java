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


	void time_bytes(Long time, Integer bytes){pout.println(time + "\t" + bytes);}
	void close(){pout.close();}
}