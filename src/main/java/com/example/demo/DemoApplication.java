package com.example.demo;

import org.slf4j.LoggerFactory;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.sql.*;

import static com.example.demo.MainController.connection;

@SpringBootApplication
public class DemoApplication {

    public static void main(String[] args) {

        initialize_DB_connection();
        SpringApplication.run(DemoApplication.class, args);
        LoggerFactory.getLogger(DemoApplication.class).info("Database online.");
        LoggerFactory.getLogger(DemoApplication.class).info("Initialization completed.");

    }

    public static boolean initialize_DB_connection() {
        try {


            String jdbcURL = "jdbc:h2:./h2";
            String username = "sa";
            String password = "1234";

            connection = DriverManager.getConnection(jdbcURL, username, password);

            System.out.println("Connected to H2 embedded database.");


            String sql = "Create table if not exists ORDERS (ID int primary key, encoded varchar(250))";
            Statement statement = connection.createStatement();
            statement.execute(sql);

            System.out.println("Created table Orders.");
/*
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
            connection.close();
*/

        } catch (SQLException throwables) {
            throwables.printStackTrace();
            return false;
        }
        return true;
    }

}

