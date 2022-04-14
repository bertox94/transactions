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

    explicit order(unordered_map<string, string> map) {

        if (map.size() == 8) {
            repeated = false;
            descr = map["descr"];
            wt = map["wt"] == "true";
            amount = stol(map["amount"]);
            planned_execution_date = datetime(stol(map["day"]), stol(map["month"]), stol(map["year"]));
        } else if (map.size() == 17) {
            repeated = map["repeated"] == "true";
            descr = map["descr"];
            wt = map["wt"] == "true";
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
            amount = stod(map["amount"]);
        }
    }


    datetime planned_execution_date;
    datetime effective_execution_date;

    bool repeated;
    string descr;
    bool wt;
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
    double amount;
    bool expired = false;
    bool scheduled = false;
};

void insert_in_order(list<tuple<std::string, struct datetime, struct datetime, double, double>> &records,
                     order &el, double &balance) {
    auto back = records.back();
    auto dd = std::get<2>(back) + ::dd(1);
    while (dd < el.effective_execution_date) {
        records.emplace_back("", datetime(), dd, 0, std::get<4>(back));
        dd += ::dd(1);
    }
    records.emplace_back(el.descr, el.planned_execution_date, el.effective_execution_date, el.amount, balance);
}

void set_execution_date(order &el) {
    el.effective_execution_date = el.planned_execution_date;
    if (el.wt)
        el.effective_execution_date = el.effective_execution_date.first_working_day();
}

void check_expired(order &el) {
    //if (el.rfindt != nullptr)
    //    if (el.effective_execution_date > el.rfindt)
    //        el.expired = true;
}

std::string schedule(order &el, datetime today) {
    el.scheduled = true;
    if (el.repeated) {
        datetime dtt;
        if (el.f2 == "days") {
            dtt = {el.rinitdd, el.rinitmm, el.rinityy};
            if (dtt < today) {
                period pd = today - dtt;
                dtt += (pd / dd(el.f1)) * dd(el.f1) + (pd % dd(el.f1) == 0 ? dd(0) : dd(el.f1));
            }
            el.planned_execution_date = dtt;
            set_execution_date(el);
            check_expired(el);
        } else if (el.f2 == "months") {
            long long mm;
            if (el.f3 == "default")
                dtt = {el.rdd, el.rinitmm, el.rinityy};
            else
                dtt = {EndOfMonth, el.rinitmm, el.rinityy};

            if (dtt < today) {
                mm = dtt.months_to(today);
                dtt = dtt.after_months((mm / el.f1) * el.f1 + (mm % el.f1 == 0 ? 0 : el.f1)).fix();

                if (el.f3 == "default") {
                    if (dtt.getYear() == today.getYear() &&
                        dtt.getMonth() == today.getMonth() &&
                        dtt.getDay() < today.getDay())
                        dtt = dtt.after_months(el.f1).fix();
                }
            }

            el.planned_execution_date = dtt;
            set_execution_date(el);
            check_expired(el);
        } else if (el.f2 == "years") {
            long long yy;
            if (el.f3 == "default")
                dtt = {el.rdd, el.rinitmm, el.rinityy};
            else if (el.f3 == "eom")
                dtt = {EndOfMonth, el.rinitmm, el.rinityy};
            else
                dtt = {EndOfYear, el.rinityy};

            if (dtt < today) {
                yy = dtt.years_between(today);
                dtt = dtt.after_years((yy / el.f1) * el.f1 + (yy % el.f1 == 0 ? 0 : el.f1)).fix();

                if (el.f3 == "default") {
                    if (dtt.getYear() == today.getYear() &&
                        dtt.getMonth() == today.getMonth() &&
                        dtt.getDay() < today.getDay())
                        dtt = dtt.after_years(el.f1).fix();
                }
            }

            el.planned_execution_date = dtt;
            set_execution_date(el);
            check_expired(el);
        }
    } else {
        set_execution_date(el);
        check_expired(el);
    }

    std::stringstream ss;
    ss << el.effective_execution_date;
    return el.expired ? "Expired" : ss.str();
}

void scheduleall(list<order> &orders, datetime &today) {
    for (auto it = orders.begin(); it != orders.end();) {
        schedule(*it, today);
        auto el = orders.begin();
        while (el != orders.end() && el->scheduled && el->effective_execution_date < it->effective_execution_date)
            el++;
        auto el2 = el;
        while (el2 != orders.end()) {
            el2++;
            if (el2 != orders.end() && el2->scheduled &&
                el2->effective_execution_date == it->effective_execution_date &&
                el2->amount < it->amount)
                el++;
            else
                break;
        }

        if (!it->expired)
            orders.insert(el, *it);
        it = orders.erase(it);
    }
}

void reschedule(order &el) {

    if (!el.repeated) {
        //cout << right << setw(25) << std::setfill(' ') << "Stopped " << endl;
        el.expired = true;
        return;
    }

    if (el.f2 == "days") {
        el.planned_execution_date += dd(el.f1);
    } else if (el.f2 == "months") {
        if (el.f3 == "eom") {
            el.planned_execution_date = el.planned_execution_date.after_months(el.f1).fix().end_of_month();
        } else {
            el.planned_execution_date = el.planned_execution_date.after_months(el.f1).setDay(el.rdd).fix();
        }
    } else if (el.f2 == "years") {
        if (el.f3 == "eoy") {
            el.planned_execution_date = el.planned_execution_date.after_years(el.f1);
        } else if (el.f3 == "eom") {
            el.planned_execution_date = el.planned_execution_date.after_years(el.f1);
        } else {
            el.planned_execution_date = el.planned_execution_date.after_years(el.f1).setMonth(el.rmm).setDay(
                    el.rdd).fix();
        }
    }

    set_execution_date(el);
    check_expired(el);
}

string execute(double &balance, order &el) {
    balance += el.amount;
    //record.emplace_back(el.effective_execution_date, balance, el.amount);
    string warn = " style = \"color:  red; font-weight: bold;\"";
    string bold = " style = \"font-weight: bold;\"";
    stringstream ss;
    ss <<
       "        <tr>\n" <<
       "          <!-- <th scope=\"row\">1</th> -->\n" <<
       "          <td" << (balance < 0 ? warn : "") << ">" << el.planned_execution_date << "</td>\n" <<
       "          <td"
       << (balance < 0 ? warn : (el.planned_execution_date != el.effective_execution_date ? bold : ""))
       << ">" << el.effective_execution_date << "</td>\n" <<
       "          <td" << (balance < 0 ? warn : "") << ">" << el.descr << "</td>\n" <<
       "          <td" << (balance < 0 ? warn : "") << ">" << fixed << std::setprecision(2) << el.amount
       << "</td>\n" <<
       "          <td" << (balance < 0 ? warn : "") << ">" << balance << "</td>\n" <<
       "          <td" << (balance < 0 ? warn : "") << ">"
       << ((balance == 0 ? "ALERT" : (balance < 0 ? "FAILURE" : "OK")))
       << "</td>\n" <<
       "        </tr>\n";

    return ss.str();
}

string preview(list<order> &orders, datetime enddate, double account_balance) {

    list<tuple<string, datetime, datetime, double, double>> records;
    datetime today(time(NULL));
    scheduleall(orders, today);

    records.emplace_back("", datetime(), today, 0, account_balance);

    for (auto it = orders.begin(); it != orders.end() && it->effective_execution_date <= enddate;) {
        execute(account_balance, *it);
        insert_in_order(records, *it, account_balance);
        reschedule(*it);
        auto el = it;
        el++;
        while (el != orders.end() && el->effective_execution_date < it->effective_execution_date)
            el++;
        auto el2 = el;
        while (el2 != orders.end()) {
            el2++;
            if (el2 != orders.end() && el2->effective_execution_date == it->effective_execution_date &&
                el2->amount < it->amount)
                el++;
            else
                break;
        }

        if (!it->expired)
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
