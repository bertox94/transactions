package processor;

import org.javatuples.Quintet;

import java.util.Calendar;
import java.util.Iterator;
import java.util.List;


public class Scheduler {
    void insert_in_order(List<Quintet<String, Calendar, Calendar, Double, Double>> records,
                         Order el, double balance) {
        Quintet<String, Calendar, Calendar, Double, Double> back = records.getLast();
        Calendar dd = back.getValue2();
        dd.add(Calendar.DAY_OF_MONTH, 1);
        while (dd.compareTo(el.effective_execution_date) < 0) {
            Quintet<String, Calendar, Calendar, Double, Double> q = new Quintet<>("", Calendar.getInstance(), dd, (double) 0, back.getValue4());
            records.addLast(q);
            dd.add(Calendar.DAY_OF_MONTH, 1);
        }
        Quintet<String, Calendar, Calendar, Double, Double> q = new Quintet<>(el.descr, el.planned_execution_date, el.effective_execution_date, el.amount, balance);
        records.addLast(q);
    }

    void scheduleall(List<Order> orders, Calendar today) {
        for (int i = 0; i < orders.size(); ) {
            orders.get(i).schedule(today);
            int j = 0;
            while (j < orders.size() && orders.get(j).scheduled &&
                    orders.get(j).effective_execution_date.compareTo(orders.get(i).effective_execution_date) < 0)
                j++;
            int k = j;
            Order el2 = orders.get(j);
            while (k != orders.size()) {
                if (k != orders.size() && orders.get(k).scheduled &&
                        ( * el2)->effective_execution_date == ( * it)->effective_execution_date &&
                        ( * el2)->amount < ( * it)->amount)
                j++;
            else
                break;
                k++;
            }

            if (!( * it)->expired)
            orders.insert(el, * it);
            it = orders.erase(it);
        }
    }

    void scheduleall(List<Order> orders, Calendar today) {
        for (int i = 0; i < orders.size(); ) {
            orders.get(i).schedule(today);
            int j = 0;
            while (j != orders.size() && orders.get(j).scheduled &&
                    (orders.get(j).effective_execution_date.compareTo(orders.get(i).effective_execution_date) < 0))
                j++;
            int k = j;
            while (k < orders.size()) {
                if (orders.get(k).scheduled &&
                        orders.get(k).effective_execution_date == orders.get(i).effective_execution_date &&
                        orders.get(k).amount < orders.get(i).amount)
                    j++;
                else
                    break;
                k++;
            }
            if (!orders.get(i).expired)
            orders.(el, * it);
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

};
