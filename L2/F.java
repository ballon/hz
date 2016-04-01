import java.io.*;
import java.net.*;
import java.io.IOException;

public class F{
    public static void main(String[]args) throws IOException, InterruptedException {
        Thread.sleep(10000);
        boolean ans = false;

        Socket s = new Socket("127.0.0.1", 12345);
        PrintWriter out = new PrintWriter(s.getOutputStream(), true);
        out.println(ans+"\n");
        s.close();
    }
}
