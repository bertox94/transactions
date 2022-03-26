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
};


bool compare(order &first, order &second) {
    if (first.effective_execution_date < second.effective_execution_date) {
        return true;
    }
    if (first.effective_execution_date == second.effective_execution_date) {
        if (first.amount < second.amount)
            return true;
        else return false;
    }
    return false;
}

//TODO: do checks functions to check all possibilites of wrong definition of input (in parse())

//suppose the JSON is valid, not array and only string as values
std::unordered_map<string, string> JSONtomap(string json) {
    std::unordered_map<std::string, std::string> map = {};
    json = json.substr(1, json.length() - 2);

    for (int i = 0; i < json.length(); i++) {
        if (json[i] == '\"') {
            string key;
            i++;
            for (; json[i] != '\"'; i++)
                key += json[i];
            string value;
            i += 3;
            for (; json[i] != '\"'; i++)
                value += json[i];
            map[key] = value;
            i++;
        }
    }

    return map;
}

void insert_in_order(list<tuple<datetime, double, double>> &records,
                     _List_iterator<_List_val<_List_simple_types<order>>> &el, double &balance) {
    auto back = records.back();
    auto dd = std::get<0>(back);
    while (dd < el->effective_execution_date) {
        dd += ::dd(1);
        records.emplace_back(dd, std::get<1>(back), 0);
    }
    records.emplace_back(el->effective_execution_date, balance, el->amount);
}

void remove_duplicates(list<tuple<datetime, double, double>> &records) {
    auto prev = records.begin();
    auto curr = prev++;
    for (; curr != records.end();) {
        if (std::get<0>(*prev) == std::get<0>(*curr)) {
            prev = curr;
            curr = records.erase(curr);
        } else {
            prev = curr;
            curr++;
        }
    }
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

datetime get_date(string opt, long long day, long long month, long long year) {
    datetime dt;
    if (opt == "eoy") {
        dt = datetime(31, 12, year);
    } else if (opt == "eom") {
        dt = datetime(32, month, year).fix();
    } else
        dt = datetime(day, month, year);
    return dt;
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
                if (el.opti == "eoy") {
                    el.planned_execution_date = datetime(31, 12, dtt.getYear());
                } else if (el.opti == "eom") {
                    el.planned_execution_date = dtt.end_of_month();
                }
                el.planned_execution_date = dtt;

                if (el.is_wire_transfer)
                    dtt = dtt.first_working_day();

                if (dtt.getYear() == today.getYear())
                    if (dtt.getMonth() == today.getMonth())
                        if (dtt.getDay() < today.getDay())
                            el.planned_execution_date = el.planned_execution_date.after_years(el.f1).fix();
            }
        }
    }

    el.effective_execution_date = el.planned_execution_date;
    if (el.is_wire_transfer)
        el.effective_execution_date = el.effective_execution_date.first_working_day();

    if (el.repeated) {
        if (el.effective_execution_date < today) {
            //cout << right << setw(14) << std::setfill(' ') << "Cancelled "
            //     << (el.repeated ? "     " : " (R) ") << setw(24) << el.name << endl;
            el.cancelled = true;
            return "Expired";
        }
    }

    datetime final_date = get_date(el.optf, el.final_day, el.final_month, el.final_year);
    if (el.repeated_order_with_final_date) {
        if (el.effective_execution_date > final_date) {
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

string schedule2(order &el, datetime today) {
    if (!el.repeated) {
        datetime initial_date = get_date(el.opti, el.initial_day, el.initial_month, el.initial_year);
        if (initial_date >= today) {
            el.planned_execution_date = initial_date;
        } else {
            if (el.f2 == "days") {
                period pd = today - initial_date;
                datetime dtt = initial_date +
                               ((pd / dd(el.f1)) * dd(el.f1) +
                                (pd % dd(el.f1) == dd(0) ? dd(0) : dd(el.f1)));

                el.planned_execution_date = dtt;
            } else if (el.f2 == "months") {
                long long mm = initial_date.months_between(today);
                datetime dtt = initial_date.after_months(
                        (mm / el.f1) * el.f1 + (mm % el.f1 == 0 ? 0 : el.f1)).fix();
                if (el.opti == "eom") {
                    el.planned_execution_date = dtt.end_of_month();
                } else {
                    el.planned_execution_date = dtt;
                    if (el.is_wire_transfer)
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
                if (el.opti == "eoy") {
                    el.planned_execution_date = datetime(31, 12, dtt.getYear());
                } else if (el.opti == "eom") {
                    el.planned_execution_date = dtt.end_of_month();
                }
                el.planned_execution_date = dtt;

                if (el.is_wire_transfer)
                    dtt = dtt.first_working_day();

                if (dtt.getYear() == today.getYear())
                    if (dtt.getMonth() == today.getMonth())
                        if (dtt.getDay() < today.getDay())
                            el.planned_execution_date = el.planned_execution_date.after_years(el.f1).fix();
            }
        }
    }

    el.effective_execution_date = el.planned_execution_date;
    if (el.is_wire_transfer)
        el.effective_execution_date = el.effective_execution_date.first_working_day();

    if (el.repeated) {
        if (el.effective_execution_date < today) {
            //cout << right << setw(14) << std::setfill(' ') << "Cancelled "
            //     << (el.repeated ? "     " : " (R) ") << setw(24) << el.name << endl;
            el.cancelled = true;
            return "Expired";
        }
    }

    datetime final_date = get_date(el.optf, el.final_day, el.final_month, el.final_year);
    if (el.repeated_order_with_final_date) {
        if (el.effective_execution_date > final_date) {
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

    if (el.repeated) {
        //cout << right << setw(25) << std::setfill(' ') << "Stopped " << endl;
        el.cancelled = true;
        return;
    }

    if (el.f2 == "days") {
        el.planned_execution_date += dd(el.f1);
    } else if (el.f2 == "months") {
        if (true) {//end_of_month) {
            el.planned_execution_date = el.planned_execution_date.after_months(el.f1).fix().end_of_month();
        } else {
            el.planned_execution_date = el.planned_execution_date.after_months(el.f1).fix();
        }
    } else if (el.f2 == "years") {
        el.planned_execution_date = el.planned_execution_date.after_years(el.f1).fix();
    }

    datetime final_date = get_date(el.optf, el.final_day, el.final_month, el.final_year);
    if (el.repeated_order_with_final_date) {
        if (el.planned_execution_date > final_date) {
            //cout << right << setw(25) << std::setfill(' ') << "Stopped " << endl;
            el.cancelled = true;
            return;
        }
    }

    el.effective_execution_date = el.planned_execution_date;
    if (el.is_wire_transfer) {
        el.effective_execution_date = el.effective_execution_date.first_working_day();
    }

    //cout << right << setw(25) << std::setfill(' ') << "Rescheduled " << "     for: " << setw(30)
    //     << el.effective_execution_date << std::setfill(' ') << endl;

}

string execute(double &balance, order &el, list<tuple<datetime, double, double>> &record) {
    balance += el.amount;
    record.emplace_back(el.effective_execution_date, balance, el.amount);
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
       "          <td" << (balance < 0 ? warn : "") << ">" << el.name << "</td>\n" <<
       "          <td" << (balance < 0 ? warn : "") << ">" << fixed << std::setprecision(2) << el.amount
       << "</td>\n" <<
       "          <td" << (balance < 0 ? warn : "") << ">" << balance << "</td>\n" <<
       "          <td" << (balance < 0 ? warn : "") << ">"
       << ((balance == 0 ? "ALERT" : (balance < 0 ? "FAILURE" : "OK")))
       << "</td>\n" <<
       "        </tr>\n";

    return ss.str();
}

string preview(string param) {
    datetime enddate(31, 12, 2022);
    param = param.substr(param.find('\n') + 1);
    double account_balance = stod(param.substr(0, param.find('\n')));
    param = param.substr(param.find('\n') + 1);

    list<order> orders;
    list<tuple<datetime, double, double>> records;

    for (; !param.empty(); param = param.substr(param.find('\n') + 1))
        orders.emplace_back(JSONtomap(param.substr(0, param.find('\n'))));

    datetime today(20, 03, 2022);
    string resp;
    scheduleall(orders, today);

    for (auto it = orders.begin(); it != orders.end() && it->effective_execution_date <= enddate;) {
        cout << it->effective_execution_date << endl;
        resp += execute(account_balance, *it, records);
        insert_in_order(records, it, account_balance);
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

    return resp;
}


string print_formatted_balances(list<double> &balances) {
    string str = "[";

    for (auto el: balances) {
        str += "'";
        str += to_string(el);
        str += "', ";
    }

    str += "]";
    return str;
}

string print_formatted_balances2(list<double> &balances2) {
    string str = "[";

    for (auto el: balances2) {
        str += "'";
        str += to_string(el);
        str += "', ";
    }

    str += "]";
    return str;
}

string print_formatted_expenses(list<double> &expenses) {
    string str = "[";

    for (const auto &el: expenses) {
        str += "'";
        str += el;
        str += "', ";
    }

    str += "]";
    return str;
}

string print_formatted_interpolation(list<double> &balances) {
    double m = find_m(balances);
    double q = find_q(balances);
    string str = "[";

    long x = 0;
    for (auto el: balances) {
        str += "'";
        str += to_string(x * m + q);
        str += "', ";
        x++;
    }

    str += "]";
    return str;
}

string print_formatted_dates(list<datetime> &dates) {
    string sttt;
    std::stringstream str(sttt);
    str << "[";

    for (auto el: dates) {
        str << "'";
        str << el;
        str << "', ";
    }

    str << "]";
    return str.str();
}

string print_formatted_dates2(list<datetime> dates2) {
    string sttt;
    std::stringstream str(sttt);
    str << "[";

    for (auto el: dates2) {
        str << "'";
        str << el;
        str << "', ";
    }

    str << "]";
    return str.str();
}


int main2() {

    ofstream myfile;
    myfile.open("scheduleall.html");

    myfile << "<!doctype html>\n"
              "<html lang=\"en\">\n"
              "  <head>\n"
              "    <!-- Required meta tags -->\n"
              "    <meta charset=\"utf-8\">\n"
              "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
              "\n"
              "    <!-- Bootstrap CSS -->\n"
              "    <link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.mins.css\" rel=\"stylesheet\" integrity=\"sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3\" crossorigin=\"anonymous\">\n"
              "\t<!-- Option 1: Bootstrap Bundle with Popper -->\n"
              "\t<script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.mins.js\" integrity=\"sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p\" crossorigin=\"anonymous\"></script>\n"
              "    \n"
              "\t<!-- Chart.js -->\n"
              "\t<script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.js\"></script>\n"
              "    \n"
              "\t<title>Estratto conto</title>\n"
              "  </head>\n"
              "  <body>\n"
              "    <!--    <h1>Hello, world!</h1>  -->\n"
              "\t\n"
              "\t\n"
              "    <table class=\"table\">\n"
              "      <thead>\n"
              "        <tr>\n"
              "          <!--  <th scope=\"col\">#</th> -->\n"
              "          <th scope=\"col\">Planned date</th>\n"
              "          <th scope=\"col\">Execution date</th>\n"
              "          <th scope=\"col\">Description</th>\n"
              "          <th scope=\"col\">Amount</th>\n"
              "          <th scope=\"col\">Balance</th>\n"
              "          <th scope=\"col\">Inspection</th>\n"
              "        </tr>\n"
              "      </thead>\n"
              "      <tbody>\n";


    //parse(filename);

    //cout << today << endl;
    //scheduleall();

    //_orders.sort(compare);
    //while (today <= ::end) {
    //    bool flag = true;
    //    for (auto &el: _orders) {
    //        if (el.effective_execution_date == today && !el.cancelled) {
    //            if (flag) {
    //                cout << endl << today << endl;
    //            }
    //            cout << "* " << el.name << endl;
    //            flag = false;
    //            execute(account_balance, el);
    //            reschedule(el);
    //            insert_stat2(account_balance, el.amount);
    //        }
    //    }
    //    if (!flag) {
    //        cout << "--------------------------------" << endl;
    //        _orders.sort(compare);
    //    }

    //    insert_stat(account_balance);
    //    today = today + dd(1);
    //}

    list<double> balances;
    list<datetime> dates;

    cout << endl << "Done: " << "m: " << find_m(balances) << ", q: " << find_q(balances) << endl;

    myfile << "      </tbody>\n"
              "    </table>\n"
              "\n";
    myfile << "    <div style=\"page-break-inside:avoid;page-break-after:always\">\n"
              "    </div>\n"
              "    \n"
              "    <canvas id=\"myChart\" style=\"width:100%;max-width:100%\"></canvas>\n" <<
           "    <script>\n" <<
           "    const ctx = document.getElementById('myChart');\n" <<
           "    const myChart = new Chart(ctx, {\n" <<
           "        type: 'line',\n" <<
           "        data: {\n" <<
           "            labels: " + print_formatted_dates(dates) + ",\n" <<
           "            datasets: [{\n" <<
           "                label: 'Balance',\n" <<
           "                data: " + print_formatted_balances(balances) + ",\n" <<
           "                backgroundColor: 'rgba(255, 206, 86, 0.2)',\n" <<
           "                borderColor: 'rgba(255, 206, 86, 1)',\n"
           "                borderWidth: 1\n"
           "            },\n"
           "            {\n" <<
           "                label: 'Interpolation',\n" <<
           "                data: " + print_formatted_interpolation(balances) + ",\n" <<
           "                borderColor: \"orange\",\n"
           "                fill: false,\n"
           "                borderWidth: 2\n"
           "            }\n"
           "            ]\n"
           "        },\n"
           "        options: {\n" <<
           "            elements: {\n"
           "                point: {\n"
           "                    radius: 0\n"
           "                }\n"
           "            },\n"
           "            scales: {\n" <<
           "                y: {\n" <<
           "                    beginAtZero: true\n" <<
           "                }\n" <<
           "            }\n" <<
           "        }\n" <<
           "    });\n" <<
           "    </script>\n" <<
           "    \n";

    myfile << "    <div style=\"page-break-inside:avoid;page-break-after:always\">\n"
              "    </div>\n"
              "    \n"
              "    <canvas id=\"myChart2\" style=\"width:100%;max-width:100%\"></canvas>\n" <<
           "    <script>\n" <<
           "    const ctx2 = document.getElementById('myChart2');\n" <<
           "    const myChart2 = new Chart(ctx2, {\n" <<
           "        type: 'line',\n" <<
           "        data: {\n" <<
           "            labels: " + print_formatted_dates2(dates) + ",\n" <<
           "            datasets: ["
           "            {\n" <<
           "                label: 'Transactions',\n" <<
           "                data: " + print_formatted_expenses(balances) + ",\n" <<
           "                borderColor: \"orange\",\n"
           "                backgroundColor: 'rgba(255, 206, 86, 0.2)',\n"
           "                fill: false,\n"
           "                borderWidth: 1,\n"
           "                stack: 'combined',\n"
           "                type:'bar'\n"
           "            },"
           "            {\n" <<
           "                label: 'Balances',\n" <<
           "                data: " + print_formatted_balances2(balances) + ",\n" <<
           "                borderColor: \"violet\",\n"
           "                fill: false,\n"
           "                borderWidth: 1\n"
           "            }\n"
           "            ]\n"
           "        },\n"
           "        options: {\n"
           "            elements: {\n"
           "                point: {\n"
           "                    radius: 1.5\n"
           "                }\n"
           "            }, scales: {\n"
           "                y: {\n"
           "                    beginAtZero: true\n"
           "                }\n"
           "            }\n"
           "        }\n"
           "    });\n"
           "</script>";

    myfile << "  </body>\n"
              "</html>";
    myfile.close();

    return 0;
}


