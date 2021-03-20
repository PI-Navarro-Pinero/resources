import java.net.*;
import java.io.*;
import java.util.Scanner;

public class CommandClient {

    private static final String SHELL_DIRECTORY = "../C/myshell";

    public static void main(String argv[]) {

      try
      {
        ProcessBuilder bash = new ProcessBuilder(SHELL_DIRECTORY);

        bash.inheritIO();
        bash.redirectErrorStream(true);

        Process bashProcess = bash.start();

        bashProcess.waitFor();

        System.out.println("\nGoodbye!");
      }
      catch (IOException e) { e.printStackTrace(); }
      catch (InterruptedException e) { e.printStackTrace(); }
    }
}
