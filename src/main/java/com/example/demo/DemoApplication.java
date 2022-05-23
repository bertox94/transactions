package com.example.demo;

import org.slf4j.LoggerFactory;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import javax.annotation.PreDestroy;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

@SpringBootApplication
public class DemoApplication {

    public static void main(String[] args) {

        initialize_processor();
        initialize_DB_connection();
        SpringApplication.run(DemoApplication.class, args);
        LoggerFactory.getLogger(DemoApplication.class).info("Database online.");
        LoggerFactory.getLogger(DemoApplication.class).info("Initialization completed.");

    }

    public static boolean initialize_processor() {
        Path path = Paths.get("processor\\main.exe");
        try {
            Files.delete(path);
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
            while (Files.notExists(path)) {
                //pass
            }
            Thread.sleep(500);
            Runtime.getRuntime().exec("E:\\workspace\\webapp-transaction-scheduler\\processor\\main.exe", null, new File("E:\\workspace\\webapp-transaction-scheduler\\processor"));
            LoggerFactory.getLogger(DemoApplication.class).info("Processor started.");
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public static boolean initialize_DB_connection() {
        try {
            String url = "jdbc:postgresql://localhost/postgres";
            Properties props = new Properties();
            props.setProperty("user", "postgres");
            props.setProperty("password", "admin");
            props.setProperty("ssl", "false");
            MainController.connection = DriverManager.getConnection(url, props);
        } catch (SQLException throwables) {
            throwables.printStackTrace();
            return false;
        }
        return true;
    }

    @PreDestroy
    public void destroy() throws IOException {
        Runtime.getRuntime().exec("taskkill /F /IM main.exe");
        LoggerFactory.getLogger(DemoApplication.class).info("Processor halted.");
    }

}

