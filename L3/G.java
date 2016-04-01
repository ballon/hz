import java.io.*;
import java.net.*;
import java.io.IOException;

public class G{
    public static void main(String[]args) throws IOException, InterruptedException {
        Thread.sleep(25000);
        boolean ans = false;

        Socket s = new Socket("127.0.0.1", 12346);
        PrintWriter out = new PrintWriter(s.getOutputStream(), true);
        out.println(ans+"\n");
        s.close();
    }
}
