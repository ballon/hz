import java.io.IOException;
import java.net.*;
import java.io.*;

public class Main {
    public static int val_f = -1, val_g = -1;

    public static void speak(ServerSocket serverSocket, boolean is_f) throws IOException, InterruptedException { 
        Socket clientSocket = null;
        try {
            clientSocket = serverSocket.accept();
        } catch (SocketTimeoutException e) {
            System.out.println(e.getMessage());
        }

        if(clientSocket==null) return;

        BufferedReader in = new BufferedReader(
                    new InputStreamReader(clientSocket.getInputStream()));
        String response = in.readLine();
        if(response != null) {
            System.out.print("We got a value of: ");
            String what = (is_f ? "F" : "G");
            System.out.println(what+" = " + response);
            int val = -1;
            if(response.equals("true")) val = 1; else
                if(response.equals("false")) val = 0; else
                    System.out.println("why god, why?");
            if(val >= 0)
            if(is_f) val_f = val;else val_g = val;
        }
    }

    public static void main(String[] args) throws IOException, InterruptedException {
        int mxwait = 30;
        ServerSocket socket_f = new ServerSocket(12345);
        socket_f.setSoTimeout(mxwait);
        ServerSocket socket_g = new ServerSocket(12346);
        socket_g.setSoTimeout(mxwait);

        System.out.println("ServerSockets are created");

        Process process_f = Runtime.getRuntime().exec("javac F.java");
        process_f.waitFor();
        
        Process process_g = Runtime.getRuntime().exec("javac G.java");
        process_g.waitFor();

        Process F = Runtime.getRuntime().exec("java F");
        Process G = Runtime.getRuntime().exec("java G");

        System.out.println("Processes for F, G are created");
        
        int ans = -1;

        for(;;) {
            System.out.println("Checking...");
            speak(socket_f, true);
            speak(socket_g, false);

            if(val_f==1 || val_g==1) {
                ans = 1;
                break;
            }

            if(val_f >= 0 && val_g >= 0) {
                ans = (val_f | val_g);
                break;
            }

            System.out.println(" "+val_f+" "+val_g);
            System.out.println("Bad news!. Press ctrt+c ctrt+z if no longer wish to continue\n");
            Thread.sleep(5000);
        }

        socket_f.close();
        socket_g.close();
        System.out.println("We got it: " + ans);
    }
}
