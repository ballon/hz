import java.io.IOException;
import java.net.*;
import java.io.*;

public class Main implements Runnable {
    public static int val = 0;
    Socket clientSocket;

    Main(Socket s) {
        this.clientSocket = s;
    }

    public static void main(String[] args) throws IOException, InterruptedException {
        ServerSocket serverSocket = new ServerSocket(12345);
        serverSocket.setSoTimeout(1000);

        System.out.println("ServerSocket are created");

        Process process_f = Runtime.getRuntime().exec("javac F.java");
        process_f.waitFor();
        
        final int cnt_proc = 3;
        for(int i=0; i<cnt_proc; ++i) {
            Process F = Runtime.getRuntime().exec("java F");
        }
        System.out.println("Two processes for increase are created");

        for(int i=0; i<1000*2*cnt_proc; ++i) {
            Socket clientSocket = null;
            while(clientSocket == null) {
                try {
                    clientSocket = serverSocket.accept();
                } catch (SocketTimeoutException e) {
                    System.out.println(e.getMessage());
                }
            }

            System.out.println("Got a connectoin");

            new Thread( new Main(clientSocket)).start();
        }

        serverSocket.close();
        System.out.println("We got it: " + val);
    }

    public void run() {
        try {
            BufferedReader in = new BufferedReader(
                                    new InputStreamReader(clientSocket.getInputStream()));

            PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true); 

            try {
                String response = in.readLine();
                int mode = Integer.parseInt(response);
                System.out.println(mode);
                if(mode >= 0) {
                    val = mode;
                } else {
                    out.println(val);
                }
            } catch (IOException e) {
                System.out.println("probme1");
            }
            out.close();
            in.close();
            clientSocket.close();
        } catch (IOException e) {
            System.out.println("problem2");
        }
    }
}
