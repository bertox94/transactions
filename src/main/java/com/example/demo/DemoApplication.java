package com.example.demo;

import org.postgresql.Driver;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.io.*;
import java.net.Socket;
import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;
import java.nio.channels.OverlappingFileLockException;
import java.nio.file.Files;
import java.nio.file.NoSuchFileException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

@SpringBootApplication
public class DemoApplication {

    public static void main(String[] args) throws Exception {
        try {
            Files.delete(Paths.get("processor\\main.exe"));
        } catch (Exception ignored) {
        }

        try {
            Files.delete(Paths.get("main.obj"));
        } catch (Exception ignored) {
        }
/*
        String[] command = {"cmd.exe", "/C", "Start /B", "E:\\workspace\\webapp-transaction-scheduler\\processor\\build.bat"};
        Runtime.getRuntime().exec(command);
        while (Files.notExists(Paths.get("processor\\main.exe"))) {
        }
        Thread.sleep(500);
        Runtime.getRuntime().exec("E:\\workspace\\webapp-transaction-scheduler\\processor\\main.exe", null, new File("E:\\workspace\\webapp-transaction-scheduler\\processor"));
        Thread.sleep(500);
*/
        try {
            String url = "jdbc:postgresql://otto.db.elephantsql.com/olkpwuzc";
            Properties props = new Properties();
            props.setProperty("user", "olkpwuzc");
            props.setProperty("password", "spguWftorvG2mqQgEZ55ySvwWlld_5AS");
            props.setProperty("ssl", "false");
            MainController.connection = DriverManager.getConnection(url, props);
        } catch (SQLException throwables) {
            throwables.printStackTrace();
        }

        SpringApplication.run(DemoApplication.class, args);


        System.out.println("Initialization completed.");

    }
}

