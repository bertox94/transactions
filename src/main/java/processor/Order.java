package processor;

import java.util.Calendar;
import java.util.HashMap;

public class Order {
    public boolean repeated;
    public String descr;
    public boolean wt;
    public Calendar plannedExecutionDate;
    public Calendar effectiveExecutionDate;
    public double amount;
    public boolean scheduled = false;
    public boolean expired = false;

    public Order(HashMap<String, String> map) {
        descr = map.get("descr");
        wt = map.get("wt").equals("true");
        amount = Double.parseDouble(map.get("amount"));
    }

    void check_expired(Calendar today){}

    public String schedule(Calendar today) {  return "";  }

    void reschedule(Calendar today) {}

    double execute(double balance){
        return balance;
    }

    boolean isExpired(){
        return expired;
    }

    void set_execution_date() {
        effectiveExecutionDate = plannedExecutionDate;
        if (wt){
            while(effectiveExecutionDate.get(Calendar.DAY_OF_WEEK)==Calendar.MONDAY)
                effectiveExecutionDate.add(Calendar.DATE,1);
        }
    }
}
