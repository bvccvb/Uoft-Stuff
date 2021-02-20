import java.io.*;
import java.net.*;
import java.util.*; 
import java.util.Vector;
import java.util.concurrent.TimeUnit ;

// java Sender <IP> <PORT> <Period> <NUM of Packets> <Bytes per packet>

public class Sender {

  public static void main(String[] args) throws IOException {
    InetAddress addr = InetAddress.getByName(args[0]);
    DatagramSocket socket = new DatagramSocket();

    Long period;
    Integer num_pckt, size_pckt;

    // T = period(ms), N number of packets, L size of each packet(bytes)

    period = Long.parseLong(args[2]);
    num_pckt = Integer.parseInt(args[3]);
    size_pckt = Integer.parseInt(args[4]);

    String dir = "T" +args[2] + "N" + args[3] + "L" + args[4] + ".data";

    // CREATE PERIODIC TRACE
    WriteFile fout = new WriteFile(dir);
    fout.create_trace(period, num_pckt, size_pckt);
    fout.close();

  	Vector<TimeBytes> time_bytes = new Vector<TimeBytes>();
  	ReadFile fin = new ReadFile(dir);

    time_bytes = fin.run_2col();


  	Vector<DatagramPacket> packets = new Vector<DatagramPacket>();
    Vector<Long> v_delays = new Vector<Long>();

  	for(int i = 0; i < time_bytes.size(); ++i)
  	{
  	   byte[] buf  = new byte[time_bytes.get(i).bytes];
  	   packets.add(new DatagramPacket(buf, buf.length, addr, Integer.parseInt(args[1])));
       if(i != time_bytes.size() - 1)
        v_delays.add((time_bytes.get(i + 1).time - time_bytes.get(i).time) * 1000000);
  	}

    Long delay;
    Long send_time = System.nanoTime();
    Long current_time = System.nanoTime();

    for(int i = 0; i < time_bytes.size() - 1; ++i)
    {
      socket.send(packets.get(i));
      if(v_delays.get(i) == 0)
        continue;

      send_time += v_delays.get(i);

      do
      {
        current_time = System.nanoTime();
      }while(current_time < send_time);

    }

    socket.send(packets.get(time_bytes.size()-1));

    System.out.println("DONE");
  }
}

