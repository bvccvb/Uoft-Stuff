import java.io.*; 
import java.util.*; 

class ReadFile
{

	File file = null;
	FileReader fis = null;
	BufferedReader bis = null;
	
	public ReadFile(String dir)
	{
		try
		{
			file = new File(dir);
			fis = new FileReader(file);
			bis = new BufferedReader(fis);
		}
		catch(IOException e)
		{
			System.out.println("IOException: " + e.getMessage());
		}
	}

	//time is in micro sec
	Vector<TimeBytes> run() throws IOException
	{
		Vector<TimeBytes> time_bytes = new Vector<TimeBytes>();
		String currentLine;
		while((currentLine = bis.readLine()) != null)
		{
			StringTokenizer st = new StringTokenizer(currentLine);
			st.nextToken();
			String col2 = st.nextToken();
			String col3 = st.nextToken();

			Long time = Long.parseLong(col2);
			Integer bytes = Integer.parseInt(col3);

			time_bytes.add(new TimeBytes(time, bytes));
		}

		return time_bytes;
	}

	Vector<TimeBytes> run_2col() throws IOException
	{
		Vector<TimeBytes> time_bytes = new Vector<TimeBytes>();
		String currentLine;
		while((currentLine = bis.readLine()) != null)
		{
			StringTokenizer st = new StringTokenizer(currentLine);
			String col1 = st.nextToken();
			String col2 = st.nextToken();

			Long time = Long.parseLong(col1);
			Integer bytes = Integer.parseInt(col2);

			time_bytes.add(new TimeBytes(time, bytes));
		}

		return time_bytes;

	}

	//time is in millisec, SeqNo shows transmission sequence, but time shows Display sequence
	// but transmission of one frame to next frame is 33ms
	Vector<TimeBytes> run_moviedata() throws IOException
	{
		Vector<TimeBytes> time_bytes = new Vector<TimeBytes>();
		String currentLine;
		Long time = 0L;
		while((currentLine = bis.readLine()) != null)
		{
			StringTokenizer st = new StringTokenizer(currentLine);
			st.nextToken();
			st.nextToken();
			st.nextToken();
			String col4 = st.nextToken();

			Integer bytes = Integer.parseInt(col4);

			// MAX SIZE IP CAN CARRY UDP
			Integer MAX = (int)(Math.pow(2,16) - 29);
			while(bytes > MAX)
			{
				time_bytes.add(new TimeBytes(time, MAX));
				bytes -= MAX;
			}

			time_bytes.add(new TimeBytes(time, bytes));
			time += 33 * 1000; //ms converted to us
		}

		return time_bytes;
	}

	Vector<TimeBytes> run_ethernetdata() throws IOException
	{
		Vector<TimeBytes> time_bytes = new Vector<TimeBytes>();
		String currentLine;
		while((currentLine = bis.readLine()) != null)
		{
			StringTokenizer st = new StringTokenizer(currentLine);
			String col1 = st.nextToken();
			String col2 = st.nextToken();

			Double time = Double.parseDouble(col1) * 1000000; // seconds to us
			Integer bytes = Integer.parseInt(col2);

			time_bytes.add(new TimeBytes(time.longValue(), bytes));
		}

		return time_bytes;
	}
}