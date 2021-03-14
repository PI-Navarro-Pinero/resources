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
              //Muestra prompt y espera el input del usuario
              System.out.print("--) ");
              in = new Scanner(System.in);
              cmd = in.nextLine();

              //Para escribir en el socket (enviar comando)
              PrintWriter toConsole = new PrintWriter(socket.getOutputStream(), true);

              //Para leer el socket
              InputStreamReader input = new InputStreamReader(socket.getInputStream());
              BufferedReader fromConsole = new BufferedReader(input);

              //Envía el comando
              toConsole.println(cmd);

              //Espera una respuesta (que el resultado del comando se escriba
              //en el socket)
              while(!fromConsole.ready());

              //Mientras se pueda leer, que lo imprima
              while(fromConsole.ready())
              {
                linea = fromConsole.readLine();
                System.out.println(linea);
              }
            }
        } catch (UnknownHostException e) {
            System.out.println(e.getMessage());
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
    }
}
