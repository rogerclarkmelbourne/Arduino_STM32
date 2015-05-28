import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.text.SimpleDateFormat;
import java.util.Date;

public class UDPserver {
	public static void main(String[] args) {
		UDPserver server = new UDPserver(1234);
		server.start();
	}
	private int port;
	private DatagramSocket s;
	
	SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

	public UDPserver(int port) {
		this.port = port;
	}

	public void start() {
		System.out.println("SERVER: Waiting for incomming connections...");
		System.out.println("DATE TIME IP:PORT   received_data");
		try {
			s = new DatagramSocket(this.port);
			while (true) {
				byte[] data = new byte[1412];
				DatagramPacket p = new DatagramPacket(data,
						data.length);
				s.receive(p);
				System.out.print(sdf.format(new Date()).toString() + " ");
				System.out.print(p.getSocketAddress() + "   ");
				System.out.println(new String(p.getData()));
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}