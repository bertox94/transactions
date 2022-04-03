package com.example.demo;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.core.io.InputStreamResource;
import org.springframework.core.io.Resource;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import javax.swing.plaf.nimbus.State;
import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.sql.*;
import java.util.Objects;
import java.util.Vector;
import java.util.concurrent.atomic.AtomicInteger;

@Controller
public class MainController {

    static Connection connection;
    static AtomicInteger preview_id = new AtomicInteger(0);
    static Vector<Integer> vec = new Vector<>();
    static final Object lock = new Object();

    // private String path = "C:\\Users\\tiraboschi\\Desktop\\folder1\\export.xml";
    private String path = "./export.xml";
    private String pathStorage = "./data.json";
    private ObjectMapper mapper = new ObjectMapper();

    private Integer getvalue() {
        int i = 0;
        synchronized (lock) {
            for (Integer el :
                    vec) {
                if (el == null) {
                    vec.insertElementAt(i, i);
                    return i;
                }
                i++;
            }
            vec.add(i);
        }
        return i;
    }

    private void deletevalue(Integer i) {
        synchronized (lock) {
            vec.set(i, null);
        }
    }

    @ResponseBody
    @PostMapping(path = "/orders")
    public String orders() {
        StringBuilder data = new StringBuilder();
        try {
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery("SELECT encoded FROM public.single_orders;");

            while (rs.next()) {
                data.append(rs.getString(1)).append("\n");
            }

        } catch (SQLException throwables) {
            throwables.printStackTrace();
        }
        return data.toString();
    }

    @ResponseBody
    @PostMapping(path = "/schedule")
    public String schedule(@RequestParam String data) {
        return SpaceTime_Gap.send("schedule\n" + data);
    }

    @ResponseBody
    @PostMapping(path = "/preview")
    public String preview(@RequestParam String data) {
        String orders = orders();
        StringBuilder resp = new StringBuilder(SpaceTime_Gap.send("preview\n" + data + "\n" + orders));
        int val = getvalue();
        String TABLENAME = "public.preview" + val;
        try {

            Statement stmt = connection.createStatement();
            stmt.executeUpdate("DROP TABLE IF EXISTS " + TABLENAME + " ;");

            stmt.executeUpdate("CREATE TABLE " + TABLENAME + " ( " +
                    " executiondate date NULL UNIQUE, " +
                    " planneddate date NULL, " +
                    " descr varchar NULL, " +
                    " amount numeric NULL, " +
                    " balance numeric NULL " +
                    ");");

            String[] lines = resp.toString().split("\n");
            StringBuilder VALUES = new StringBuilder();
            for (String line :
                    lines) {
                String[] tokens = line.split(";");
                VALUES.append("('").append(tokens[0]).append("',")
                        .append(Objects.equals(tokens[1], " null") ? "NULL" : "'" + tokens[1] + "'").append(", ")
                        .append(Objects.equals(tokens[2], " null") ? "NULL" : "'" + tokens[2] + "'").append(", ")
                        .append(tokens[3]).append(", ")
                        .append(tokens[4]).append("), ");
            }
            VALUES.deleteCharAt(VALUES.length() - 2);
            VALUES.append(";");

            stmt.executeUpdate(
                    "INSERT INTO " + TABLENAME + " (descr, planneddate, executiondate, amount, balance) VALUES " + VALUES);

            //QUERY FOR THE TABLE
            ResultSet rs = stmt.executeQuery("select * " +
                    " from " + TABLENAME +
                    " where planneddate is not null; ");

            resp = new StringBuilder("{\"enddate\":\"" + data.substring(0, data.indexOf('\n')) + "\",\"initialbal\":\"" + data.substring(data.indexOf('\n') + 1) + "\", \"html\":[");

            if (rs.next()) {
                resp.append("{\"executiondate\":\"").append(rs.getString(1)).append("\",");
                resp.append("\"planneddate\":\"").append(rs.getString(2)).append("\",");
                resp.append("\"descr\":\"").append(rs.getString(3)).append("\",");
                resp.append("\"amount\":\"").append(rs.getString(4)).append("\",");
                resp.append("\"balance\":\"").append(rs.getString(5)).append("\"}");
            }
            while (rs.next()) {
                resp.append(",{\"executiondate\":\"").append(rs.getString(1)).append("\",");
                resp.append("\"planneddate\":\"").append(rs.getString(2)).append("\",");
                resp.append("\"descr\":\"").append(rs.getString(3)).append("\",");
                resp.append("\"amount\":\"").append(rs.getString(4)).append("\",");
                resp.append("\"balance\":\"").append(rs.getString(5)).append("\"}");
            }
            resp.append("], ");

            //QUERIES FOR THE FIRST CHART
            rs = stmt.executeQuery(" select distinct executiondate " +
                    " from " + TABLENAME +
                    " ORDER BY executiondate ASC;");

            resp.append("\"arr1\":[");
            if (rs.next())
                resp.append("\"").append(rs.getString(1)).append("\"");
            while (rs.next()) {
                resp.append(",\"").append(rs.getString(1)).append("\"");
            }
            resp.append("], ");

            rs = stmt.executeQuery(" select sum(balance) as balance " +
                    " from " + TABLENAME + " " +
                    " group by executiondate " +
                    " order by executiondate; ");

            resp.append("\"arr2\":[");
            if (rs.next())
                resp.append("\"").append(rs.getString(1)).append("\"");
            while (rs.next()) {
                resp.append(",\"").append(rs.getString(1)).append("\"");
            }
            resp.append("], ");

            rs = stmt.executeQuery("SELECT COUNT(*) " +
                    " FROM (" +
                    "   select distinct executiondate " +
                    "   FROM " + TABLENAME +
                    "   ) as subq1");

            long num = 0L;
            if (rs.next()) {
                num = rs.getLong(1);
            }

            rs = stmt.executeQuery(" select m, " +
                    "        (select avg(balance) " +
                    "         from (select sum(balance) as balance " +
                    "               from " + TABLENAME + " " +
                    "               group by executiondate " +
                    "               order by executiondate " +
                    "              ) as subq_b " +
                    "        ) - m * (select (count(*) + 1) / 2.0 " +
                    "                 from ( " +
                    "                          select distinct executiondate " +
                    "                          from " + TABLENAME + " " +
                    "                      ) as subq2 " +
                    "        ) as q " +
                    " from ( " +
                    "          select _num / nullif(_denom,0) as m " +
                    "          from ( " +
                    "                   select sum(balance_a * x_a) as _num " +
                    "                   from ( " +
                    "                            select balance - (select avg(balance) " +
                    "                                              from ( " +
                    "                                                       select sum(balance) as balance " +
                    "                                                       from " + TABLENAME + " " +
                    "                                                       group by executiondate " +
                    "                                                       order by executiondate " +
                    "                                                   ) as subq1) as balance_a, " +
                    "                                   x_a " +
                    "                            from ( " +
                    "                                     select sum(balance) as balance, executiondate " +
                    "                                     from " + TABLENAME + " " +
                    "                                     group by executiondate " +
                    "                                     order by executiondate " +
                    "                                 ) as subq2 " +
                    "                                     full outer join ( " +
                    "                                select ROW_NUMBER() OVER (ORDER BY executiondate) - (select (count(*) + 1) / 2.0 " +
                    "                                                                                     from ( " +
                    "                                                                                              select distinct executiondate " +
                    "                                                                                              from " + TABLENAME + " " +
                    "                                                                                          ) as subq2 " +
                    "                                ) as x_a, " +
                    "                                       executiondate " +
                    "                                from ( " +
                    "                                         select distinct executiondate " +
                    "                                         from " + TABLENAME + " " +
                    "                                     ) as subq1 " +
                    "                            ) as subq3 on subq2.executiondate = subq3.executiondate " +
                    "                            where subq2.executiondate is not null " +
                    "                               or subq3.executiondate is not null " +
                    "                        ) as subq4 " +
                    "               ) as sub6, " +
                    "               ( " +
                    "                   select sum(x_a ^ 2) as _denom " +
                    "                   from ( " +
                    "                            select ROW_NUMBER() OVER (ORDER BY executiondate) - (select (count(*) + 1) / 2.0 " +
                    "                                                                                 from ( " +
                    "                                                                                          select distinct executiondate " +
                    "                                                                                          from " + TABLENAME + " " +
                    "                                                                                      ) as subq2) as x_a " +
                    "                            from ( " +
                    "                                     select distinct executiondate " +
                    "                                     from " + TABLENAME + " " +
                    "                                 ) as subq1 " +
                    "                        ) as subq5 " +
                    "               ) as sub7 " +
                    "      ) as sub14; ");

            double m = 0;
            double q = 0;

            if (rs.next()) {
                m = rs.getDouble(1);
                q = rs.getDouble(2);
            }

            resp.append("\"arr3\":[");
            resp.append("\"").append(q).append("\"");
            for (long i = 1; i < num; i++) {
                resp.append(",\"").append(m * i + q).append("\"");
            }
            resp.append("], ");

            //QUERIES FOR THE SECOND CHART
            rs = stmt.executeQuery(" select executiondate " +
                    " from " + TABLENAME + " " +
                    " where planneddate is not null" +
                    " order by executiondate;");

            resp.append("\"arr4\":[");
            if (rs.next())
                resp.append("\"").append(rs.getString(1)).append("\"");
            while (rs.next()) {
                resp.append(",\"").append(rs.getString(1)).append("\"");
            }
            resp.append("], ");

            rs = stmt.executeQuery(" select amount " +
                    " from " + TABLENAME + " " +
                    " where planneddate is not null " +
                    "order by executiondate;");

            resp.append("\"arr5\":[");
            if (rs.next())
                resp.append("\"").append(rs.getString(1)).append("\"");
            while (rs.next()) {
                resp.append(",\"").append(rs.getString(1)).append("\"");
            }
            resp.append("], ");

            rs = stmt.executeQuery(" select balance " +
                    " from " + TABLENAME + " " +
                    " where planneddate is not null" +
                    " order by executiondate;");

            resp.append("\"arr6\":[");
            if (rs.next())
                resp.append("\"").append(rs.getString(1)).append("\"");
            while (rs.next()) {
                resp.append(",\"").append(rs.getString(1)).append("\"");
            }
            resp.append("]}");

            stmt.executeUpdate("DROP TABLE " + TABLENAME + " ;");

        } catch (SQLException e) {
            e.printStackTrace();
        }

        deletevalue(val);

        return resp.toString();
    }

    @ResponseBody
    @PostMapping(path = "/addnew")
    public String addnew(@RequestParam String data) {
        try {
            String _SUB_Q_ID = " (  SELECT ROW_NUMBER " +
                    "               FROM (" +
                    "                  SELECT ROW_NUMBER() OVER (ORDER BY id), id " +
                    "                  FROM ( " +
                    "                      SELECT id    " +
                    "                      FROM single_orders " +
                    "                      UNION ALL  " +
                    "                      SELECT COALESCE(MAX(id),2) AS id   " +
                    "                      FROM single_orders         " +
                    "                  ) AS sub1" +
                    "               ) AS sub2       " +
                    "               WHERE ROW_NUMBER != id      " +
                    "               LIMIT 1) ";
            Statement stmt = connection.createStatement();
            stmt.executeUpdate(
                    "INSERT INTO public.single_orders (id, encoded) " +
                            "VALUES(" + _SUB_Q_ID + ", '{\"id\":'|| '\"' || " + _SUB_Q_ID + "|| '\"," + data.substring(1) + "');");
        } catch (SQLException throwables) {
            throwables.printStackTrace();
            return "KO";
        }
        return "OK";
    }

    @ResponseBody
    @PostMapping(path = "/delete")
    public String delete(@RequestParam String data) {
        try {
            Statement stmt = connection.createStatement();
            stmt.executeUpdate("DELETE FROM public.single_orders WHERE id = " + data + ";");
        } catch (SQLException throwables) {
            throwables.printStackTrace();
            return "KO";
        }
        return "OK";
    }

    @ResponseBody
    @PostMapping(path = "/duplicate")
    public String duplicate(@RequestParam String data) {
        try {
            Statement stmt = connection.createStatement();
            stmt.executeUpdate(
                    "INSERT INTO public.single_orders (id, encoded) " +
                            "SELECT (" +
                            "       SELECT ROW_NUMBER " +
                            "       FROM( " +
                            "           SELECT ROW_NUMBER() OVER (ORDER BY id), id " +
                            "           FROM (  " +
                            "               SELECT id " +
                            "               FROM single_orders " +
                            "               UNION ALL " +
                            "               SELECT COALESCE(MAX(id),2) AS id " +
                            "               FROM single_orders " +
                            "           ) AS sub1 " +
                            "       ) AS sub2 " +
                            "       WHERE ROW_NUMBER != id " +
                            "       LIMIT 1 ),encoded " +
                            "FROM public.single_orders " +
                            "WHERE id = " + data + ";");

        } catch (SQLException throwables) {
            throwables.printStackTrace();
            return "KO";
        }
        return "OK";
    }

    @PostMapping("/haltprocessor")
    @ResponseStatus(value = HttpStatus.OK)
    public void haltprocessor() {
        SpaceTime_Gap.send("close");
    }

    @PostMapping("/export")
    @ResponseBody
    public boolean export(@RequestParam String data) {

        data = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                + "<xfa:data xmlns:xfa=\"http://www.xfa.org/schema/xfa-data/1.0/\">\n" + "	<moduloIntermittenti>\n"
                + "		<Campi>\n" + "			<CFdatorelavoro>00909360174</CFdatorelavoro>\n"
                + "			<BCbarcodeModello01>ML-15-01</BCbarcodeModello01>\n"
                + "			<BCbarcodeModello01>ML-15-01</BCbarcodeModello01>\n"
                + "			<EMmail>info@ristorantealmulino.it</EMmail>\n" + data;

        data = data + "\n</Campi>\n" + "	</moduloIntermittenti>\n" + "</xfa:data>";

        File file = new File(path);
        file.delete();

        try {
            file.createNewFile();
            PrintWriter printWriter = new PrintWriter(path);
            printWriter.println(data);
            printWriter.close();
        } catch (IOException ioe) {
            return false;
        }

        return true;
    }

    @PostMapping("/update")
    @ResponseStatus(value = HttpStatus.OK)
    public void update(@RequestParam String data) {

        PrintWriter writer;
        try {
            writer = new PrintWriter(pathStorage);
            writer.println(data);
            writer.close();
            //String[][] array = mapper.readValue(data, String[][].class);
            //mapper.writeValue(new FileOutputStream(pathStorage), array);


        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @GetMapping(path = "/download")
    public ResponseEntity<Resource> download() throws FileNotFoundException {
        File file = new File(path);
        HttpHeaders headers = new HttpHeaders();
        try {
            headers.set("Content-Disposition", "attachment; filename=\"" + file.getName() + "\"");
            InputStreamResource resource = new InputStreamResource(new FileInputStream(file));

            return ResponseEntity.ok()
                    .headers(headers)
                    .contentLength(file.length())
                    .contentType(MediaType.parseMediaType("application/octet-stream"))
                    .body(resource);

        } catch (
                IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return null;
    }

}
