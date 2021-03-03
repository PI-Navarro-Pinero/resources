import java.net.*;
import java.io.*;
import java.util.Scanner;

public class CommandClient {

    public static void main(String argv[]) {

        String direccionIP = "127.0.0.1";
        int puerto = 7777;

        Socket socket;

        String cmd = "";
        String linea = "";

        Scanner in;

        try {
            socket = new Socket(direccionIP, puerto);

            System.out.println("Conectado exitosamente a " + direccionIP);

            while (!cmd.equals("quit")) {
                System.out.print(">> ");
                in = new Scanner(System.in);
                cmd = in.nextLine();

                PrintWriter toConsole = new PrintWriter(socket.getOutputStream(), true);

                InputStreamReader input = new InputStreamReader(socket.getInputStream());
                BufferedReader fromConsole = new BufferedReader(input);

                toConsole.println(cmd);

                linea = fromConsole.readLine();

                System.out.println(linea);
            }
        } catch (UnknownHostException e) {
            System.out.println(e.getMessage());
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
    }
}
