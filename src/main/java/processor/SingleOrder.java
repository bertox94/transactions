package processor;

import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.HashMap;

public class SingleOrder extends Order {
    public SingleOrder(HashMap<String, String> map) {
        super(map);
        repeated = false;
        planned_execution_date = new GregorianCalendar();
        planned_execution_date.set(Calendar.DAY_OF_MONTH, Integer.parseInt(map.get("day")));
        planned_execution_date.set(Calendar.MONTH, Integer.parseInt(map.get("month")));
        planned_execution_date.set(Calendar.YEAR, Integer.parseInt(map.get("year")));
    }

    void check_expired(Calendar today) {
        if (effective_execution_date.compareTo(today) < 0)
            expired = true;
    }

    String schedule(Calendar today) {
        scheduled = true;
        set_execution_date();
        check_expired(today);
        return expired ? "Expired" : effective_execution_date.toString();
    }

    double execute(double balance) {
        expired = true;
        balance += amount;
        return balance;
    }

}
