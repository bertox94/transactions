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
            //descr, planneddate, executiondate, amount, id, balance
            sql = "Create table if not exists PREVIEW (" +
                    "id_prev int primary key auto_increment, " +
                    "descr varchar(100), " +
                    "planneddate date, " +
                    "executiondate date, " +
                    "amount double, " +
                    "id int, " +
                    "balance double " +
                    ")";
            statement = connection.createStatement();
            statement.execute(sql);

/*
            sql = "Insert into orders (ID, encoded) values (1, 'Nam Ha Minh')";

            int rows = statement.executeUpdate(sql);

            if (rows > 0) {
                System.out.println("Inserted a new row.");
            }


            sql = "SELECT * FROM orders";

            statement = connection.createStatement();
            ResultSet resultSet = statement.executeQuery(sql);

            int count = 0;

            while (resultSet.next()) {
                count++;

                int ID = resultSet.getInt("ID");
                String name = resultSet.getString("encoded");
                System.out.println("Student #" + count + ": " + ID + ", " + name);
            }

            sql = "Delete from ORDERS;";
            statement = connection.createStatement();
            statement.execute(sql);
            // connection.close();
*/

        } catch (SQLException throwables) {
            throwables.printStackTrace();
            return false;
        }
        return true;
    }

}

