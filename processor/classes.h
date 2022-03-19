#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma ide diagnostic ignored "google-explicit-constructor"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "modernize-use-nodiscard"
//
// Created by Halib on 21.02.2022.
//

#ifndef DATETIME_CLASSES_H
#define DATETIME_CLASSES_H

#include <iomanip>

using namespace std;

class days {
private:
    long long param;
public:
    days(long long _param) : param(_param) {}

    long long operator*(long long factor) const { return param * factor; }
};

class hrs {
private:
    long long param;
public:
    hrs(long long _param) : param(_param) {}

    long long operator*(long long factor) const { return param * factor; }
};

class mins {
private:
    long long param;
public:
    mins(long long _param) : param(_param) {}

    long long operator*(long long factor) const { return param * factor; }
};

class secs {
private:
    long long param;
public:
    secs(long long _param) : param(_param) {}

    long long operator+(long long addendum) const { return param + addendum; }

    long long operator*(long long factor) const { return param * factor; }
};


class period {
private:
    long long days{};
    int hrs{};
    int min{};
    int sec{};

public:

    /**
     * Creates a period in canonical form from @param _seconds.
     * is_canonical_form = (_days <= 0 && _hrs <= 0 && _min <= 0 && _sec <= 0) ||
     *                      (_days >= 0 && _hrs >= 0 && _min >= 0 && _sec >= 0)
     * NB: this is a non-explicit constructor.
     */
    period(long long seconds) {
        days = seconds / 86400;
        long long ss = days * 86400;
        hrs = (seconds - ss) / 3600;
        ss += hrs * 3600;
        min = (seconds - ss) / 60;
        ss += min * 60;
        sec = seconds - ss;
    }

    /**
     * Creates a period in canonical form from @param _sec, @param _min, @param _hrs, @param _days in whatever form.
     * NB: this is a non explicit constructor.
     */

    period(::days _days = 0, ::hrs _hrs = 0, ::mins _min = 0, ::secs _sec = 0) {
        *this = period(_sec + _min * 60 + _hrs * 3600 + _days * 86400);
    }

    /**
     * @return = @this == @pd
     */
    bool operator==(period &pd) const { return !(*this > pd || *this < pd); }

    bool operator==(period &&pd) const { return this->operator==(pd); }

    /**
     * @return = @this != @pd
     */
    bool operator!=(period &dt) const { return !(*this == dt); }

    bool operator!=(period &&dt) const { return this->operator!=(dt); }

    /**
    * @return = @this < @pd
    */
    bool operator<(period &pd) const {
        if (days < pd.days) {
            return true;
        } else if (days == pd.days) {
            if (hrs < pd.hrs) {
                return true;
            } else if (hrs == pd.hrs) {
                if (min < pd.min) {
                    return true;
                } else if (min == pd.min) {
                    if (sec < pd.sec) {
                        return true;
                    } else
                        return false;
                } else
                    return false;
            } else
                return false;
        } else
            return false;
    }

    bool operator<(period &&pd) const { return this->operator<(pd); }

    /**
    * @return = @this > @pd
    */
    bool operator>(period &pd) const {
        if (days > pd.days) {
            return true;
        } else if (days == pd.days) {
            if (hrs > pd.hrs) {
                return true;
            } else if (hrs == pd.hrs) {
                if (min > pd.min) {
                    return true;
                } else if (min == pd.min) {
                    if (sec > pd.sec) {
                        return true;
                    } else
                        return false;
                } else
                    return false;
            } else
                return false;
        } else
            return false;
    }

    bool operator>(period &&pd) const { return this->operator>(pd); }

    /**
    * @return = @this <= @pd
    */
    bool operator<=(period &pd) const { return !(*this > pd); }

    bool operator<=(period &&pd) const { return this->operator<=(pd); }

    /**
    * @return = @this >= @pd
    */
    bool operator>=(period &pd) const { return !(*this < pd); }

    bool operator>=(period &&pd) const { return this->operator>=(pd); }

    /**
    * @return = @this + @pd
    */
    period operator+(period &pd) const { return this->to_seconds() + pd.to_seconds(); }

    period operator+(period &&pd) const { return this->operator+(pd); }

    /**
    * @this = @return = @this < @pd
    */
    period operator+=(period &pd) {
        *this = period(this->to_seconds() + pd.to_seconds());
        return *this;
    }

    period operator+=(period &&pd) {
        return this->operator+=(pd);
    }

    /**
    * @return = @this - @pd
    */
    period operator-(period &pd) const { return this->to_seconds() - pd.to_seconds(); }

    period operator-(period &&pd) const { return this->operator-(pd); }

    /**
    * @this = @return = @this < @pd
    */
    period operator-=(period &pd) {
        *this = period(this->to_seconds() - pd.to_seconds());
        return *this;
    }

    period operator-=(period &&pd) { return this->operator-=(pd); }

    /**
    * @return = @this * @pd
    */
    period operator*(period &pd) const { return this->to_seconds() * pd.to_seconds(); }

    period operator*(period &&pd) const { return this->operator*(pd); }

    /**
    * @this = @return = @this * @pd
    */
    period operator*=(period &pd) {
        *this = this->operator*(pd);
        return *this;
    }

    period operator*=(period &&pd) { return this->operator*=(pd); }

    /**
    * @return = @this / @pd
    */
    period operator/(period &pd) const { return this->to_seconds() / pd.to_seconds(); }

    period operator/(period &&pd) const { return this->operator/(pd); }

    /**
    * @this = @return = @this / @pd
    */
    period operator/=(period &pd) {
        *this = this->operator/(pd);
        return *this;
    }

    period operator/=(period &&pd) { return this->operator/=(pd); }

    /**
     * @return = @this % @pd
     */
    period operator%(period &pd) const { return this->to_seconds() % pd.to_seconds(); }

    period operator%(period &&pd) const { return this->operator%(pd); }

    /**
    * @this = @return = @this % @pd
    */
    period operator%=(period &pd) {
        *this = this->operator%(pd);
        return *this;
    }

    period operator%=(period &&pd) { return this->operator%=(pd); }

    /**
     * Getter functions.
     */
    int get_sec() const { return sec; }

    int get_min() const { return min; }

    int get_hrs() const { return hrs; }

    long long int get_days() const { return days; }

    /**
     * Convert @this to seconds.
     */
    long long to_seconds() const { return days * 86400 + hrs * 3600 + min * 60 + sec; }

};

class datetime_formatter {
public:
    //string format = "www, dd.MMM.yy, hh:mm:ss";
    //string format = "~~~, !!.@@@.##, $$:%%:&&";
    string format = "~~~, !! @@@ ##";
    bool month_str = true;
    bool h24 = true;
    bool keep_original_length = false;
};


int days_of_months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

class datetime {
private:
    int day = 1;
    int month = 1;
    long long year = 1970;
    int hrs = 0;
    int mins = 0;
    int secs = 0;

public:
    datetime_formatter format;

    /**
     * Construct a datetime which is the epoch time
     */
    datetime() = default;

    /**
     * Constructor of datetime. Enforce the month to be valid (1 <= _month <= 12)
     * and then fixes (@fix_date) the date accordingly.
     */
    datetime(int _day, int _month, long long _year, bool autofix) :
            day(_day), month(_month), year(_year) {
        if (_month < 1 || _month > 12)
            throw runtime_error("");
        if (_day > days_of_this_month())
            this->day = days_of_this_month();
    }

    /**
     * Constructor of datetime. Enforce the following constraints: 1 <= _month <= 12, 0 <= ...
     * and then fixes (@fix_date) the date accordingly.
     */
    datetime(int _day, int _month, long long _year, int _hrs, int _min, int _sec,
             bool autofix)
            : secs(_sec), mins(_min), hrs(_hrs), day(_day), month(_month), year(_year) {
        if (_sec < 0 || _sec > 59)
            throw runtime_error("");
        if (_min < 0 || _min > 59)
            throw runtime_error("");
        if (_hrs < 0 || _hrs > 23)
            throw runtime_error("");
        if (_month < 1 || _month > 12)
            throw runtime_error("");
        if (_day > days_of_this_month())
            this->day = days_of_this_month();
    }

    /**
     * Constructor of datetime. Enforce the month to be valid (1 <= _month <= 12) and the day to be valid...
     */
    datetime(long long _day, long long _month, long long _year) :
            day(_day), month(_month), year(_year) {
        if (_month < 1 || _month > 12)
            throw runtime_error("");
        if (_day < 1 || _day > days_of_this_month())
            throw runtime_error("");
    }

    /**
     * Constructor of datetime. Enforce the following constraints: 1 <= _month <= 12, 0 <= ...
     */
    datetime(int _day, int _month, long long _year, int _hrs, int _min, int _sec) :
            secs(_sec), mins(_min), hrs(_hrs), day(_day), month(_month), year(_year) {
        if (_sec < 0 || _sec > 59)
            throw runtime_error("");
        if (_min < 0 || _min > 59)
            throw runtime_error("");
        if (_hrs < 0 || _hrs > 23)
            throw runtime_error("");
        if (_month < 1 || _month > 12)
            throw runtime_error("");
        if (_day > days_of_this_month())
            throw runtime_error("");
    }

    /**
    * Construct a new date which is @param seconds after epoch time.
    */
    explicit datetime(long long timestamp) { *this = after(timestamp); }

private:
    /**
     * Auxiliary function for @f.
     */
    static long long fK(int PERIOD, long long x, long long y) {
        if (x >= 0)
            y -= PERIOD * ((PERIOD + x - 1) / PERIOD);
        else
            y -= PERIOD * (x / PERIOD);

        if (y <= 0)
            return y / PERIOD;
        else
            return 1 + (y - 1) / PERIOD;

    }

    /**
     * @return the number of days from 01.01.@param x 00:00:00 to 01.01.@param y 00:00:00
     */
    static long long f(long long x, long long y) { return (y - x) * 365 + fK(4, x, y) - fK(100, x, y) + fK(400, x, y); }

    static int days_of_month(unsigned int _month, long long _year) {
        return ( //if leap _year add 1 day to the normal number of days of February.
                       ((_year % 400 == 0) || (_year % 4 == 0 && _year % 100 != 0)) && _month == 2 ? 1 : 0
               ) +
               days_of_months[_month - 1];
    }

    /**
     * @return = @param start + @param seconds
     */
    datetime after(long long seconds) const {

        long long start_timestamp = this->to_timestamp();
        long long yyear = 1970 + ((start_timestamp + seconds) / (365.2425 * 86400));
        period from_dt(start_timestamp + seconds - datetime(1, 1, yyear).to_timestamp());

        int _sec = from_dt.get_sec();
        int _min = 0;
        int _hrs = 0;
        long long _day = 1;
        int _month = 1;
        long long _year = yyear;

        if (_sec < 0) {
            _year--;
            _month = 12;
            _day = 31;
            _hrs = 23;
            _min = 59;
            _sec += 60;
        }

        _min += from_dt.get_min();
        if (_min < 0) { // here mins and secs can be anything
            _hrs--;
            if (_hrs == -1) { //here hrs can be either -1 or 22
                _day--;
                if (_day == 0) { //here day can be either 0 or 30
                    _month--;
                    if (_month == 0) { // here month can be either 0 or 11
                        _year--;
                        _month = 12;
                    }
                    _day = days_of_month(_month, _year);
                }
                _hrs = 23;
            }
            _min += 60;
        }

        _hrs += from_dt.get_hrs();
        if (_hrs < 0) {
            _day--;
            if (_day == 0) {
                _month--;
                if (_month == 0) {
                    _year--;
                    _month = 12;
                }
                _day = days_of_month(_month, _year);
            }
            _hrs += 24;
        }

        _day += from_dt.get_days();
        int _dom = days_of_month(_month, _year);
        while (_day > _dom) {
            _day -= _dom;
            _month++;
            if (_month == 13) {
                _year++;
                _month = 1;
            }
            _dom = days_of_month(_month, _year);
        }
        while (_day <= 0) {
            _month--;
            if (_month == 0) {
                _year--;
                _month = 12;
            }
            _day += days_of_month(_month, _year);
        }

        return {static_cast<int>(_day), _month, _year, _hrs, _min, _sec};
    }

    /**
     * @return = @param end - @param start
     */
    long long seconds_to(const datetime &end) const {

        datetime start = *this;
        int flag = (end > start ? 1 : -1);
        datetime dt1 = (end > start ? start : end);
        datetime dt2 = (end > start ? end : start);
        long long dd = 0;

        if (dt1.year == dt2.year) {
            if (dt1.month == dt2.month) {
                dd += dt2.day - dt1.day;
            } else {
                dd += dt1.days_of_this_month() - dt1.day;
                //because of the check on dt2 it is not a problem if ddd.month==13
                for (datetime ddd(1, dt1.month + 1, dt1.year); ddd.month < dt2.month; ddd.month++)
                    dd += ddd.days_of_this_month();
                dd += dt2.day;
            }
        } else {

            dd += f(dt1.year, dt2.year);

            datetime ddd(1, 1, dt1.year);
            for (ddd.month = 1; ddd.month < dt1.month; ddd.month++)
                dd -= ddd.days_of_this_month();
            ddd = datetime(1, 1, dt2.year);
            for (ddd.month = 1; ddd.month < dt2.month; ddd.month++)
                dd += ddd.days_of_this_month();

            dd -= dt1.day;
            dd += dt2.day;
        }

        long long ss = dt2.secs - dt1.secs;
        long long mm = dt2.mins - dt1.mins;
        long long hh = dt2.hrs - dt1.hrs;

        return (ss + mm * 60 + hh * 3600 + dd * 86400) * flag;
    }

public:
    /**
     * @return = @this == @dt
     */
    bool operator==(datetime &dt) const { return !(*this < dt || *this > dt); }

    bool operator==(datetime &&dt) const { return *this == dt; }

    /**
     * @return = @this != @dt
     */
    bool operator!=(datetime &dt) const { return !(*this == dt); }

    bool operator!=(datetime &&dt) const { return *this != dt; }

    /**
     * @return = @this < @dt
     */
    bool operator<(datetime &dt) const {
        if (year < dt.year) {
            return true;
        } else if (year == dt.year) {
            if (month < dt.month) {
                return true;
            } else if (month == dt.month) {
                if (day < dt.day) {
                    return true;
                } else if (day == dt.day) {
                    if (hrs < dt.hrs) {
                        return true;
                    } else if (hrs == dt.hrs) {
                        if (mins < dt.mins) {
                            return true;
                        } else if (mins == dt.mins) {
                            if (secs < dt.secs) {
                                return true;
                            } else
                                return false;
                        } else
                            return false;
                    } else
                        return false;
                } else
                    return false;
            } else
                return false;
        } else
            return false;
    }

    bool operator<(datetime &&dt) const { return this->operator<(dt); }

    /**
     * @return = @this > @dt
     */
    bool operator>(datetime &dt) const {
        if (year > dt.year) {
            return true;
        } else if (year == dt.year) {
            if (month > dt.month) {
                return true;
            } else if (month == dt.month) {
                if (day > dt.day) {
                    return true;
                } else if (day == dt.day) {
                    if (hrs > dt.hrs) {
                        return true;
                    } else if (hrs == dt.hrs) {
                        if (mins > dt.mins) {
                            return true;
                        } else if (mins == dt.mins) {
                            if (secs > dt.secs) {
                                return true;
                            } else
                                return false;
                        } else
                            return false;
                    } else
                        return false;
                } else
                    return false;
            } else
                return false;
        } else
            return false;
    }

    bool operator>(datetime &&dt) const { return this->operator>(dt); }

    /**
     * @return = @this <= @dt
     */
    bool operator<=(datetime &d2) const { return !(*this > d2); }

    bool operator<=(datetime &&d2) const { return this->operator<=(d2); }

    /**
     * @return = @this >= @dt
     */
    bool operator>=(datetime &d2) const { return !(*this < d2); }

    bool operator>=(datetime &&d2) const { return this->operator>=(d2); }

    /**
     * @return = @this + @p
     */
    datetime operator+(period &p) const { return after(p.to_seconds()); }

    datetime operator+(period &&p) const { return operator+(p); }

    /**
     * @this = @return = @this + @p
     */
    datetime operator+=(period &p) {
        *this = operator+(p);
        return *this;
    }

    datetime operator+=(period &&p) { return operator+=(p); }

    /**
     * @return = @this - @p
     */
    datetime operator-(period &p) const { return after(-p.to_seconds()); }

    datetime operator-(period &&p) const { return operator-(p); }

    /**
     * @this = @return = @this - @p
     */
    datetime operator-=(period &p) {
        *this = this->operator-(p);
        return *this;
    }

    datetime operator-=(period &&p) { return this->operator-=(p); }

    /**
     * @return = @this - @dt
     */
    period operator-(datetime &dt) const { return seconds_from(dt); }

    period operator-(datetime &&dt) const { return operator-(dt); }

    /**
     * Getter functions.
     */
    long long int get_sec() const { return secs; }

    long long int get_min() const { return mins; }

    long long int get_hrs() const { return hrs; }

    long long int get_day() const { return day; }

    long long int get_month() const { return month; }

    long long int get_year() const { return year; }

    /**
     * @return is the number of months from @this and @param dt regardless of the days,
     * e.g. (5.1.2020).months_between(3.2.2020) =====> 1.
     */
    long long months_between(datetime &dt) const { return 12 * (dt.year - year) + dt.month - month; }

    /**
     * @return =  @this after @param n years. The obtained date is adjusted to return the last of the when it overflows,
     * e.g. (31.1.2020).after_months(1) =====> 28.1.2020
     */
    datetime after_months(long long n) const {
        datetime dt = *this;
        dt.year += n / 12;
        dt.month += n - ((n / 12) * 12);

        if (dt.month > 12) {
            dt.year++;
            dt.month -= 12;
        }

        if (dt.month <= 0) {
            dt.year--;
            dt.month += 12;
        }

        return dt.fix_date();
    }

    /**
     * @return is the number of years from @this and @param dt regardless of the days,
     * e.g. (5.1.2020).months_between(3.8.2021) =====> 1.
     */
    long long years_between(datetime &dt) const { return dt.year - year; }

    /**
     * @return =  @this after @param n years.
     */
    datetime after_years(long long n) const {
        datetime dt = *this;
        dt.year += n;
        return dt;
    }

    /**
     * The date is fixed in the following way: given a month, if the day is greater than the days of the specified month,
     * the day is set to the last day of that month. Leap years are taken into account (February has 29 days).
     * @return: the fixed date.
     */
    datetime fix_date() const {
        datetime dt = *this;
        if (dt.day > dt.days_of_this_month())
            dt.day = dt.days_of_this_month();
        return dt;
    }

    /**
     * @return the week_day of @this.
     */
    int get_week_day() const {
        long long tt = this->to_timestamp();
        long long res;
        if (tt >= 0) {
            res = (4 + (tt / 86400)) % 7;
        } else {
            res = (4 + 7 + (((tt - 86399) / 86400) % 7)) % 7;
        }
        return res; // NOLINT(cppcoreguidelines-narrowing-conversions)
    }

    /**
     * @return the first working day (today is included in the computation).
     */
    datetime first_working_day() const {
        int wd = this->get_week_day();

        if (wd == 6)
            return *this + days(2);
        else if (wd == 0)
            return *this + days(1);
        else
            return *this;
    }

    /**
     * @return the number of days of @this year.
     */
    int days_of_this_year() const { return 365 + ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)); }

    /**
     * @return the number of days of @this month.
     */
    int days_of_this_month() const {
        return ( //if leap year add 1 day to the normal number of days of February.
                       ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)) && month == 2 ? 1 : 0
               ) +
               days_of_months[month - 1];
    }

    /**
     * @return @this whose day is the last of the month.
     */
    datetime end_of_month() const {
        datetime dt = *this;
        dt.day = dt.days_of_this_month();
        return dt;
    }

    /**
     * @return the seconds from @epoch to @this.
     */
    long long to_timestamp() const { return seconds_from(datetime()); }

    /**
     * @return the seconds from @d2 to @this.
     */
    long long seconds_from(const datetime &d2) const { return d2.seconds_to(*this); }

};

/**
 * @return = -@p
 */
period operator-(period &p) { return -p.to_seconds(); }

period operator-(period &&p) { return -p; }

void replace(string &input, const string &from, const string &to) {
    unsigned int pos = 0;
    while (true) {
        size_t startPosition = input.find(from, pos);
        if (startPosition == string::npos)
            return;
        input.replace(startPosition, from.length(), to);
        pos += to.length();
    }
}

string to_week_day(int wk) {
    switch (wk) {
        case 0:
            return "Sunday";
        case 1:
            return "Monday";
        case 2:
            return "Tuesday";
        case 3:
            return "Wednesday";
        case 4:
            return "Thursday";
        case 5:
            return "Friday";
        case 6:
            return "Saturday";
        default:
            throw runtime_error("");
    }
}

string to_month(int mm) {
    switch (mm) {
        case 1:
            return "January";
        case 2:
            return "February";
        case 3:
            return "March";
        case 4:
            return "April";
        case 5:
            return "May";
        case 6:
            return "June";
        case 7:
            return "July";
        case 8:
            return "August";
        case 9:
            return "September";
        case 10:
            return "October";
        case 11:
            return "November";
        case 12:
            return "December";
        default:
            throw runtime_error("");
    }
}

std::ostream &operator<<(std::ostream &os, datetime const &dd) {
    string output = dd.format.format;
    bool month_str = dd.format.month_str;
    bool h24 = dd.format.h24;
    bool keep_original_length = dd.format.keep_original_length;

    unsigned int num = output.find_last_of('~') - output.find('~') + 1;
    std::string W = to_week_day(dd.get_week_day());
    if (!keep_original_length) {
        W = W.substr(0, num);
        W += std::string(num - W.length(), ' ');
    }
    replace(output, std::string(num, '~'), W);

    num = output.find_last_of('!') - output.find('!') + 1;
    std::string D = to_string(dd.get_day());
    if (!keep_original_length) {
        D.insert(0, num - D.length(), '0');
    }
    replace(output, std::string(num, '!'), D);

    num = output.find_last_of('@') - output.find('@') + 1;
    std::string M;
    if (month_str) {
        M = to_month(dd.get_month()); // NOLINT(cppcoreguidelines-narrowing-conversions)
        if (!keep_original_length) {
            M = M.substr(0, num);
            M += std::string(num - M.length(), ' ');
        }
    } else {
        M = to_string(dd.get_month());
        if (!keep_original_length)
            M.insert(0, num - M.length(), '0');
    }
    replace(output, std::string(num, '@'), M);

    num = output.find_last_of('#') - output.find('#') + 1;
    std::string Y = to_string(abs(dd.get_year()));
    if (!keep_original_length) {
        if (dd.get_year() >= 0) {
            if (Y.length() > num) {
                Y = Y.substr(Y.length() - num, num);
            } else {
                Y.insert(0, num - Y.length(), '0');
            }
        } else {
            if (Y.length() > num) {
                Y = Y.substr(Y.length() - num + 1, num);
            } else if (Y.length() == num) {
                Y = Y.substr(1, num);
            } else {
                Y.insert(0, num - Y.length() - 1, '0');
            }
            Y = "-" + Y;
        }
    }
    replace(output, std::string(num, '#'), Y);

    num = output.find_last_of('$') - output.find('$') + 1;
    std::string h;
    if (!h24) {
        if (dd.get_hrs() > 12) {
            h = to_string(dd.get_hrs() - 12);
            output += " PM";
        } else {
            h = to_string(dd.get_hrs());
            output += " AM";
        }
    } else {
        h = to_string(dd.get_hrs());
    }
    if (!keep_original_length)
        h.insert(0, num - h.length(), '0');
    replace(output, std::string(num, '$'), h);

    num = output.find_last_of('%') - output.find('%') + 1;
    std::string m = to_string(dd.get_min());
    if (!keep_original_length)
        m.insert(0, num - m.length(), '0');
    replace(output, std::string(num, '%'), m);

    num = output.find_last_of('&') - output.find('&') + 1;
    std::string s = to_string(dd.get_sec());
    if (!keep_original_length)
        s.insert(0, num - s.length(), '0');
    replace(output, std::string(num, '&'), s);

    return os << output;
}

std::ostream &operator<<(std::ostream &os, period const &d) {
    return os << "Days: " << std::setfill('0') << std::setw(2) << d.get_days() << ", hrs: " << std::setfill('0')
              << std::setw(2) << d.get_hrs() << ", mins: " << std::setfill('0') << std::setw(2) << d.get_min()
              << ", secs: "
              << std::setfill('0') << std::setw(2) << d.get_sec();
}

#endif //DATETIME_CLASSES_H

#pragma clang diagnostic pop
