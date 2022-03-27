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

        if (map.size() == 7) {
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
            rdd = stoi(map["rdd"]);
            rmm = stoi(map["rmm"]);
            rlim = map["rlim"] == "limited";
            rinitdt = datetime(stoi(map["rinitdd"]), stoi(map["rinitmm"]), stol(map["rinityy"]));
            if (rlim)
                rfindt = datetime(stoi(map["rfindd"]), stoi(map["rfinmm"]), stol(map["rfinyy"]));
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
    datetime rinitdt;
    datetime rfindt;
    double amount;
    bool cancelled;
    bool scheduled;
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

double find_m(std::list<double> &balances) {
    double v1 = 0, v2 = 0, v3 = 0, v4 = 0, v5 = 0;
    int i = 0;

    for (auto el: balances) {
        v1 += (i * el);
        v2 += i;
        v3 += el;
        v4 += pow(i, 2);
        v5 += i;
        i++;
    }

    v1 *= balances.size();
    v2 *= v3;
    v4 *= balances.size();
    v5 = pow(v5, 2);

    return (v1 - v2) / (v4 - v5);

}

double find_q(std::list<double> &balances) {
    double v1 = 0, v2 = 0;
    int i = 0;
    for (auto el: balances) {
        v1 += el;
        v2 += i;
        i++;
    }
    v2 *= find_m(balances);

    return ((double) 1 / balances.size()) * (v1 - v2);

}

string print_formatted_balances(list<tuple<struct datetime, double, double>> records) {
    string str = "[";

    for (auto &el: records) {
        str += "\"";
        str += to_string(std::get<1>(el));
        str += "\", ";
    }

    str.pop_back();
    str.pop_back();

    str += "]";
    return str;
}

string print_formatted_expenses(list<tuple<struct datetime, double, double>> records) {
    string str = "[";

    for (const auto &el: records) {
        str += "\"";
        str += to_string(std::get<1>(el));
        str += "\", ";
    }

    str.pop_back();
    str.pop_back();

    str += "]";
    return str;
}

string print_formatted_interpolation(list<tuple<struct datetime, double, double>> records) {

    list<double> balances;

    for (auto &el: records)
        balances.push_back(std::get<2>(el));


    double m = find_m(balances);
    double q = find_q(balances);
    string str = "[";

    long x = 0;
    for (auto el: records) {
        str += "\"";
        str += to_string(m * x + q);
        str += "\", ";
        x++;
    }

    str.pop_back();
    str.pop_back();

    str += "]";
    return str;
}

string print_formatted_dates(list<tuple<struct datetime, double, double>> records) {
    string sttt;
    std::stringstream str(sttt);
    str << "[";

    for (const auto &el: records) {
        str << "\"";
        str << std::get<0>(el);
        str << "\", ";
    }

    string s = str.str();
    s.pop_back();
    s.pop_back();
    s += "]";
    return s;
}

string schedule(order &el, datetime today) {
    if (el.repeated) {
        datetime initial_date = el.rinitdt;
        if (initial_date >= today) {
            el.planned_execution_date = initial_date;
        } else {
            if (el.f2 == "days") {
                period pd = today - initial_date;
                datetime dtt = initial_date + (pd / dd(el.f1)) * dd(el.f1) +
                               (pd % dd(el.f1) == 0 ? dd(0) : dd(el.f1));
                el.planned_execution_date = dtt;
            } else if (el.f2 == "months") {
                long long mm = initial_date.months_between(today);
                datetime dtt = initial_date.after_months(
                        (mm / el.f1) * el.f1 + (mm % el.f1 == 0 ? 0 : el.f1)).fix();
                if (el.f3 == "eom") {
                    el.planned_execution_date = dtt.end_of_month();
                } else {
                    el.planned_execution_date = dtt;
                    el.planned_execution_date.setDay(el.rdd).fix();
                    if (el.wt)
                        dtt = dtt.first_working_day();

                    if (dtt.getYear() == today.getYear())
                        if (dtt.getMonth() == today.getMonth())
                            if (dtt.getDay() < today.getDay())
                                el.planned_execution_date = el.planned_execution_date.after_months(el.f1).fix();
                }
            } else if (el.f2 == "years") {
                long long yy = initial_date.years_between(today);
                datetime dtt = initial_date.after_years(
                        (yy / el.f1) * el.f1 + (yy % el.f1 == 0 ? 0 : el.f1));
                if (el.f2 == "eoy") {
                    el.planned_execution_date = datetime(31, 12, dtt.getYear());
                } else if (el.f2 == "eom") {
                    el.planned_execution_date = dtt.end_of_month();
                } else {
                    if (el.rmm < today.getMonth() || el.rmm == today.getMonth() && el.rdd < today.getDay())
                        dtt = dtt.after_years(el.f1).fix();
                    el.planned_execution_date = dtt.setMonth(el.rmm).setDay(el.rdd).fix();
                }
            }
        }
    }

    el.effective_execution_date = el.planned_execution_date;
    if (el.wt)
        el.effective_execution_date = el.effective_execution_date.first_working_day();

    if (!el.repeated) {
        if (el.effective_execution_date < today) {
            //cout << right << setw(14) << std::setfill(' ') << "Cancelled "
            //     << (el.repeated ? "     " : " (R) ") << setw(24) << el.name << endl;
            el.cancelled = true;
            return "Expired";
        }
    }

    if (el.rfindt != nullptr) {
        if (el.effective_execution_date > el.rfindt) {
            //cout << right << setw(14) << std::setfill(' ') << "Cancelled "
            //     << (el.repeated ? "     " : " (R) ") << setw(24) << el.name << endl;
            el.cancelled = true;
            return "Expired";
        }
    }


    //cout << right << setw(14) << std::setfill(' ') << "Scheduled " << (el.repeated ? "     " : " (R) ")
    //     << setw(24) << el.name << "        for: " << setw(30) << el.effective_execution_date
    //     << std::setfill(' ') << endl;

    stringstream ss;
    ss << el.effective_execution_date;
    return ss.str();

};

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

        it->scheduled = true;
        orders.insert(el, *it);
        it = orders.erase(it);
    }
}

void reschedule(order &el) {

    if (!el.repeated) {
        //cout << right << setw(25) << std::setfill(' ') << "Stopped " << endl;
        el.cancelled = true;
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

    if (el.rfindt != nullptr) {
        if (el.effective_execution_date > el.rfindt) {
            //cout << right << setw(14) << std::setfill(' ') << "Cancelled "
            //     << (el.repeated ? "     " : " (R) ") << setw(24) << el.name << endl;
            el.cancelled = true;
        }
    }

    el.effective_execution_date = el.planned_execution_date;
    if (el.wt)
        el.effective_execution_date = el.effective_execution_date.first_working_day();

    //cout << right << setw(25) << std::setfill(' ') << "Rescheduled " << "     for: " << setw(30)
    //     << el.effective_execution_date << std::setfill(' ') << endl;

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
    datetime today(20, 3, 2022);
    scheduleall(orders, today);

    records.emplace_back("", datetime(), today, 0, account_balance);

    for (auto it = orders.begin(); it != orders.end() && it->effective_execution_date <= enddate;) {
        cout << it->effective_execution_date << endl;
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
