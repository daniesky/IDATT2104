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
                out.flush();

                Thread readThread = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        while(true){
                            try {
                                String received = decryptInput(in);
                                if(!received.isBlank() && !received.isEmpty()){
                                    System.out.println("Message received from client: " + received);
                                }
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }
                );
                readThread.start();
                while (true) {
                    Scanner readServer = new Scanner(System.in);
                    String mes = readServer.nextLine();
                    if(!mes.isEmpty() && !mes.isBlank()){
                        this.sendWebSocketMessage(mes, out);
                    }

                }

            }


        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
    }

    /*
    public void sendResponse(OutputStream out, String mes) throws IOException{
        byte[] responsePayload = mes.getBytes(StandardCharsets.UTF_8);
        byte[] responseHeader = new byte[2];
        responseHeader[0] = (byte) (0x80 | opcode);
        responseHeader[1] = (byte) responsePayload.length;
        out.write(responseHeader);
        out.write(responsePayload);
    }

     */



    public void sendWebSocketMessage(String message, OutputStream outputStream) throws IOException {
        byte[] messageBytes = message.getBytes(StandardCharsets.UTF_8);
        byte[] header = new byte[10];
        int newSize = 0;

        // Set FIN bit and opcode
        header[0] = (byte) 0x81;

        int messageLength = messageBytes.length;

        // Set payload length bytes
        if (messageLength <= 125) {
            header[1] = (byte) messageLength;
            newSize = 2;
        } else if (messageLength <= 65535) {
            header[1] = (byte) 126;
            header[2] = (byte) ((messageLength >> 8) & 0xFF);
            header[3] = (byte) (messageLength & 0xFF);
            newSize = 4;
        } else {
            header[1] = (byte) 127;
            header[2] = (byte) ((messageLength >> 56) & 0xFF);
            header[3] = (byte) ((messageLength >> 48) & 0xFF);
            header[4] = (byte) ((messageLength >> 40) & 0xFF);
            header[5] = (byte) ((messageLength >> 32) & 0xFF);
            header[6] = (byte) ((messageLength >> 24) & 0xFF);
            header[7] = (byte) ((messageLength >> 16) & 0xFF);
            header[8] = (byte) ((messageLength >> 8) & 0xFF);
            header[9] = (byte) (messageLength & 0xFF);
            newSize = 10;
        }
        byte[] finalHeader = new byte[newSize];
        System.arraycopy(header, 0, finalHeader, 0, newSize);
        // Combine header and message into a single byte array
        byte[] messageWithHeader = new byte[finalHeader.length + messageBytes.length];
        System.arraycopy(finalHeader, 0, messageWithHeader, 0, finalHeader.length);
        System.arraycopy(messageBytes, 0, messageWithHeader, finalHeader.length, messageBytes.length);

        // Send combined byte array
        outputStream.write(messageWithHeader);
        outputStream.flush();
    }


    public String decryptInput(InputStream in) throws IOException {
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
