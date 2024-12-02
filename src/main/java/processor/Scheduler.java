package processor;

import java.util.*;

class Request {
    public String descr;
    public Calendar plannedExecutionDate;
    public Calendar effectiveExecutionDate;
    public Double amount;
    public Double balance;

    public Request(String descr, Calendar plannedExecutionDate, Calendar effectiveExecutionDate, Double amount, Double balance) {
        this.descr = descr;
        this.plannedExecutionDate = plannedExecutionDate;
        this.effectiveExecutionDate = effectiveExecutionDate;
        this.amount = amount;
        this.balance = balance;
    }
}

public class Scheduler {
    static void insertInOrder(List<Request> records,
                              Order el, double balance) {
        Request back = records.getLast();
        Calendar dd = back.effectiveExecutionDate;
        dd.add(Calendar.DAY_OF_MONTH, 1);
        while (dd.compareTo(el.effectiveExecutionDate) < 0) {
            Request q = new Request("", Calendar.getInstance(), dd, (double) 0, back.balance);
            records.addLast(q);
            dd.add(Calendar.DAY_OF_MONTH, 1);
        }
        Request q = new Request(el.descr, el.plannedExecutionDate, el.effectiveExecutionDate, el.amount, balance);
        records.addLast(q);
    }


    static void scheduleAll(List<Order> orders, Calendar today) {
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

    public static String preview(List<Order> orders, Calendar endDate, double accountBalance) {
        List<Request> records = new LinkedList<>();
        Calendar today = Calendar.getInstance();
        scheduleAll(orders, today);
        records.getLast();
        records.add(new Request("", null, today, (double) 0, accountBalance));

        // Iterate through orders
        for (int i = 0; i < orders.size() && (orders.get(i).effectiveExecutionDate.before(endDate) ||
                orders.get(i).effectiveExecutionDate.equals(endDate)); ) {

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
        for (Request el : records) {
            resp.append(el.toString()).append("\n");
        }
        // Remove last character (newline)
        if (resp.length() > 0) {
            resp.setLength(resp.length() - 1);
        }

        return resp.toString();
    }
}
