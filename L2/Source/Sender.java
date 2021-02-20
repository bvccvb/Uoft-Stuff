import java.io.*;
import java.net.*;
import java.util.*; 
import java.util.Vector;
import java.util.concurrent.TimeUnit ;

public class Sender {

  public static void main(String[] args) throws IOException {
    InetAddress addr = InetAddress.getByName(args[0]);
    DatagramSocket socket = new DatagramSocket();


  	Vector<TimeBytes> time_bytes = new Vector<TimeBytes>();
    String dir = "ethernet.data";
  	ReadFile fin = new ReadFile(dir);

    if(dir.equals("poisson3.data"))
  	 time_bytes = fin.run();
    else if(dir.equals("movietrace.data")){
      time_bytes = fin.run_moviedata();
    }
    else if(dir.equals("ethernet.data"))
      time_bytes = fin.run_ethernetdata();


  	Vector<DatagramPacket> packets = new Vector<DatagramPacket>();
  	for(int i = 0; i < time_bytes.size(); ++i)
  	{
  	   byte[] buf  = new byte[time_bytes.get(i).bytes];
  	   packets.add(new DatagramPacket(buf, buf.length, addr, Integer.parseInt(args[1])));
        System.out.println("Packet size: " + packets.get(i).getLength());
  	}

    Long delay;
    Long send_time = System.nanoTime();
    Long current_time = System.nanoTime();

    for(int i = 0; i < time_bytes.size() - 1; ++i)
    {
      socket.send(packets.get(i));
      delay = (Long) (time_bytes.get(i + 1).time - time_bytes.get(i).time);
      System.out.println(delay);
      send_time += delay * 1000;

      do
      {
        current_time = System.nanoTime();
      }while(current_time < send_time);

    }

    socket.send(packets.get(time_bytes.size()-1));
  }
}

