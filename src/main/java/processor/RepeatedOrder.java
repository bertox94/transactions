package processor;

import java.time.Duration;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.HashMap;

public class RepeatedOrder extends Order {

    public int f1;
    public String f2;
    public String f3;
    public int rdd;
    public int rmm;
    public boolean rlim;
    public int rinitdd;
    public int rinitmm;
    public int rinityy;
    public int rfindd;
    public int rfinmm;
    public int rfinyy;

    public RepeatedOrder(HashMap<String, String> map) {
        super(map);
        repeated = true;
        f1 = Integer.parseInt(map.get("f1"));
        f2 = map.get("f2");
        f3 = map.get("f3");
        if (!map.get("rdd").equals("$"))
            rdd = Integer.parseInt(map.get("rdd"));
        if (!map.get("rmm").equals("$"))
            rmm = Integer.parseInt(map.get("rmm"));
        if (!map.get("rinitdd").equals("$"))
            rinitdd = Integer.parseInt(map.get("rinitdd"));
        if (!map.get("rinitmm").equals("$"))
            rinitmm = Integer.parseInt(map.get("rinitmm"));
        if (!map.get("rinityy").equals("$"))
            rinityy = Integer.parseInt(map.get("rinityy"));
        rlim = map.get("rlim").equals("limited");
        if (rlim) {
            if (!map.get("rfindd").equals("$"))
                rfindd = Integer.parseInt(map.get("rfindd"));
            if (!map.get("rfinmm").equals("$"))
                rfinmm = Integer.parseInt(map.get("rfinmm"));
            if (!map.get("rfinyy").equals("$"))
                rfinyy = Integer.parseInt(map.get("rfinyy"));
        }
    }


    void check_expired(GregorianCalendar today) {
        if (!rlim)
            return;

        Calendar enddate = Calendar.getInstance();
        switch (f2) {
            case "days":
                enddate.set(Calendar.DAY_OF_MONTH, rfindd);
                enddate.set(Calendar.MONTH, rfinmm);
                enddate.set(Calendar.YEAR, rfinyy);
                break;
            case "months":
                if (f3.equals("eom")) {
                    enddate.set(Calendar.YEAR, rfinyy);
                    enddate.set(Calendar.MONTH, rfinmm);
                } else
                    enddate.set(Calendar.DAY_OF_MONTH, rfindd);
                break;
            case "years":
                if (f3.equals("eoy")) {
                    enddate.set(Calendar.YEAR, rfinyy);
                } else if (f3.equals("eom")) {
                    enddate.set(Calendar.YEAR, rfinyy);
                    enddate.set(Calendar.MONTH, rfinmm);
                } else {
                    enddate.set(Calendar.YEAR, rfinyy);
                    enddate.set(Calendar.MONTH, rfinmm);
                    enddate.set(Calendar.DAY_OF_MONTH, rfindd);
                }
                break;
        }
        if (effective_execution_date.compareTo(today) < 0)
            expired = true;
    }

    //make sure today has 0hrs, 0min, 0sec
    String schedule(Calendar today) {
        scheduled = true;
        Calendar dtt = Calendar.getInstance();
        dtt.set(Calendar.DAY_OF_MONTH, rinitdd);
        dtt.set(Calendar.MONTH, rinitmm);
        dtt.set(Calendar.YEAR, rinityy);
        switch (f2) {
            case "days":
                if (dtt.compareTo(today) < 0) {
                    dtt.add(Calendar.DATE, f1);
                }
                break;
            case "months":

                if (f3.equals("eom")) {
                    int lastDay = dtt.getActualMaximum(Calendar.DAY_OF_MONTH);
                    dtt.set(Calendar.DAY_OF_MONTH, lastDay);
                }

                if (dtt.compareTo(today) < 0) {
                    dtt.add(Calendar.MONTH, f1);
                }
                break;
            case "years":
                if (f3.equals("eom")) {
                    int lastDay = dtt.getActualMaximum(Calendar.DAY_OF_MONTH);
                    dtt.set(Calendar.DAY_OF_MONTH, lastDay);
                } else if (f3.equals("eoy")) {
                    int lastDay = dtt.getActualMaximum(Calendar.DAY_OF_MONTH);
                    dtt.set(Calendar.DAY_OF_MONTH, lastDay);
                    int lastMonth = dtt.getActualMaximum(Calendar.MONTH);
                    dtt.set(Calendar.MONTH, lastMonth);
                }

                if (dtt.compareTo(today) < 0) {
                    dtt.add(Calendar.YEAR, f1);
                }
                   /* if (f3 == "default") {
                        if (dtt.getYear() == today.getYear() &&
                                dtt.getMonth() == today.getMonth() &&
                                dtt.getDay() < today.getDay())
                            dtt = dtt.after_years(f1).fix();
                        */
                break;
        }
        planned_execution_date = dtt;
        set_execution_date();
        if (effective_execution_date.compareTo(today) < 0)
            expired = true;

        return expired ? "Expired" : effective_execution_date.toString();
    }

    void reschedule(Calendar today) override {
        if (f2 == "days") {
            planned_execution_date += dd(f1);
        } else if (f2 == "months") {
            if (f3 == "eom")
                planned_execution_date = planned_execution_date.after_months(f1).end_of_month();
            else
                planned_execution_date = planned_execution_date.after_months(f1).setDay(rdd).fix();
        } else if (f2 == "years") {
            if (f3 == "eoy")
                planned_execution_date = planned_execution_date.after_years(f1).end_of_year();
            else if (f3 == "eom")
                planned_execution_date = planned_execution_date.after_years(f1).end_of_month();
            else
                planned_execution_date = planned_execution_date.after_years(f1).setMonth(rmm).setDay(rdd).fix();
        }
        set_execution_date();
        check_expired(today);
    }

    void execute(double &balance) override {
        balance += amount;
    }
}

;

void insert_in_order(list<tuple<std::string, struct datetime, struct datetime, double, double>>&records,
                     const shared_ptr<order> &el, double &balance) {
    auto back = records.back();
    auto dd = std::get < 2 > (back) + ::dd(1);
    while (dd < el -> effective_execution_date) {
        records.emplace_back("", datetime(), dd, 0, std::get < 4 > (back));
        dd += ::dd(1);
    }
    records.emplace_back(el -> descr, el -> planned_execution_date, el -> effective_execution_date, el -> amount, balance);
}

void scheduleall(list<shared_ptr<order>> &orders, datetime &today) {
    for (auto it = orders.begin(); it != orders.end(); ) {
        ( * it)->schedule(today);
        auto el = orders.begin();
        while (el != orders.end() && ( * el)->scheduled &&
                ( * el)->effective_execution_date < ( * it)->effective_execution_date)
        el++;
        auto el2 = el;
        while (el2 != orders.end()) {
            if (el2 != orders.end() && ( * el2)->scheduled &&
                    ( * el2)->effective_execution_date == ( * it)->effective_execution_date &&
                    ( * el2)->amount < ( * it)->amount)
            el++;
            else
            break;
            el2++;
        }

        if (!( * it)->expired)
        orders.insert(el, * it);
        it = orders.erase(it);
    }
}

string preview(list<shared_ptr<order>> &orders, datetime enddate, double account_balance) {

    list<tuple<string, datetime, datetime, double, double>> records;
    datetime today (time(NULL));
    scheduleall(orders, today);

    records.emplace_back("", datetime(), today, 0, account_balance);

    for (auto it = orders.begin(); it != orders.end() && ( * it)->effective_execution_date <= enddate;){
        ( * it)->execute(account_balance);
        insert_in_order(records, * it, account_balance);
        ( * it)->reschedule(today);
        auto el = it;
        el++;
        while (el != orders.end() && ( * el)->effective_execution_date < ( * it)->effective_execution_date)
        el++;
        auto el2 = el;
        while (el2 != orders.end()) {
            if (el2 != orders.end() && ( * el2)->effective_execution_date == ( * it)->effective_execution_date &&
                    ( * el2)->amount < ( * it)->amount)
            el++;
            else
            break;
            el2++;
        }

        if (!( * it)->expired)
        orders.insert(el, * it);
        it = orders.erase(it);
    }

    string resp;

    for (auto & el:records){
        stringstream ss;
        ss << std::get < 0 > (el) << "; ";
        ss << std::get < 1 > (el) << "; ";
        ss << std::get < 2 > (el) << "; ";
        ss << std::get < 3 > (el) << "; ";
        ss << std::get < 4 > (el) << endl;
        resp += ss.str();
    }
    resp.pop_back();

    return resp;
}
