/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package envyfileserver.net;

import envyfileserver.net.EnvyNetMessageProtos.EnvyNetMessage;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;

/**
 *
 * @author jithornton47
 */
public class EnvyFTPServer {

    private EnvyProtocol currentProtocol;

    public EnvyFTPServer() {
        this.currentProtocol = new EnvyAP();
    }

    private class EnvyFTPServerConnection extends Thread {

        protected Socket socket;
        private EnvyProtocol currentProtocol;

        public EnvyFTPServerConnection(Socket cs) {
            this.socket = cs;
            this.currentProtocol = new EnvyAP();
        }

        void WriteData(DataOutputStream out, EnvyNetMessage msg, boolean verbose) throws IOException {
            msg.writeTo(out);
            out.writeByte('\n');
            if (verbose) {
                System.out.print("Sending: ");
                byte[] dat = msg.toByteArray();
                for (int i = 0; i < dat.length; i++) {
                    System.out.print(dat[i] + " ");
                }
                System.out.println("to " + this.socket.getInetAddress().getHostAddress());
            }
            /*
            out.writeInt(data.length);
            out.write(data);
            if (verbose) {
                System.out.print("Sending: ");
                byte[] sizedata = ByteBuffer.allocate(4).putInt(data.length).array();
                for(int i = 0; i < sizedata.length; i++) {
                    System.out.print(sizedata[i] + " ");
                }
                for (int i = 0; i < data.length; i++) {
                    System.out.print(data[i] + " ");
                }
                System.out.println("to " + this.socket.getInetAddress().getHostAddress());
            }
             */
        }

        @Override
        public void run() {
            try {
                DataOutputStream out = new DataOutputStream(socket.getOutputStream());
                BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                String input;
                EnvyNetMessage toWrite = currentProtocol.processInput(null);
                WriteData(out, toWrite, true);
                while ((input = in.readLine()) != null) {
                    System.out.println("RECEIVED: " + input);
                    toWrite = currentProtocol.processInput(input);
                    if (this.currentProtocol.shouldTerminate()) {
                        WriteData(out, toWrite, true);
                        WriteData(out, currentProtocol.requestTerminate(), true);
                        break;
                    } else {
                        WriteData(out, toWrite, true);
                    }
                }
            } catch (IOException ex) {
                System.out.println("A Client suddently dropped connection!");
                System.out.println(ex.getMessage());
            }
        }
    }

    public void start() {
        ServerSocket ss = null;
        try {
            ss = new ServerSocket(currentProtocol.port());
        } catch (IOException e) {
            System.out.println("Exception caught when trying to listen on port "
                    + currentProtocol.port() + " or listening for a connection");
            System.out.println(e.getMessage());
        }
        while (true) {
            try {
                if (ss != null) {
                    Socket cs = ss.accept();
                    System.out.println("ACCEPTING NEW CONNECTION AT: " + cs.getInetAddress().getHostName());
                    new EnvyFTPServerConnection(cs).start();
                }
            } catch (IOException ex) {
                System.out.println("SERVER I/O ERROR: " + ex);
            }
        }
    }
}
