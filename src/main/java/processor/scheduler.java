import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;


public class order {


};

class single_order extends order {
public single_order(HashMap<String, String> map) {


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


