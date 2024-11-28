package processor;

import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.TimeZone;

public class SingleOrder extends Order{
    public SingleOrder(HashMap<String, String> map) {
        super(map);
        repeated = false;
        planned_execution_date = new GregorianCalendar();
        planned_execution_date = datetime(stol(map["day"]), stol(map["month"]), stol(map["year"]));
    }
}
