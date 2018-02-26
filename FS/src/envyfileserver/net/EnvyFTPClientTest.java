/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package envyfileserver.net;

/**
 *
 * @author jithornton47
 */
import java.io.*;
import java.net.*;

public class EnvyFTPClientTest {

    public static void main(String[] args) throws IOException {

        /*
        if (args.length != 2) {
            System.err.println(
                    "Usage: java EchoClient <host name> <port number>");
            System.exit(1);
        }
         */
        String hostName = "173.230.153.123";
        int portNumber = Integer.parseInt("2356");

        try (
                Socket kkSocket = new Socket(hostName, portNumber);
                PrintWriter out = new PrintWriter(kkSocket.getOutputStream(), true);
                DataInputStream in = new DataInputStream(kkSocket.getInputStream());) {
            BufferedReader stdIn
                    = new BufferedReader(new InputStreamReader(System.in));
            byte[] fromServer;
            String fromUser;
            int len;

            while ((len = in.readInt()) != 0) {
                fromServer = new byte[len];
                in.readFully(fromServer, 0, fromServer.length);
                String s = new String(fromServer, "UTF-8");
                System.out.print(s);
                if (s.equals("Goodbye.")) {
                    System.out.println();
                    break;
                }
                boolean poll = !(s.contains("Welcome"));
                if (poll) {
                    fromUser = stdIn.readLine();
                    if (fromUser != null) {
                        out.println(fromUser);
                    }
                } else {
                    out.println("");
                }
            }
        } catch (UnknownHostException e) {
            System.err.println("Don't know about host " + hostName);
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Couldn't get I/O for the connection to "
                    + hostName);
            System.exit(1);
        }
    }
}
