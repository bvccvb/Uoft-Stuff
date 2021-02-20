import java.io.*;
import java.net.*;
public class Receiver {
  public static void main(String[] args) throws IOException {
    DatagramSocket socket = new DatagramSocket(Integer.parseInt(args[0]));
    byte[] buf = new byte[(int)(Math.pow(2,16) - 1)]; //max UDP packet

    WriteFile fout = new WriteFile("receiver.txt");
    DatagramPacket p = new DatagramPacket(buf, buf.length);



    Long prev_time = 0l;
    Long rec_time = 0l;

   	while(true)
   	{
   		socket.receive(p);
   		rec_time = System.nanoTime();

   		if(prev_time == 0)
   			prev_time = rec_time;
   		fout.time_bytes(((rec_time - prev_time)/1000), p.getLength());
   		prev_time = rec_time;
   	}

  }
}
