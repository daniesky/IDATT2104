import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Websocket class created for running a websocket server. Formed after the RFC 6455 form, therefore only support Chrome version 16, Firefox 11, IE 10 and higher.
 * @author Daniel Skymoen
 */
public class WebSocket {

    public void start() throws IOException {
        ServerSocket server = new ServerSocket(3001);
        System.out.println("Established a server socket on 127.0.0.1:3001. \n Waiting for connection");
        try {
            Socket client = server.accept();
            System.out.println("A client has connected to the socket");

            //Retrieve streams from the new client socket. This is the streams where we can retrieve/send data.

            InputStream in = client.getInputStream();
            OutputStream out = client.getOutputStream();
            Scanner sc = new Scanner(in, "UTF-8");

            //Retrieve the initial message from client and verifying that it matches the pattern of a GET request.
            String data = sc.useDelimiter("\\r\\n\\r\\n").next();
            System.out.println("Following request received:");
            System.out.println(data);
            Matcher get = Pattern.compile("^GET").matcher(data);

            //Code only runs if the pattern matches get request.
            if(get.find()) {

                //Retrieve the websocket key header data.
                Matcher match = Pattern.compile("Sec-WebSocket-Key: (.*)").matcher(data);
                match.find();

                //We create the HTTP response that we will send to the client. We format it according to RFC6455.
                byte[] response = (
                        "HTTP/1.1 101 Switching Protocols\r\n"
                                + "Connection: Upgrade\r\n"
                                + "Upgrade: websocket\r\n"
                                + "Sec-WebSocket-Accept: "
                                + Base64.getEncoder().encodeToString(MessageDigest.getInstance("SHA-1")
                                .digest((match.group(1) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11").getBytes("UTF-8")))
                                + "\r\n\r\n").getBytes("UTF-8");

                //Write the response to the output stream.
                System.out.println("Responding with:");
                System.out.println(new String(response, StandardCharsets.UTF_8));
                out.write(response, 0, response.length);

                //We should now have a completed handshake and we can start transferring data.
            }



        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) throws IOException {
        WebSocket ws = new WebSocket();
        ws.start();
    }
}
