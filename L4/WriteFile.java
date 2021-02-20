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

	void print(Integer seqNo, Long time){pout.println(seqNo + "\t" + time);}

	void close(){pout.close();}
}