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
import java.sql.*;
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
            String[] command = {"cmd.exe", "/C", "Start /B", "processor\\build.bat"};
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


            String jdbcURL = "jdbc:h2:~/test";
            String username = "sa";
            String password = "1234";

            MainController.connection = DriverManager.getConnection(jdbcURL, username, password);

/*            System.out.println("Connected to H2 embedded database.");


            String sql = "Create table students (ID int primary key, name varchar(50))";
            Statement statement = connection.createStatement();
            statement.execute(sql);

            System.out.println("Created table students.");

            sql = "Insert into students (ID, name) values (1, 'Nam Ha Minh')";

            int rows = statement.executeUpdate(sql);

            if (rows > 0) {
                System.out.println("Inserted a new row.");
            }


            sql = "SELECT * FROM students";

            statement = connection.createStatement();
            ResultSet resultSet = statement.executeQuery(sql);

            int count = 0;

            while (resultSet.next()) {
                count++;

                int ID = resultSet.getInt("ID");
                String name = resultSet.getString("name");
                System.out.println("Student #" + count + ": " + ID + ", " + name);
            }

            //connection.close();


            String url = "jdbc:postgresql://localhost/postgres";
            Properties props = new Properties();
            props.setProperty("user", "postgres");
            props.setProperty("password", "admin");
            props.setProperty("ssl", "false");
            MainController.connection = DriverManager.getConnection(url, props);

 */
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

