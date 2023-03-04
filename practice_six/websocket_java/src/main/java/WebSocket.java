import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
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

                while (true) {
                    String clientMes = this.decryptInput(in);
                    System.out.println("Received message from client: " + clientMes);

                    //Construct a response

                    String returnMes = "Message received";
                    this.sendResponse(out, returnMes);


                }

            }


        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
    }

    private void sendResponse(OutputStream out, String mes) throws IOException{
        byte[] responsePayload = returnMes.getBytes(StandardCharsets.UTF_8);
        byte[] responseHeader = new byte[2];
        responseHeader[0] = (byte) (0x80 | opcode);
        responseHeader[1] = (byte) responsePayload.length;
        out.write(responseHeader);
        out.write(responsePayload);
    }

    private String decryptInput(InputStream in) throws IOException {
        byte[] frameHeader = new byte[2];
        in.read(frameHeader);

        //We read the first parts of the message frame. The opcode is the message type, fin indicates if this is final fragment in a message.
        //masked indicates if the payload data is masked or not.

        byte opcode = (byte)(frameHeader[0] & 0x0F);
        boolean fin = (frameHeader[1] & 0x80) != 0;
        boolean masked = (frameHeader[1] & 0x80) != 0;
        int payloadLength = (byte)(frameHeader[1] & 0x7F);

        //Determine payload length

        if(payloadLength == 126){
            //We extract whether or not there is another extension of payload.
            byte[] extendedLen = new byte[2];
            in.read(extendedLen);
            payloadLength = ByteBuffer.wrap(extendedLen).getShort();
        }
        else if(payloadLength == 127){
            byte[] extendedLen = new byte[8];
            in.read(extendedLen);
            payloadLength = (int) ByteBuffer.wrap(extendedLen).getLong();
        }

        //Read mask if masked.
        byte[] mask = new byte[4];
        if(masked){
            in.read(mask);
        }

        //Read the actual payload
        byte[] payload = new byte[payloadLength];

        in.read(payload);

        //Use the mask and a XOR operation to demask the payload.
        if(masked) {
            for (int i = 0; i < payloadLength; i++){
                payload[i] ^= mask[i % 4];
            }
        }

        String message = new String(payload, StandardCharsets.UTF_8);
        return message;
    }

        public static void main(String[] args) throws IOException {
        WebSocket ws = new WebSocket();
        ws.start();
    }
}
