#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <list>
#include <cmath>
#include <sstream>
#include "classes.h"

using namespace std;

std::string trim(const std::string &str, const std::string &whitespace = " \t") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

class order {
public:
    bool repeated;
    string descr;
    bool wt;
    datetime planned_execution_date;
    datetime effective_execution_date;
    double amount;
    bool scheduled = false;
    bool expired = false;

    explicit order(unordered_map<string, string> map) {
        descr = map["descr"];
        wt = map["wt"] == "true";
        amount = stol(map["amount"]);
    }

    virtual void check_expired(datetime &today) = 0;

    virtual std::string schedule(datetime &today) = 0;

    virtual void reschedule(datetime &today) {};

    virtual void execute(double &balance) = 0;

    void set_execution_date() {
        effective_execution_date = planned_execution_date;
        if (wt)
            effective_execution_date = effective_execution_date.first_working_day();
    }
};

class single_order : public order {
public:
    explicit single_order(unordered_map<string, string> map) : order(map) {
        repeated = false;
        planned_execution_date = datetime(stol(map["day"]), stol(map["month"]), stol(map["year"]));
    }

    void check_expired(datetime &today) override {
        if (effective_execution_date < today)
            expired = true;
    }

    std::string schedule(datetime &today) override {
        scheduled = true;
        set_execution_date();
        check_expired(today);
        std::stringstream ss;
        ss << effective_execution_date;
        return expired ? "Expired" : ss.str();
    }

    void execute(double &balance) override {
        expired = true;
        balance += amount;
    }

};

class repeated_order : public order {
public:
    explicit repeated_order(unordered_map<string, string> map) : order(map) {
        repeated = true;
        f1 = stol(map["f1"]);
        f2 = map["f2"];
        f3 = map["f3"];
        if (map["rdd"] != "$")
            rdd = stoi(map["rdd"]);
        if (map["rmm"] != "$")
            rmm = stoi(map["rmm"]);
        if (map["rinitdd"] != "$")
            rinitdd = stoi(map["rinitdd"]);
        if (map["rinitmm"] != "$")
            rinitmm = stoi(map["rinitmm"]);
        if (map["rinityy"] != "$")
            rinityy = stol(map["rinityy"]);
        rlim = map["rlim"] == "limited";
        if (rlim) {
            if (map["rfindd"] != "$")
                rfindd = stoi(map["rfindd"]);
            if (map["rfinmm"] != "$")
                rfinmm = stoi(map["rfinmm"]);
            if (map["rfinyy"] != "$")
                rfinyy = stol(map["rfinyy"]);
        }
    }

    long f1;
    string f2;
    string f3;
    int rdd;
    int rmm;
    bool rlim;
    int rinitdd;
    int rinitmm;
    int rinityy;
    int rfindd;
    int rfinmm;
    int rfinyy;

    void check_expired(datetime &today) override {
        if (!rlim)
            return;

        datetime enddate;
        if (f2 == "days") {
            enddate = datetime(rfindd, rfinmm, rfinyy);
        } else if (f2 == "months") {
            if (f3 == "eom")
                enddate = datetime(EndOfMonth, rfinmm, rfinyy);
            else
                enddate = datetime(rdd, rfinmm, rfinyy);
        } else if (f2 == "years") {
            if (f3 == "eoy")
                enddate = datetime(EndOfYear, rfinyy);
            else if (f3 == "eom")
                enddate = datetime(EndOfMonth, rmm, rfinyy);
            else
                enddate = datetime(rdd, rmm, rfinyy);
        }
        if (effective_execution_date > enddate)
            expired = true;
    }

    std::string schedule(datetime &today) override {
        scheduled = true;
        datetime dtt;
        if (f2 == "days") {
            dtt = {rinitdd, rinitmm, rinityy};
            if (dtt < today) {
                period pd = today - dtt;
                dtt += (pd / dd(f1)) * dd(f1) + (pd % dd(f1) == 0 ? dd(0) : dd(f1));
            }
            planned_execution_date = dtt;
            set_execution_date();
            check_expired(today);
        } else if (f2 == "months") {
            long long mm;
            if (f3 == "default")
                dtt = {rdd, rinitmm, rinityy};
            else
                dtt = {EndOfMonth, rinitmm, rinityy};

            if (dtt < today) {
                mm = dtt.months_to(today);
                dtt = dtt.after_months((mm / f1) * f1 + (mm % f1 == 0 ? 0 : f1)).fix();

                if (f3 == "default") {
                    if (dtt.getYear() == today.getYear() &&
                        dtt.getMonth() == today.getMonth() &&
                        dtt.getDay() < today.getDay())
                        dtt = dtt.after_months(f1).fix();
                }
            }

            planned_execution_date = dtt;
            set_execution_date();
            check_expired(today);
        } else if (f2 == "years") {
            long long yy;
            if (f3 == "default")
                dtt = {rdd, rmm, rinityy};
            else if (f3 == "eom")
                dtt = {EndOfMonth, rmm, rinityy};
            else
                dtt = {EndOfYear, rinityy};

            if (dtt < today) {
                yy = dtt.years_to(today);
                dtt = dtt.after_years((yy / f1) * f1 + (yy % f1 == 0 ? 0 : f1)).fix();

                if (f3 == "default") {
                    if (dtt.getYear() == today.getYear() &&
                        dtt.getMonth() == today.getMonth() &&
                        dtt.getDay() < today.getDay())
                        dtt = dtt.after_years(f1).fix();
                }
            }

            planned_execution_date = dtt;
            set_execution_date();
            check_expired(today);
        }

        std::stringstream ss;
        ss << effective_execution_date;
        return expired ? "Expired" : ss.str();
    }

    void reschedule(datetime &today) override {
        if (f2 == "days") {
            planned_execution_date += dd(f1);
        } else if (f2 == "months") {
            if (f3 == "eom") {
                planned_execution_date = planned_execution_date.after_months(f1).fix().end_of_month();
            } else {
                planned_execution_date = planned_execution_date.after_months(f1).setDay(rdd).fix();
            }
        } else if (f2 == "years") {
            if (f3 == "eoy") {
                planned_execution_date = planned_execution_date.after_years(f1).end_of_year();
            } else if (f3 == "eom") {
                planned_execution_date = planned_execution_date.after_years(f1).end_of_month();
            } else {
                planned_execution_date = planned_execution_date.after_years(f1).setMonth(rmm).setDay(
                        rdd).fix();
            }
        }

        set_execution_date();
        check_expired(today);
    }

    void execute(double &balance) override {
        balance += amount;
    }
};

void insert_in_order(list<tuple<std::string, struct datetime, struct datetime, double, double>> &records,
                     const shared_ptr<order> &el, double &balance) {
    auto back = records.back();
    auto dd = std::get<2>(back) + ::dd(1);
    while (dd < el->effective_execution_date) {
        records.emplace_back("", datetime(), dd, 0, std::get<4>(back));
        dd += ::dd(1);
    }
    records.emplace_back(el->descr, el->planned_execution_date, el->effective_execution_date, el->amount, balance);
}

void scheduleall(list<shared_ptr<order>> &orders, datetime &today) {
    for (auto it = orders.begin(); it != orders.end();) {
        (*it)->schedule(today);
        auto el = orders.begin();
        while (el != orders.end() && (*el)->scheduled &&
               (*el)->effective_execution_date < (*it)->effective_execution_date)
            el++;
        auto el2 = el;
        while (el2 != orders.end()) {
            if (el2 != orders.end() && (*el2)->scheduled &&
                (*el2)->effective_execution_date == (*it)->effective_execution_date &&
                (*el2)->amount < (*it)->amount)
                el++;
            else
                break;
            el2++;
        }

        if (!(*it)->expired)
            orders.insert(el, *it);
        it = orders.erase(it);
    }
}


string preview(list<shared_ptr<order>> &orders, datetime enddate, double account_balance) {

    list<tuple<string, datetime, datetime, double, double>> records;
    datetime today(time(NULL));
    scheduleall(orders, today);

    records.emplace_back("", datetime(), today, 0, account_balance);

    for (auto it = orders.begin(); it != orders.end() && (*it)->effective_execution_date <= enddate;) {
        (*it)->execute(account_balance);
        insert_in_order(records, *it, account_balance);
        (*it)->reschedule(today);
        auto el = it;
        el++;
        while (el != orders.end() && (*el)->effective_execution_date < (*it)->effective_execution_date)
            el++;
        auto el2 = el;
        while (el2 != orders.end()) {
            if (el2 != orders.end() && (*el2)->effective_execution_date == (*it)->effective_execution_date &&
                (*el2)->amount < (*it)->amount)
                el++;
            else
                break;
            el2++;
        }

        if (!(*it)->expired)
            orders.insert(el, *it);
        it = orders.erase(it);
    }

    string resp;

    for (auto &el: records) {
        stringstream ss;
        ss << std::get<0>(el) << "; ";
        ss << std::get<1>(el) << "; ";
        ss << std::get<2>(el) << "; ";
        ss << std::get<3>(el) << "; ";
        ss << std::get<4>(el) << endl;
        resp += ss.str();
    }
    resp.pop_back();

    return resp;
}
