package com.example.demo;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonMappingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.core.io.InputStreamResource;
import org.springframework.core.io.Resource;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;
import processor.Order;
import processor.RepeatedOrder;
import processor.Scheduler;
import processor.SingleOrder;

import javax.swing.plaf.nimbus.State;
import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.sql.*;
import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;

@Controller
public class MainController {

    static Connection connection;
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

        /*public class Library {
            @JsonProperty("libraryname")
            public String name;

            @JsonProperty("mymusic")
            public List<Song> songs;
        }
        public class Song {
            @JsonProperty("Artist Name") public String artistName;
            @JsonProperty("Song Name") public String songName;
        }

        Library lib = mapper.readValue(jsonString, Library.class);
        */

        StringBuilder data = new StringBuilder();
        try {
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery("SELECT encoded FROM public.ORDERS " +
                    " order by substring(encoded from (locate('\"descr\":', encoded))) asc ;");

            List<Order> orders = new ArrayList<>();

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
    public String schedule(@RequestParam String data) throws JsonProcessingException {

/*
        data = data.substr(data.find('\n') + 1);
        auto map = JSONtomap(msg);
        datetime dt = datetime(time(NULL));
        if (map["repeated"] == "true") {
            repeated_order ro (map);
            resp = ro.schedule(dt);
        } else {
            single_order so (map);
            resp = so.schedule(dt);
*/
        ObjectMapper mapper = new ObjectMapper();
        Map<String, String> map = mapper.readValue(data, Map.class);
        Order order;
        if (!Boolean.parseBoolean(map.get("repeated")))
            order = new SingleOrder(new HashMap<>(map));
        else
            order = new RepeatedOrder(new HashMap<>(map));

        return order.schedule(Calendar.getInstance()) + "\r\n";
    }

    @ResponseBody
    @PostMapping(path = "/preview")
    public String preview(@RequestParam String data) throws JsonProcessingException {
        String ordersString = orders();
        List<Order> orders = new ArrayList<>();

        if (ordersString.isEmpty())
            return "";

        String[] lines = ordersString.split("\n");
        StringJoiner VALUES = new StringJoiner(",", "", "");

        for (String line :
                lines) {
            //String[] tokens = line.split(";");
            HashMap<String, String> map = mapper.readValue(line, HashMap.class);
            if (!Boolean.parseBoolean(map.get("repeated")))
                orders.addLast(new SingleOrder(map));
            else
                orders.addLast(new RepeatedOrder(map));

        }
        String _resp = Scheduler.preview(orders, Calendar.getInstance(), 2300);//(data+"\n"+orders);//SpaceTime_Gap.send("preview\n" + data + "\n" + orders);
        StringJoiner resp = new StringJoiner(",", "{", "}");
        int val = getvalue();
        String TABLENAME = "public.preview";
        try {

            Statement stmt = connection.createStatement();

            lines = _resp.split("\n");
            VALUES = new StringJoiner(",", "", "");

            for (String line :
                    lines) {
                String[] tokens = line.split(";");

                StringJoiner sjj = new StringJoiner(",", "(", ")");
                sjj.add("'" + tokens[0] + "'");
                sjj.add((Objects.equals(tokens[1], " null") ? "NULL" : "'" + tokens[1] + "'"));
                sjj.add((Objects.equals(tokens[2], " null") ? "NULL" : "'" + tokens[2] + "'"));
                sjj.add(tokens[3]);
                sjj.add(Integer.toString(val));
                sjj.add(tokens[4]);

                VALUES.add(sjj.toString());
            }

            stmt.executeUpdate("INSERT INTO " + TABLENAME + " (descr, planneddate, executiondate, amount, id, balance) VALUES "
                    + VALUES + ";");

            //QUERY FOR THE TABLE
            ResultSet rs = stmt.executeQuery("select * " +
                    " from " + TABLENAME +
                    " where planneddate is not null and id = " + val + "; ");

            resp.add("\"enddate\":\"" + data.substring(0, data.indexOf('\n')) + "\"");
            resp.add("\"initialbal\":\"" + data.substring(data.indexOf('\n') + 1) + "\"");

            StringJoiner sj = new StringJoiner(",", "[", "]");

            while (rs.next()) {
                StringJoiner sjj = new StringJoiner(",", "{", "}");

                sjj.add("\"executiondate\":\"" + rs.getString(1) + "\"");
                sjj.add("\"planneddate\":\"" + rs.getString(2) + "\"");
                sjj.add("\"descr\":\"" + rs.getString(3) + "\"");
                sjj.add("\"amount\":\"" + rs.getString(4) + "\"");
                sjj.add("\"balance\":\"" + rs.getString(5) + "\"");

                sj.add(sjj.toString());

            }
            resp.add("\"html\":" + sj);

            //QUERIES FOR THE FIRST CHART
            rs = stmt.executeQuery(" select distinct executiondate " +
                    " from " + TABLENAME +
                    " where id = " + val +
                    " ORDER BY executiondate ASC;");

            sj = new StringJoiner(",", "[", "]");
            while (rs.next()) {
                sj.add("\"" + rs.getString(1) + "\"");
            }
            resp.add("\"arr1\":" + sj);

            rs = stmt.executeQuery("select distinct on (executiondate) executiondate, balance " +
                    "from (" +
                    "   select ROW_NUMBER() OVER() as rownumber, *" +
                    "   from preview p " +
                    "   order by rownumber desc " +
                    ") as subq2; ");

            sj = new StringJoiner(",", "[", "]");
            while (rs.next()) {
                sj.add("\"" + rs.getString(2) + "\"");
            }
            resp.add("\"arr2\":" + sj);

            rs = stmt.executeQuery("SELECT COUNT(*) " +
                    " FROM (" +
                    "   select distinct executiondate " +
                    "   FROM " + TABLENAME +
                    " where id = " + val +
                    "   ) as subq1");

            long num = 0L;
            if (rs.next()) {
                num = rs.getLong(1);
            }

            rs = stmt.executeQuery("select m, (select avg(balance) " +
                    "           from ( " +
                    "               select distinct on (executiondate) executiondate, balance " +
                    "               from ( " +
                    "                   select ROW_NUMBER() OVER() as rownumber, * " +
                    "                   from preview p " +
                    "                   order by rownumber desc " +
                    "               ) as subq223 " +
                    "           ) as subq_b " +
                    "       ) - m * (select (count(*) + 1) / 2.0 " +
                    "           from ( " +
                    "               select distinct executiondate " +
                    "               from " + TABLENAME + " " +
                    "               where id = " + val +
                    "           ) as subq2 " +
                    "       ) as q " +
                    "       from ( " +
                    "           select _num / nullif(_denom,0) as m " +
                    "           from ( " +
                    "               select sum(balance_a * x_a) as _num " +
                    "               from ( " +
                    "                   select balance - (select avg(balance) " +
                    "                       from ( " +
                    "                           select distinct on (executiondate) executiondate, balance " +
                    "                           from ( " +
                    "                               select ROW_NUMBER() OVER() as rownumber, * " +
                    "                               from preview p " +
                    "                               order by rownumber desc " +
                    "                           ) as subq223 " +
                    "                       ) as subq1" +
                    "                   ) as balance_a, " +
                    "                   x_a " +
                    "                   from ( " +
                    "                       select distinct on (executiondate) executiondate, balance " +
                    "                       from ( " +
                    "                           select ROW_NUMBER() OVER() as rownumber, * " +
                    "                           from preview p " +
                    "                           order by rownumber desc " +
                    "                       ) as subq223 " +
                    "                    ) as subq2 " +
                    "                    inner join ( " +
                    "                       select ROW_NUMBER() OVER (ORDER BY executiondate) - (select (count(*) + 1) / 2.0 " +
                    "                           from ( " +
                    "                               select distinct executiondate " +
                    "                               from " + TABLENAME + " " +
                    "                               where id = " + val +
                    "                           ) as subq2 " +
                    "                       ) as x_a, " +
                    "                       executiondate " +
                    "                       from ( " +
                    "                           select distinct executiondate " +
                    "                           from " + TABLENAME + " " +
                    "                           where id = " + val +
                    "                       ) as subq1 " +
                    "                    ) as subq3 on subq2.executiondate = subq3.executiondate " +
                    "                    where subq2.executiondate is not null " +
                    "                    or subq3.executiondate is not null " +
                    "               ) as subq4 " +
                    "           ) as sub6, ( " +
                    "               select sum(x_a ^ 2) as _denom " +
                    "               from ( " +
                    "                   select ROW_NUMBER() OVER (ORDER BY executiondate) - (select (count(*) + 1) / 2.0 " +
                    "                       from ( " +
                    "                            select distinct executiondate " +
                    "                            from " + TABLENAME + " " +
                    "                            where id = " + val +
                    "                        ) as subq2" +
                    "                   ) as x_a " +
                    "                   from ( " +
                    "                       select distinct executiondate " +
                    "                       from " + TABLENAME + " " +
                    "                       where id = " + val +
                    "                   ) as subq1 " +
                    "               ) as subq5 " +
                    "           ) as sub7 " +
                    "       ) as sub14; ");

            double m = 0;
            double q = 0;

            if (rs.next()) {
                m = rs.getDouble(1);
                q = rs.getDouble(2);
            }

            resp.add("\"m\":\"" + m + "\"");
            resp.add("\"q\":\"" + q + "\"");

            sj = new StringJoiner(",", "[", "]");
            for (long i = 0; i < num; i++) {
                sj.add("\"" + String.format(Locale.US, "%.2f", (m * i + q)) + "\"");
            }
            resp.add("\"arr3\":" + sj);

            //QUERIES FOR THE SECOND CHART
            rs = stmt.executeQuery(" select executiondate " +
                    " from " + TABLENAME + " " +
                    " where planneddate is not null and id = " + val +
                    " order by executiondate;");

            sj = new StringJoiner(",", "[", "]");
            while (rs.next()) {
                sj.add("\"" + rs.getString(1) + "\"");
            }
            resp.add("\"arr4\":" + sj);

            rs = stmt.executeQuery(" select amount " +
                    " from " + TABLENAME + " " +
                    " where planneddate is not null and id = " + val +
                    "order by executiondate;");

            sj = new StringJoiner(",", "[", "]");
            while (rs.next()) {
                sj.add("\"" + rs.getString(1) + "\"");
            }
            resp.add("\"arr5\":" + sj);

            rs = stmt.executeQuery(" select balance " +
                    " from " + TABLENAME + " " +
                    " where planneddate is not null and id = " + val +
                    " order by executiondate;");

            sj = new StringJoiner(",", "[", "]");
            while (rs.next()) {
                sj.add("\"" + rs.getString(1) + "\"");
            }
            resp.add("\"arr6\":" + sj);

            stmt.executeUpdate("DELETE FROM " + TABLENAME + " WHERE id = " + val + " ;");

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
                    "                  SELECT ROW_NUMBER() OVER (ORDER BY id) as ROW_NUMBER, id " +
                    "                  FROM ( " +
                    "                      SELECT id    " +
                    "                      FROM orders " +
                    "                      UNION ALL  " +
                    "                      SELECT COALESCE(MAX(id),2) AS id   " +
                    "                      FROM orders         " +
                    "                  ) AS sub1 " +
                    "               ) AS sub2       " +
                    "               WHERE ROW_NUMBER != id      " +
                    "               LIMIT 1) ";
            Statement stmt = connection.createStatement();
            stmt.executeUpdate(
                    "INSERT INTO public.orders (id, encoded) " +
                            "VALUES(" + _SUB_Q_ID + ", '{\"id\":'|| '\"' || " + _SUB_Q_ID + "|| '\"," + data.substring(1) + "');");
        } catch (SQLException throwables) {
            throwables.printStackTrace();
            return "KO";
        }
        return "OK";
    }

    @ResponseBody
    @PostMapping(path = "/get")
    public String get(@RequestParam String _id) {
        String resp = "";
        try {

            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery(
                    "SELECT encoded " +
                            " FROM public.orders " +
                            " WHERE id = " + _id + ";");

            if (rs.next())
                resp = rs.getString(1);

        } catch (SQLException throwables) {
            throwables.printStackTrace();
            return "KO";
        }
        return resp;
    }

    @ResponseBody
    @PostMapping(path = "/delete")
    public String delete(@RequestParam String data) {
        try {
            Statement stmt = connection.createStatement();
            stmt.executeUpdate("DELETE FROM public.orders WHERE id = " + data + ";");
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

            String _SUB_Q_ID = " (  SELECT ROW_NUMBER " +
                    "               FROM (" +
                    "                  SELECT ROW_NUMBER() OVER (ORDER BY id), id " +
                    "                  FROM ( " +
                    "                      SELECT id    " +
                    "                      FROM orders " +
                    "                      UNION ALL  " +
                    "                      SELECT COALESCE(MAX(id),2) AS id   " +
                    "                      FROM orders         " +
                    "                  ) AS sub1 " +
                    "               ) AS sub2       " +
                    "               WHERE ROW_NUMBER != id      " +
                    "               LIMIT 1) ";

            stmt.executeUpdate(
                    "INSERT INTO public.orders (id, encoded) " +
                            "SELECT " + _SUB_Q_ID + ", REGEXP_REPLACE(encoded, '{\"id\":\"[0-9]*\"', '{\"id\":\"'||" + _SUB_Q_ID + "||'\"' ) " +
                            "FROM public.orders " +
                            "WHERE id = " + data + ";");

        } catch (SQLException throwables) {
            throwables.printStackTrace();
            return "KO";
        }
        return "OK";
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
