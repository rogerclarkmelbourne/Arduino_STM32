import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.UnknownHostException;

public class UDPClient {

	public static void main(String[] args) throws UnknownHostException {
		DatagramSocket s;
		byte[] sendBuffer = new byte[1024];
		DatagramPacket sendPacket;
		final InetAddress ADDRESS = InetAddress.getByName("localhost");
		final int PORT = 1234;
		try {
			s = new DatagramSocket();
			System.out.println("Odesilam data na server...");

			sendBuffer = "abc123".getBytes();
			sendPacket = new DatagramPacket(sendBuffer, sendBuffer.length,
					ADDRESS, PORT);
			s.send(sendPacket);

		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}