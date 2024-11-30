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

    public String preview(List<Order> orders, Calendar endDate, double accountBalance) {
        List<Record> records = new LinkedList<>();
        Calendar today = Calendar.getInstance();
        scheduleAll(orders, today);
        records.add(new Record("", null, today, 0, accountBalance));

        // Iterate through orders
        for (int i = 0; i < orders.size() && (orders.get(i).effectiveExecutionDate.before(endDate) || 
             orders.get(i).effectiveExecutionDate.equals(endDate));) {
             
            Order order = orders.get(i);
            order.execute(accountBalance);
            insertInOrder(records, order, accountBalance);
            order.reschedule(today);

            // Find the proper position for this order
            int el = i + 1;
            while (el < orders.size() && orders.get(el).effectiveExecutionDate.before(order.effectiveExecutionDate)) {
                el++;
            }

            int el2 = el;
            while (el2 < orders.size()) {
                if (orders.get(el2).effectiveExecutionDate.equals(order.effectiveExecutionDate) && 
                    orders.get(el2).amount < order.amount) {
                    el++;
                } else {
                    break;
                }
                el2++;
            }

            if (!order.isExpired()) {
                orders.add(el, order);
            }
            orders.remove(i);
        }

        StringBuilder resp = new StringBuilder();
        for (Record el : records) {
            resp.append(el.toString()).append("\n");
        }
        // Remove last character (newline)
        if (resp.length() > 0) {
            resp.setLength(resp.length() - 1);  
        }

        return resp.toString();
    }
}
