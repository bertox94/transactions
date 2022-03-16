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

import java.io.*;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.sql.*;
import java.util.ArrayList;
import java.util.List;

@Controller
public class MainController {

    static Connection connection;

    // private String path = "C:\\Users\\tiraboschi\\Desktop\\folder1\\export.xml";
    private String path = "./export.xml";
    private String pathStorage = "./data.json";
    private ObjectMapper mapper = new ObjectMapper();

    @ResponseBody
    @PostMapping(path = "/populate1")
    public String populate1() {
        String data = "";
        try {
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery("SELECT * FROM public.single_orders;");

            while (rs.next()) {
                data += rs.getString(1) + "; " + rs.getString(2) + "\n";
            }

        } catch (SQLException throwables) {
            throwables.printStackTrace();
        }
        return data;
    }

    @ResponseBody
    @PostMapping(path = "/schedule")
    public String schedule(@RequestParam String data) {
        return SpaceTime_Gap.send("schedule\n" + data);
    }

    @ResponseBody
    @PostMapping(path = "/quickcheck")
    public String quickcheck(@RequestParam String data) {
        return SpaceTime_Gap.send("quickcheck\n" + data);
    }

    @ResponseBody
    @PostMapping(path = "/addnews")
    public String addnews(@RequestParam String data) {
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

    @GetMapping("/get")
    @ResponseBody
    public String get() {

        String content = "";

        try {
            content = new String(Files.readAllBytes(Paths.get(pathStorage)));
            // data = mapper.readValue(new File("c:\\test\\staff.json"), String[][].class);
        } catch (IOException ioe) {

        }

        return content;
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
