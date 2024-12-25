#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <map>

using namespace std;

const double FIXED_LOCAL_RATE_FIRST_3_MIN = 0.4; // 固话市话前3分钟费率
const double FIXED_LOCAL_RATE_AFTER_3_MIN = 0.25; // 固话市话超过3分钟后每分钟费率
const double FIXED_LONG_DISTANCE_RATE = 1.0; // 固话长途费率
const double MOBILE_LOCAL_OUTGOING_RATE = 0.3; // 无线本地主叫费率
const double MOBILE_LONG_DISTANCE_OUTGOING_RATE = 0.6; // 无线长途主叫费率
const double MOBILE_OFF_PEAK_DISCOUNT = 0.5; // 晚间优惠折扣

// 免费时长
const int FREE_FIXED_LOCAL_MINUTES = 300; // 本市通话免费时长（分钟）
const int FREE_LONG_DISTANCE_MINUTES = 100; // 长途（漫游）免费时长（分钟）

// 定义通话记录的数据结构
struct CallRecord {
    string number;
    int serviceType; // 0: 固话市话, 1: 固话长途, 2: 无线本地, 3: 无线长途, 4: 无线接听, 5: 亲情号码
    time_t startTime;
    int durationSecs;
};

// 将字符串转换为time_t类型
time_t parseTimeString(const char* dateTimeStr) {
    struct tm tmStruct = {};
    stringstream ss(dateTimeStr);
    char delimiter;
    int year, month, day, hour, minute, second;

    if (ss >> year >> delimiter >> month >> delimiter >> day 
        >> delimiter >> hour >> delimiter >> minute >> delimiter >> second && delimiter == ':') {
        tmStruct.tm_year = year - 1900; // 年份从1900年开始计算
        tmStruct.tm_mon = month - 1;    // 月份从0开始计算
        tmStruct.tm_mday = day;
        tmStruct.tm_hour = hour;
        tmStruct.tm_min = minute;
        tmStruct.tm_sec = second;
        tmStruct.tm_isdst = -1;         // 不考虑夏令时调整
        return mktime(&tmStruct);
    } else {
        cerr << "日期和时间格式无效" << endl;
        return 0; // 返回0表示解析失败
    }
}

// 计算费用函数
double calculateFee(CallRecord &record, int &freeFixedLocalTime, int &freeLongDistanceTime) {
    double fee = 0.0;
    time_t startTime = record.startTime;
    tm *localTime = localtime(&startTime);
    int durationMinutes = (record.durationSecs + 59) / 60; // 向上取整到分钟

    switch (record.serviceType) {
        case 0: // 固话市话
            if (freeFixedLocalTime >= durationMinutes) {
                freeFixedLocalTime -= durationMinutes;
            } else {
                durationMinutes -= freeFixedLocalTime;
                freeFixedLocalTime = 0;
                if (durationMinutes <= 3) {
                    fee += durationMinutes * FIXED_LOCAL_RATE_FIRST_3_MIN;
                } else {
                    fee += 3 * FIXED_LOCAL_RATE_FIRST_3_MIN + (durationMinutes - 3) * FIXED_LOCAL_RATE_AFTER_3_MIN;
                }
            }
            break;
        case 1: // 固话长途
            if (freeLongDistanceTime >= durationMinutes) {
                freeLongDistanceTime -= durationMinutes;
            } else {
                durationMinutes -= freeLongDistanceTime;
                freeLongDistanceTime = 0;
                fee += durationMinutes * FIXED_LONG_DISTANCE_RATE;
            }
            break;
        case 2: // 无线本地主叫
            fee += durationMinutes * MOBILE_LOCAL_OUTGOING_RATE;
            break;
        case 3: // 无线长途主叫
            if (localTime->tm_hour >= 7 && localTime->tm_hour < 24) {
                fee += durationMinutes * MOBILE_LONG_DISTANCE_OUTGOING_RATE;
            } else {
                fee += durationMinutes * MOBILE_LONG_DISTANCE_OUTGOING_RATE * MOBILE_OFF_PEAK_DISCOUNT;
            }
            break;
        case 4: // 无线接听（免费）
            break;
        default:
            cerr << "未知服务类型." << endl;
            break;
    }

    return fee;
}

// 辅助函数：将字符串转换为整数
int stringToInt(const string& str) {
    stringstream ss(str);
    int result;
    ss >> result;
    return result;
}

// 将通话记录写入文件
void writeBillToFile(const vector<CallRecord>& calls, const string& filename) {
    ofstream file(filename.c_str());
    for (size_t i = 0; i < calls.size(); ++i) {
        file << "号码: " << calls[i].number
             << ", 服务类型: " << calls[i].serviceType
             << ", 开始时间: " << ctime(&calls[i].startTime)
             << ", 通话时长: " << calls[i].durationSecs / 60 << " minutes"
             << endl;
    }
    file.close();
}

// 从文件中读取通话记录
vector<CallRecord> readCallsFromFile(const string& filename) {
    vector<CallRecord> calls;
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << endl;
        return calls;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        CallRecord record;

        getline(ss, token, ','); // 电话号码
        record.number = token;

        getline(ss, token, ','); // 服务类别
        record.serviceType = stringToInt(token);

        getline(ss, token, ','); // 开始时间（秒数自1970-01-01 00:00:00 UTC）
        record.startTime = static_cast<time_t>(stringToInt(token));

        getline(ss, token, ','); // 通话时长（秒）
        record.durationSecs = stringToInt(token);

        calls.push_back(record);
    }

    file.close();
    return calls;
}

// 查询功能（按号码或时间段）
void queryByNumberOrTime(const vector<CallRecord>& calls, const string& number, time_t startTime, time_t endTime) {
    cout << "查询号码的结果: " << number 
         << ", 开始时间: " << ctime(&startTime)
         << ", 结束时间: " << ctime(&endTime) << endl;
    
    bool found = false;
    for (size_t i = 0; i < calls.size(); ++i) {
        if (calls[i].number == number && 
            calls[i].startTime >= startTime && 
            calls[i].startTime <= endTime) {
            
            found = true;
            cout << "通话记录: "
                 << "服务类型: " << calls[i].serviceType
                 << ", 开始时间: " << ctime(&calls[i].startTime)
                 << ", 通话时长: " << calls[i].durationSecs / 60 << " minutes"
                 << endl;
        }
    }

    if (!found) {
        cout << "未找到给定号码和时间范围的呼叫记录。" << endl;
    }
}



int main() {
    // 主程序逻辑
    vector<CallRecord> allCalls = readCallsFromFile("source_data.txt");

    // 分类处理通话记录
    vector<CallRecord> fixedLocalCalls, fixedLongDistanceCalls,
                       mobileLocalCalls, mobileLongDistanceCalls,
                       mobileReceivedCalls, familyNumbers;
                       
    map<string, pair<int, int> > freeTimeMap; // <电话号码, <固话本市免费分钟, 长途免费分钟>>

    // 初始化每个号码的免费时长
    for (size_t i = 0; i < allCalls.size(); ++i) {
        freeTimeMap[allCalls[i].number] = make_pair(FREE_FIXED_LOCAL_MINUTES, FREE_LONG_DISTANCE_MINUTES);
    }

    // 根据serviceType分类通话记录...
    for (size_t i = 0; i < allCalls.size(); ++i) {
        CallRecord& call = allCalls[i];
        pair<int, int>& freeTimes = freeTimeMap[call.number];

        // 计算每个通话的费用，并更新免费时间
        double fee = calculateFee(call, freeTimes.first, freeTimes.second);

        // 根据服务类型将通话记录添加到相应的容器中
        switch (call.serviceType) {
            case 0: fixedLocalCalls.push_back(call); break;
            case 1: fixedLongDistanceCalls.push_back(call); break;
            case 2: mobileLocalCalls.push_back(call); break;
            case 3: mobileLongDistanceCalls.push_back(call); break;
            case 4: mobileReceivedCalls.push_back(call); break;
            case 5: familyNumbers.push_back(call); break;
        }
    }


    // 写入不同类型的通话记录到文件
    writeBillToFile(fixedLocalCalls, "fixed_local_calls_bill.txt");
    writeBillToFile(fixedLongDistanceCalls, "fixed_long_distance_calls_bill.txt");
    writeBillToFile(mobileLocalCalls, "mobile_local_calls_bill.txt");
    writeBillToFile(mobileLongDistanceCalls, "mobile_long_distance_calls_bill.txt");
    writeBillToFile(mobileReceivedCalls, "mobile_received_calls_bill.txt");
    writeBillToFile(familyNumbers, "family_numbers_bill.txt");

    // 提供查询接口
    string queryNumber;
    time_t queryStartTime = 0, queryEndTime = 0;
    char choice; 
    do {
        cout << "输入电话号码查询或按 q' 退出: ";
        cin >> queryNumber;
        if (queryNumber == "q") break;

        // 获取查询时间段...
        char startTimeStr[20], endTimeStr[20];
        cout << "输入开始时间(YYYY-MM-DD HH:MM:SS): ";
        cin >> startTimeStr;
        queryStartTime = parseTimeString(startTimeStr);

        cout << "输入结束时间(YYYY-MM-DD HH:MM:SS): ";
        cin >> endTimeStr;
        queryEndTime = parseTimeString(endTimeStr);

        // 查询并显示结果
        queryByNumberOrTime(allCalls, queryNumber, queryStartTime, queryEndTime);

        cout << "是否继续查询? [y/n]: ";
        cin >> choice;
    } while (choice == 'y' || choice == 'Y');

    return 0;
}