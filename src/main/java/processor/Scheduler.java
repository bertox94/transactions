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

   
    void scheduleAll(List<Order> orders, Calendar today) {
        Iterator<Order> it = orders.iterator();
        
        while (it.hasNext()) {
            Order currentOrder = it.next();
            currentOrder.schedule(today);
            
            Iterator<Order> el = orders.iterator();
            while (el.hasNext()) {
                Order order = el.next();
                if (order.scheduled && 
                    order.effectiveExecutionDate.before(currentOrder.effectiveExecutionDate)) {
                    continue;
                }
                break; // Break when we find the first order that is not scheduled or is after currentOrder
            }

            // Keep a reference to where to insert
            Order insertionPosition = el.hasNext() ? el.next() : null;

            Iterator<Order> el2 = el; // Copy iterator reference to compare later
            while (el2.hasNext()) {
                Order nextOrder = el2.next();
                // Compare the dates and amounts accordingly
                if (nextOrder.scheduled &&
                    nextOrder.effectiveExecutionDate.equals(currentOrder.effectiveExecutionDate) &&
                    nextOrder.amount < currentOrder.amount) {
                    insertionPosition = el2.next(); // This will point to where we should insert currentOrder
                } else {
                    break;
                }
            }

            if (!currentOrder.expired) {
                // Use the insertion position's index, if it's not null
                if (insertionPosition != null) {
                    orders.add(orders.indexOf(insertionPosition), currentOrder);
                } else {
                    orders.add(currentOrder); // or add at end if no position
                }
            }
            it.remove(); // Remove current order
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
