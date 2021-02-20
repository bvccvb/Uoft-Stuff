import java.io.*;
import java.net.*;
import java.util.*; 
import java.util.Vector;
import java.util.concurrent.TimeUnit ;


public class Estimator implements Runnable{

	InetAddress addr = null;
	DatagramSocket socket = null;
	Vector<Long> packets = null;
	byte[] buf = null;
	Integer returnPort = 50000;
	Integer n = 0;
	Integer bb_port = 4444;
	Long delay = 0l;


	Estimator(Integer n_, Integer l, Integer r, String address, Integer port)
	{	
		try
		{
			packets = new Vector<Long>(n);
			addr = InetAddress.getByName(address);
			n = n_;
			bb_port = port;

			socket = new DatagramSocket();
			buf = new byte[l/n_];
			System.arraycopy(toByteArray(returnPort), 2, buf, 0, 2);

			Long size = l/n_ * 8L * 1000000000L;
			Long rate_bps = r * 1000L;

			delay = (size/rate_bps); //in nanoseconds (ns)
			
			System.out.println("N = " + n + " (packets) L = " + l + " (bytes of train) r = " + r + " (rate of train in kbps)");
			System.out.println("Delay between packets (nanoseconds): " + delay);
			System.out.println();

		}
		catch(Exception e)
		{
			System.out.println("Init Error: " + e.getMessage());
		}
	}


	public void run()
	{
		try{
			Long send_time = System.nanoTime();
	    	Long current_time = send_time;
	    	Long last_sent = send_time;

			WriteFile fout = new WriteFile("send.txt");

			for(int i = 1; i <= n; ++i)
			{
				System.arraycopy(toByteArray(i), 0, buf, 2, 4);
				fout.print(i, System.nanoTime());
				socket.send(new DatagramPacket(buf, buf.length, addr, bb_port));

				send_time = last_sent + delay;
				do
				{
	        		current_time = System.nanoTime();
	      		} while(current_time < send_time);

	      		last_sent = current_time;

			}

			System.out.println("Finished Sending");

		}
		catch(Exception e)
		{
			System.out.println("Sending Error: " + e.getMessage());
		}

	}

	public static void main(String args[])
	{
		Estimator sender = new Estimator(Integer.parseInt(args[2]), Integer.parseInt(args[3]), Integer.parseInt(args[4]), args[0], Integer.parseInt(args[1]));



		//Receiver

		Integer n = Integer.parseInt(args[2]);
		Integer l = Integer.parseInt(args[3]);

		try{
			DatagramSocket socket = new DatagramSocket(Integer.parseInt("50000"));
			byte[] buf  = new byte[l/n];
			DatagramPacket p = new DatagramPacket(buf, buf.length);
			WriteFile fout = new WriteFile("rec.txt");

			new Thread(sender).start();

	   		for(int i = 0; i < n; ++i)
	   		{
	   			socket.receive(p);
	   			fout.print(fromByteArray(p.getData(), 2, 4), System.nanoTime());
	   		}

	   		System.out.println("Finished Receiving");
		}
		catch(Exception e)
		{
			System.out.println("Receiving Error: " + e.getMessage());	 
		}

	} 


	public static byte[] toByteArray(int value)
	{
		byte[] Result = new byte[4];
		Result[3] = (byte) ((value >>> (8*0)) & 0xFF);
		Result[2] = (byte) ((value >>> (8*1)) & 0xFF);
		Result[1] = (byte) ((value >>> (8*2)) & 0xFF);
		Result[0] = (byte) ((value >>> (8*3)) & 0xFF);
		return Result;
	}

	public static int fromByteArray(byte [] value, int start, int length)
	{
		int Return = 0;

		for (int i=start; i< start+length; i++)
		{
			Return = (Return << 8) + (value[i] & 0xff);
		}

		return Return;
	}
}

