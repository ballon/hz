import java.io.*;
import java.net.*;
import java.io.IOException;

public class F{
    public static void main(String[]args) throws IOException, InterruptedException {
        for(int iter=1; iter <= 1000; ++iter) {
            System.out.println("HELLO");

            //ask for a value
            Socket s1 = new Socket("127.0.0.1", 12345);
            PrintWriter out = new PrintWriter(s1.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(s1.getInputStream()));
            out.println(-1);
            String response = in.readLine();
            s1.close();
            out.close();
            in.close();


            //increase by 1
            int val = Integer.parseInt(response);
            val++;


            // set value
            Socket s2 = new Socket("127.0.0.1", 12345);
            out = new PrintWriter(s2.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(s2.getInputStream()));
            out.println(val);
            s2.close();
            out.close();
            in.close();
        }
    }
}
