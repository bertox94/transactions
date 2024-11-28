package processor;

import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.HashMap;

public class Order {
    public boolean repeated;
    public String descr;
    public boolean wt;
    public Calendar planned_execution_date;
    public Calendar effective_execution_date;
    public double amount;
    public boolean scheduled = false;
    public boolean expired = false;

    public Order(HashMap<String, String> map) {
        descr = map.get("descr");
        wt = map.get("wt").equals("true");
        amount = Double.parseDouble(map.get("amount"));
    }

    void check_expired(Calendar today){}

    String schedule(Calendar today) {  return "";  }

    void reschedule(Calendar today) {}

    double execute(double balance){
        return balance;
    }

    void set_execution_date() {
        effective_execution_date = planned_execution_date;
        if (wt){
            while(effective_execution_date.get(Calendar.DAY_OF_WEEK)==Calendar.MONDAY)
                effective_execution_date.add(Calendar.DATE,1);
        }
    }
}
