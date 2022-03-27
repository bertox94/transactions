package com.example.demo;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class SpaceTime_Gap {

    public static String read() {

        return null;
    }


    public static String send(String msg) {
        Socket clientSocket;
        PrintWriter out;
        BufferedReader in;
        String resp = "KO";
        try {
            clientSocket = new Socket("localhost", 27017);
            out = new PrintWriter(clientSocket.getOutputStream(), false);
            in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

            write(in, out, msg);
            resp = read(in, out);

            in.close();
            out.close();
            clientSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return resp;
    }

    private static void raw_write(PrintWriter out, String msg) {
        out.print(msg);
        out.flush();
    }

    private static String raw_read(BufferedReader in, int howmuch) throws IOException {
        char[] buf = new char[howmuch];
        int err = in.read(buf, 0, howmuch);
        return String.valueOf(buf, 0, howmuch);
    }

    private static void write(BufferedReader in, PrintWriter out, String msg) throws IOException {
        raw_write(out, String.format("%010d", msg.length()));
        raw_read(in, 2);
        raw_write(out, msg);
    }

    private static String read(BufferedReader in, PrintWriter out) throws IOException {
        String len = raw_read(in, 10);
        raw_write(out, "OK");
        return raw_read(in, Integer.parseInt(len));
    }
}
