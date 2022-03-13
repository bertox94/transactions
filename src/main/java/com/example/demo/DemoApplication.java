package com.example.demo;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.io.File;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

@SpringBootApplication
public class DemoApplication {

    public static void main(String[] args) {

        //initialize_processor();
        initialize_DB_connection();
        SpringApplication.run(DemoApplication.class, args);

        System.out.println("Initialization completed.");

    }

    public static boolean initialize_processor() {
        try {
            Files.delete(Paths.get("processor\\main.exe"));
        } catch (Exception ignored) {
            //no problem
        }

        try {
            Files.delete(Paths.get("main.obj"));
        } catch (Exception ignored) {
            //no problem
        }

        try {
            String[] command = {"cmd.exe", "/C", "Start /B", "E:\\workspace\\webapp-transaction-scheduler\\processor\\build.bat"};
            Runtime.getRuntime().exec(command);
            while (Files.notExists(Paths.get("processor\\main.exe"))) {
            }
            Thread.sleep(500);
            Runtime.getRuntime().exec("E:\\workspace\\webapp-transaction-scheduler\\processor\\main.exe", null, new File("E:\\workspace\\webapp-transaction-scheduler\\processor"));
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public static boolean initialize_DB_connection() {
        try {
            String url = "jdbc:postgresql://otto.db.elephantsql.com/olkpwuzc";
            Properties props = new Properties();
            props.setProperty("user", "olkpwuzc");
            props.setProperty("password", "spguWftorvG2mqQgEZ55ySvwWlld_5AS");
            props.setProperty("ssl", "false");
            MainController.connection = DriverManager.getConnection(url, props);
        } catch (SQLException throwables) {
            throwables.printStackTrace();
            return false;
        }
        return true;
    }

}

