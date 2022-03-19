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
#include <string>

using namespace std;

class dd {
private:
    long long param;
public:
    explicit dd(long long _param) : param(_param) {}

    long long int get() const { return param; }
};

class hh {
private:
    long long param;
public:
    explicit hh(long long _param) : param(_param) {}

    long long int get() const { return param; }
};

class mm {
private:
    long long param;
public:
    explicit mm(long long _param) : param(_param) {}

    long long int get() const { return param; }
};

class ss {
private:
    long long param;
public:
    explicit ss(long long _param) : param(_param) {}

    long long int get() const { return param; }
};


class period {
private:
    long long days{};
    long long hours{};
    long long minutes{};
    long long seconds{};

public:

    period() = default;

    /**
     * Creates a period from @param _sec, @param _min, @param _hrs, @param _days in whatever form.
     * NB: this is a non explicit constructor.
     */

    period(::ss _sec) : seconds(_sec.get()) {};

    period(::mm _mins) : minutes(_mins.get()) {};

    period(::mm _mins, ::ss _sec) : minutes(_mins.get()), seconds(_sec.get()) {};

    period(::hh _hrs) : hours(_hrs.get()) {};

    period(::hh _hrs, ::mm _min) : hours(_hrs.get()), minutes(_min.get()) {};

    period(::hh _hrs, ::mm _min, ::ss _sec) : hours(_hrs.get()), minutes(_min.get()), seconds(_sec.get()) {};

    period(::dd _days) : days(_days.get()) {};

    period(::dd _days, ::hh _hrs) : days(_days.get()), hours(_hrs.get()) {};

    period(::dd _days, ::mm _min) : days(_days.get()), minutes(_min.get()) {};

    period(::dd _days, ::ss _sec) : days(_days.get()), seconds(_sec.get()) {};

    period(::dd _days, ::hh _hrs, ::mm _min) : days(_days.get()), hours(_hrs.get()), minutes(_min.get()) {};

    period(::dd _days, ::hh _hrs, ::ss _sec) : days(_days.get()), hours(_hrs.get()), seconds(_sec.get()) {};

    period(::dd _days, ::hh _hrs, ::mm _min, ::ss _sec) : days(_days.get()), hours(_hrs.get()), minutes(_min.get()),
                                                          seconds(_sec.get()) {}

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
        return to_seconds() < pd.to_seconds();
    }

    bool operator<(period &&pd) const { return this->operator<(pd); }

    /**
    * @return = @this > @pd
    */
    bool operator>(period &pd) const {
        return to_seconds() > pd.to_seconds();
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
    period operator+(period &pd) const { return ss(this->to_seconds() + pd.to_seconds()); }

    period operator+(period &&pd) const { return this->operator+(pd); }

    /**
    * @this = @return = @this < @pd
    */
    period operator+=(period &pd) {
        *this = ss(this->to_seconds() + pd.to_seconds());
        return *this;
    }

    period operator+=(period &&pd) { return this->operator+=(pd); }

    /**
    * @return = @this - @pd
    */
    period operator-(period &pd) const { return ss(this->to_seconds() - pd.to_seconds()); }

    period operator-(period &&pd) const { return this->operator-(pd); }

    /**
    * @this = @return = @this < @pd
    */
    period operator-=(period &pd) {
        *this = ss(this->to_seconds() - pd.to_seconds());
        return *this;
    }

    period operator-=(period &&pd) { return this->operator-=(pd); }

    /**
    * @return = @this * @pd
    */
    period operator*(period &pd) const { return ss(this->to_seconds() * pd.to_seconds()); }

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
    period operator/(period &pd) const { return ss(this->to_seconds() / pd.to_seconds()); }

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
    period operator%(period &pd) const { return ss(this->to_seconds() % pd.to_seconds()); }

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
    long long getSec() const { return seconds; }

    long long getMin() const { return minutes; }

    long long getHrs() const { return hours; }

    long long int getDays() const { return days; }

    void setDays(long long _days) { period::days = _days; }

    void setHrs(long long _hrs) { period::hours = _hrs; }

    void setMin(long long _min) { period::minutes = _min; }

    void setSec(long long _sec) { period::seconds = _sec; }

    /**
     * Creates a period in canonical form from @param _seconds.
     * A period is in canonical form iff (_days <= 0 && _hrs <= 0 && _min <= 0 && _sec <= 0) ||
     *                              (_days >= 0 && _hrs >= 0 && _min >= 0 && _sec >= 0)
     * NB: this is a non-explicit constructor.
     */
    period to_canonical_form() const {
        period pd;
        long long _seconds = this->to_seconds();
        pd.days = _seconds / 86400;
        long long ss = pd.days * 86400;
        pd.hours = (_seconds - ss) / 3600;
        ss += pd.hours * 3600;
        pd.minutes = (_seconds - ss) / 60;
        ss += pd.minutes * 60;
        pd.seconds = _seconds - ss;
        return pd;
    }

    /**
     * Convert @this to ss.
     */
    long long to_seconds() const { return days * 86400 + hours * 3600 + minutes * 60 + seconds; }

    period strip_days() const {
        auto el = *this;
        el.days = 0;
        return el;
    }

};

/**
 * www, dd.MMM.yy, hh:mm:ss
 * ~~~, !!.@@@.##, $$:%%:&&
 */
class datetime_formatter {
public:
    string format = "~~~, !! @@@. ##";
    bool month_str = true;
    bool h24 = true;
    bool keep_original_length = false;
};


int days_of_months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

class datetime {
private:
    //do not use unsigned to avoid bad surprises on narrowing etc. casting!
    long long day = 0;
    long long month = 0;
    long long year = 1970;
    long long hrs = 0;
    long long min = 0;
    long long sec = 0;

public:
    datetime_formatter format;

    /**
     * Construct a datetime which is the epoch time
     */
    datetime() = default;

    /**
     * Construct a new date which is @param seconds after epoch time.
     */
    explicit datetime(long long timestamp) { *this = after(timestamp); }

    /**
     * Constructor of datetime. Enforce the month to be valid (1 <= _month <= 12) and the day to be valid...
     */
    datetime(long long _day, long long _month, long long _year) :
            day(_day - 1), month(_month - 1), year(_year) {}

    /**
     * Constructor of datetime. Enforce the following constraints: 1 <= _month <= 12, 0 <= ...
     */
    datetime(long long _day, long long _month, long long _year, long long _hrs, long long _min, long long _sec) :
            datetime(_day, _month, _year) {
        sec = _sec;
        min = _min;
        hrs = _hrs;
    }

private:
    /**
     * Auxiliary function for @f.
     */
    static long long fK(long long x, long long y) {
        long long y1 = y - 4 * (((x >= 0) * 3 + x) / 4);
        long long y2 = y - 100 * (((x >= 0) * 99 + x) / 100);
        long long y3 = y - 400 * (((x >= 0) * 399 + x) / 400);

        return (y1 > 0) + (y1 - (y1 > 0)) / 4
               - ((y2 > 0) + (y2 - (y2 > 0)) / 100)
               + (y3 > 0) + (y3 - (y3 > 0)) / 400;
    }

    /**
     * @return the number of dd from 01.01.@param x 00:00:00 to 01.01.@param y 00:00:00
     */
    static long long f(long long x, long long y) { return (y - x) * 365 + fK(x, y); }

    static int days_of_month(unsigned int _month, long long _year) {
        return ( //if leap _year add 1 day to the normal number of dd of February.
                       _month == 1 && ((_year % 400 == 0) || (_year % 4 == 0 && _year % 100 != 0))
               ) + days_of_months[_month];
    }

    /**
     * @return = @param start + @param seconds
     */
    datetime after(long long seconds) const {

        long long sts = this->to_timestamp() + seconds;
        long long _year = 1970 + (sts / (365.2425 * 86400)); // NOLINT(cppcoreguidelines-narrowing-conversions)
        auto secs = sts - datetime(1, 1, _year).to_timestamp();

        long long _day = secs / 86400;
        long long time_sec = secs - _day * 86400;
        if (time_sec < 0) {
            time_sec += 86400;
            _day--;
        }
        period time = ss(time_sec);
        time = time.to_canonical_form();

        int _month = 0;

        int _dom = days_of_month(_month, _year);
        while (_day >= _dom) {
            _day -= _dom;
            _month++;
            if (_month == 12) {
                _year++;
                _month = 0;
            }
            _dom = days_of_month(_month, _year);
        }
        while (_day < 0) {
            _month--;
            if (_month == -1) {
                _year--;
                _month = 11;
            }
            _day += days_of_month(_month, _year);
        }

        return {_day + 1, _month + 1, _year, time.getHrs(), time.getMin(), time.getSec()};
    }

    /**
     * @return = @param end - @param start
     */
    long long seconds_to(const datetime &end) const {

        int flag = (end > *this ? 1 : -1);
        datetime dt1 = (end > *this ? *this : end);
        datetime dt2 = (end > *this ? end : *this);
        long long dd = f(dt1.year, dt2.year) + dt2.day - dt1.day;

        for (int _month = 0; _month < dt1.month; _month++)
            dd -= days_of_month(_month, dt1.year);
        for (int _month = 0; _month < dt2.month; _month++)
            dd += days_of_month(_month, dt2.year);

        return (dt2.sec - dt1.sec + (dt2.min - dt1.min) * 60 + (dt2.hrs - dt1.hrs) * 3600 + dd * 86400) * flag;
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
        return year < dt.year || year == dt.year && (
                month < dt.month || month == dt.month && (
                        day < dt.day || day == dt.day && (
                                hrs < dt.hrs || hrs == dt.hrs && (
                                        min < dt.min || min == dt.min && sec < dt.sec
                                )
                        )
                )
        );
    }

    bool operator<(datetime &&dt) const { return this->operator<(dt); }

    /**
     * @return = @this > @dt
     */
    bool operator>(const datetime &dt) const {
        return year > dt.year || year == dt.year && (
                month > dt.month || month == dt.month && (
                        day > dt.day || day == dt.day && (
                                hrs > dt.hrs || hrs == dt.hrs && (
                                        min > dt.min || min == dt.min && sec > dt.sec
                                )
                        )
                )
        );
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
    period operator-(datetime &dt) const { return ss(seconds_from(dt)); }

    period operator-(datetime &&dt) const { return operator-(dt); }

    bool isOK() const {
        return !(month < 0 || month >= 12) &&
               !(day < 0 || day >= days_of_this_month()) &&
               !(hrs < 0 || hrs >= 60) &&
               !(min < 0 || min >= 60) &&
               !(sec < 0 || sec >= 60);
    }

    datetime fix(bool _default = true) const {
        datetime dt = *this;
        dt.hrs = 0;
        dt.min = 0;
        dt.sec = 0;

        dt.year += dt.month / 12;
        dt.month %= 12;

        if (dt.day >= dt.days_of_this_month()) {
            long long _day = dt.day;
            dt.day = dt.days_of_this_month() - 1;
            if (!_default)
                dt += dd(_day - dt.days_of_this_month() + 1);
        } else if (dt.day < 0) {
            long long _day = dt.day;
            dt.day = 0;
            if (!_default)
                dt += dd(_day);
        }

        period pd = extract_time_of_day();
        return dt + pd;
    }

    /**
     * Getter functions.
     */
    long long int getSec() const { return sec; }

    long long int getMin() const { return min; }

    long long int getHrs() const { return hrs; }

    long long int getDay() const { return day + 1; }

    long long int getMonth() const { return month + 1; }

    long long int getYear() const { return year; }

    /**
     * @return is the number of months from @this and @param dt regardless of the dd,
     * e.g. (5.1.2020).months_between(3.2.2020) =====> 1.
     */
    long long months_between(datetime &dt) const { return 12 * (dt.year - year) + dt.month - month; }

    /**
     * @return =  @this after @param n years. The obtained date is not checked against overflows,
     * e.g. (31.1.2020).after_months(1) =====> 31.1.2020
     */
    datetime after_months(long long n) const {
        datetime dt = *this;
        dt.year += n / 12;
        dt.month += n % 12; // NOLINT(cppcoreguidelines-narrowing-conversions)

        if (dt.month > 11) {
            dt.year++;
            dt.month -= 12;
        }

        if (dt.month < 0) {
            dt.year--;
            dt.month += 12;
        }
        return dt;
    }

    /**
     * @return is the number of years from @this and @param dt regardless of the dd,
     * e.g. (5.1.2020).months_between(3.8.2021) =====> 1.
     */
    long long years_between(datetime &dt) const { return dt.year - year; }

    /**
     * @return =  @this after @param n years. The obtained date is not checked against overflows,
     * e.g. (29.2.2020).after_years(1) =====> 29.2.2021
     */
    datetime after_years(long long n) const {
        datetime dt = *this;
        dt.year += n;
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
            return *this + dd(2);
        else if (wd == 0)
            return *this + dd(1);
        else
            return *this;
    }

    /**
     * @return the number of dd of @this year.
     */
    int days_of_this_year() const { return 365 + ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)); }

    /**
     * @return the number of dd of @this month.
     */
    int days_of_this_month() const {
        return ( //if leap year add 1 day to the normal number of dd of February.
                       month == 1 && ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
               ) + days_of_months[month];
    }

    /**
     * @return @this whose day is the last of the month.
     */
    datetime end_of_month() const {
        datetime dt = *this;
        dt.day = dt.days_of_this_month() - 1;
        return dt;
    }

    /**
     * @return the ss from @epoch to @this.
     */
    long long to_timestamp() const { return seconds_from(datetime()); }

    /**
     * @return the ss from @d2 to @this.
     */
    long long seconds_from(const datetime &d2) const { return d2.seconds_to(*this); }

    period extract_time_of_day() const {
        return {hh(hrs), mm(min), ss(sec)};
    }

};

/**
 * @return = -@p
 */
period operator-(period &p) { return ss(-p.to_seconds()); }

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
    std::string D = to_string(dd.getDay());
    if (!keep_original_length) {
        D.insert(0, num - D.length(), '0');
    }
    replace(output, std::string(num, '!'), D);

    num = output.find_last_of('@') - output.find('@') + 1;
    std::string M;
    if (month_str) {
        M = to_month(dd.getMonth()); // NOLINT(cppcoreguidelines-narrowing-conversions)
        if (!keep_original_length) {
            M = M.substr(0, num);
            M += std::string(num - M.length(), ' ');
        }
    } else {
        M = to_string(dd.getMonth());
        if (!keep_original_length)
            M.insert(0, num - M.length(), '0');
    }
    replace(output, std::string(num, '@'), M);

    num = output.find_last_of('#') - output.find('#') + 1;
    std::string Y = to_string(abs(dd.getYear()));
    if (!keep_original_length) {
        if (dd.getYear() >= 0) {
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
        if (dd.getHrs() > 12) {
            h = to_string(dd.getHrs() - 12);
            output += " PM";
        } else {
            h = to_string(dd.getHrs());
            output += " AM";
        }
    } else {
        h = to_string(dd.getHrs());
    }
    if (!keep_original_length)
        h.insert(0, num - h.length(), '0');
    replace(output, std::string(num, '$'), h);

    num = output.find_last_of('%') - output.find('%') + 1;
    std::string m = to_string(dd.getMin());
    if (!keep_original_length)
        m.insert(0, num - m.length(), '0');
    replace(output, std::string(num, '%'), m);

    num = output.find_last_of('&') - output.find('&') + 1;
    std::string s = to_string(dd.getSec());
    if (!keep_original_length)
        s.insert(0, num - s.length(), '0');
    replace(output, std::string(num, '&'), s);

    return os << output;
}

std::ostream &operator<<(std::ostream &os, period const &d) {
    return os << "Days: " << std::setfill('0') << std::setw(2) << d.getDays() << ", hh: " << std::setfill('0')
              << std::setw(2) << d.getHrs() << ", mm: " << std::setfill('0') << std::setw(2) << d.getMin()
              << ", ss: "
              << std::setfill('0') << std::setw(2) << d.getSec();
}

#endif //DATETIME_CLASSES_H

#pragma clang diagnostic pop
