package com.example.demo;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class SpaceTime_Gap {
    public static String send(String msg) {
        Socket clientSocket;
        PrintWriter out;
        BufferedReader in;
        String resp = "KO";
        try {
            clientSocket = new Socket("localhost", 27017);
            out = new PrintWriter(clientSocket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            out.println(msg);
            resp = in.readLine();
            in.close();
            out.close();
            clientSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return resp;
    }
}
