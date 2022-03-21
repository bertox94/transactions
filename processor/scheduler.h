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

    order(unordered_map<string, string> map) {

        if (map.size() == 7) {
            single_order = true;
            name = map["descr"];
            is_wire_transfer = map["wt"] == "true";
            amount = stol(map["amount"]);
            planned_execution_date = datetime(stol(map["day"]), stol(map["month"]), stol(map["year"]));
        } else if (map.size() == 14) {
            single_order = false;
            name = map["descr"];
            f1 = stoi(map["f1"]);
            is_wire_transfer = map["wt"] == "true";
            f2 = map["f2"];
            opti = map["opt1"];
            if (map["opt1"] == "default") {
                initial_day = stol(map["day1"]);
                initial_month = stol(map["month1"]);
            } else if (map["opt1"] == "eom")
                initial_month = stol(map["month1"]);
            initial_year = stol(map["year1"]);
            optf = map["opt2"];
            if (map["day2"] == "$" && map["month2"] == "$" && map["year2"].empty()) {
                repeated_order_with_final_date = false;
            } else {
                repeated_order_with_final_date = true;
                if (map["opt2"] == "default") {
                    final_day = stol(map["day2"]);
                    final_month = stol(map["month2"]);
                } else if (map["opt2"] == "eom")
                    final_month = stol(map["month2"]);
                final_year = stol(map["year2"]);
            }
            amount = stol(map["amount"]);
        }
    }


    string name;
    bool is_wire_transfer;
    bool end_of_month_i;
    long long initial_day;
    long long initial_month;
    long long initial_year;
    long long final_day;
    long long final_month;
    long long final_year;
    datetime planned_execution_date;
    datetime effective_execution_date;
    bool repeated_order_with_final_date = false;
    bool single_order = false;
    bool cancelled = false;
    bool scheduled = false;
    int f1;
    string f2;
    double amount;
    string opti;
    string optf;
};

list<datetime> dates;
list<datetime> dates2;
list<double> balances;
list<double> balances2;
list<double> transactions;

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

//suppose the JSON is valid
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

void insert_stat(double bal, datetime today) {
    dates.push_back(today);
    balances.push_back(bal);
}

void insert_stat2(double bal, double exp, datetime today) {
    transactions.push_back(exp);
    balances2.push_back(bal);
    dates2.push_back(today);
}

double find_m() {
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

double find_q() {
    double v1 = 0, v2 = 0;
    int i = 0;
    for (auto el: balances) {
        v1 += el;
        v2 += i;
        i++;
    }
    v2 *= find_m();

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
    if (!el.single_order) {
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

    if (el.single_order) {
        if (el.effective_execution_date < today) {
            //cout << right << setw(14) << std::setfill(' ') << "Cancelled "
            //     << (el.single_order ? "     " : " (R) ") << setw(24) << el.name << endl;
            el.cancelled = true;
            return "Expired";
        }
    }

    datetime final_date = get_date(el.optf, el.final_day, el.final_month, el.final_year);
    if (el.repeated_order_with_final_date) {
        if (el.effective_execution_date > final_date) {
            //cout << right << setw(14) << std::setfill(' ') << "Cancelled "
            //     << (el.single_order ? "     " : " (R) ") << setw(24) << el.name << endl;
            el.cancelled = true;
            return "Expired";
        }
    }

    //cout << right << setw(14) << std::setfill(' ') << "Scheduled " << (el.single_order ? "     " : " (R) ")
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

    if (el.single_order) {
        //cout << right << setw(25) << std::setfill(' ') << "Stopped " << endl;
        el.cancelled = true;
        return;
    }

    if (el.f2 == "days") {
        el.planned_execution_date += dd(el.f1);
    } else if (el.f2 == "months") {
        if (el.end_of_month_i) {
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

string execute(double &d, order &el) {
    d += el.amount;
    string warn = " style = \"color:  red; font-weight: bold;\"";
    string bold = " style = \"font-weight: bold;\"";
    stringstream ss;
    ss <<
       "        <tr>\n" <<
       "          <!-- <th scope=\"row\">1</th> -->\n" <<
       "          <td" << (d < 0 ? warn : "") << ">" << el.planned_execution_date << "</td>\n" <<
       "          <td"
       << (d < 0 ? warn : (el.planned_execution_date != el.effective_execution_date ? bold : ""))
       << ">" << el.effective_execution_date << "</td>\n" <<
       "          <td" << (d < 0 ? warn : "") << ">" << el.name << "</td>\n" <<
       "          <td" << (d < 0 ? warn : "") << ">" << fixed << std::setprecision(2) << el.amount
       << "</td>\n" <<
       "          <td" << (d < 0 ? warn : "") << ">" << d << "</td>\n" <<
       "          <td" << (d < 0 ? warn : "") << ">" << ((d == 0 ? "ALERT" : (d < 0 ? "FAILURE" : "OK")))
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

    for (; !param.empty(); param = param.substr(param.find('\n') + 1))
        orders.emplace_back(JSONtomap(param.substr(0, param.find('\n'))));

    datetime today(20, 03, 2022);
    string resp;
    scheduleall(orders, today);

    for (auto it = orders.begin(); it != orders.end() && it->effective_execution_date <= enddate;) {
        cout << it->effective_execution_date << endl;
        execute(account_balance, *it);
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


//while (today <= enddate) {
//    bool flag = true;
//    for (auto &el: orders) {
//        if (el.effective_execution_date == today && !el.cancelled) {
//            if (flag) {
//                cout << endl << today << endl;
//            }
//            cout << "* " << el.name << endl;
//            flag = false;
//            resp += execute(account_balance, el);
//            reschedule(el);
//            insert_stat2(account_balance, el.amount, today);
//        }
//    }
//    if (!flag) {
//        cout << "--------------------------------" << endl;
//        orders.sort(compare);
//    }
//
//    insert_stat(account_balance, today);
//    today = today + dd(1);
//}


    return
            resp;


}

string print_formatted_balances() {
    string str = "[";

    for (auto el: balances) {
        str += "'";
        str += to_string(el);
        str += "', ";
    }

    str += "]";
    return str;
}

string print_formatted_balances2() {
    string str = "[";

    for (auto el: balances2) {
        str += "'";
        str += to_string(el);
        str += "', ";
    }

    str += "]";
    return str;
}

string print_formatted_expenses() {
    string str = "[";

    for (auto el: transactions) {
        str += "'";
        str += to_string(el);
        str += "', ";
    }

    str += "]";
    return str;
}

string print_formatted_interpolation() {
    double m = find_m();
    double q = find_q();
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

string print_formatted_dates() {
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

string print_formatted_dates2() {
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


    cout << endl << "Done: " << "m: " << find_m() << ", q: " << find_q() << endl;

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
           "            labels: " + print_formatted_dates() + ",\n" <<
           "            datasets: [{\n" <<
           "                label: 'Balance',\n" <<
           "                data: " + print_formatted_balances() + ",\n" <<
           "                backgroundColor: 'rgba(255, 206, 86, 0.2)',\n" <<
           "                borderColor: 'rgba(255, 206, 86, 1)',\n"
           "                borderWidth: 1\n"
           "            },\n"
           "            {\n" <<
           "                label: 'Interpolation',\n" <<
           "                data: " + print_formatted_interpolation() + ",\n" <<
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
           "            labels: " + print_formatted_dates2() + ",\n" <<
           "            datasets: ["
           "            {\n" <<
           "                label: 'Transactions',\n" <<
           "                data: " + print_formatted_expenses() + ",\n" <<
           "                borderColor: \"orange\",\n"
           "                backgroundColor: 'rgba(255, 206, 86, 0.2)',\n"
           "                fill: false,\n"
           "                borderWidth: 1,\n"
           "                stack: 'combined',\n"
           "                type:'bar'\n"
           "            },"
           "            {\n" <<
           "                label: 'Balances',\n" <<
           "                data: " + print_formatted_balances2() + ",\n" <<
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


