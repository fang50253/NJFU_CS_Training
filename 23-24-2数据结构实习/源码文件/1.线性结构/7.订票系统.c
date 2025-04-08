#include <stdio.h>
#include <string.h>

#define MAX_FLIGHTS 100
#define MAX_BOOKINGS 100

// 航班信息结构
typedef struct {
    char flight_number[10];
    char departure_city[30];
    char arrival_city[30];
    char departure_time[10];
    char arrival_time[10];
    double ticket_price;
    double discount;
    int total_seats;
    int booked_seats;
} Flight;

// 订票信息结构
typedef struct {
    char booking_id[20];
    char customer_name[50];
    char id_number[20];
    char flight_number[10];
    int tickets_booked;
} Booking;

// 存储航班信息
Flight flights[MAX_FLIGHTS];
int flight_count = 0;

// 存储订票信息
Booking bookings[MAX_BOOKINGS];
int booking_count = 0;

// 添加航班信息
void add_flight(Flight flight) {
    if (flight_count < MAX_FLIGHTS) {
        flights[flight_count++] = flight;
        // 追加到文件
        FILE *fp = fopen("flights.dat", "ab");
        fwrite(&flight, sizeof(Flight), 1, fp);
        fclose(fp);
    } else {
        printf("无法添加更多航班信息!\n");
    }
}

// 按航班号查询航班信息
Flight* query_flight_by_number(char* flight_number) {
    for (int i = 0; i < flight_count; i++) {
        if (strcmp(flights[i].flight_number, flight_number) == 0) {
            return &flights[i];
        }
    }
    return NULL;
}

// 订票
int book_ticket(Booking booking) {
    Flight *flight = query_flight_by_number(booking.flight_number);
    if (flight == NULL) {
        printf("未找到航班!\n");
        return -1;
    }

    if (flight->booked_seats + booking.tickets_booked > flight->total_seats) {
        printf("该航班已满座!\n");
        return -1;
    }

    // 更新航班的已订座位数
    flight->booked_seats += booking.tickets_booked;

    // 保存订票信息
    bookings[booking_count++] = booking;

    // 更新航班信息文件
    FILE *fp = fopen("flights.dat", "rb+");
    for (int i = 0; i < flight_count; i++) {
        if (strcmp(flights[i].flight_number, booking.flight_number) == 0) {
            fseek(fp, sizeof(Flight) * i, SEEK_SET);
            fwrite(&flights[i], sizeof(Flight), 1, fp);
            break;
        }
    }
    fclose(fp);

    // 保存订票信息到文件
    fp = fopen("bookings.dat", "ab");
    fwrite(&booking, sizeof(Booking), 1, fp);
    fclose(fp);

    printf("订票成功!\n");
    return 0;
}

// 查询航班信息示例
void display_flight_info(Flight* flight) {
    if (flight != NULL) {
        printf("航班号: %s\n", flight->flight_number);
        printf("起飞城市: %s\n", flight->departure_city);
        printf("抵达城市: %s\n", flight->arrival_city);
        printf("起飞时间: %s\n", flight->departure_time);
        printf("抵达时间: %s\n", flight->arrival_time);
        printf("票价: %.2f\n", flight->ticket_price);
        printf("折扣: %.2f\n", flight->discount);
        printf("总座位数: %d\n", flight->total_seats);
        printf("已订座位数: %d\n", flight->booked_seats);
    } else {
        printf("未找到航班信息!\n");
    }
}

int main() {
    int op=0;
    while(op!=6)
    {
        printf("1.录入航班信息\n");
        printf("2.查询航班信息\n");
        printf("3.")
        scanf("%d",&op);
        if(op==1)
        {
            printf("依次输入：航班号 离港 到达 离港时间 到达时间 票价 折扣 席位数\n");
            char flight_number[10];
            char departure_city[30];
            char arrival_city[30];
            char departure_time[10];
            char arrival_time[10];
            double ticket_price;
            double discount;
            int total_seats;
            scanf(" %s %s %s %s %s",flight_number,departure_city,arrival_city,departure_time,arrival_time);
            scanf("%d%d%d",&ticket_price,&discount,&total_seats);
            Flight f={flight_number,departure_city,arrival_city,departure_time,arrival_time,ticket_price,discount,total_seats,0};
            add_flight(f);
        }
        else if(op==2)
        {
            printf("输入航班号\n");
            char flight_number[10];
            scanf("%s",flight_number);
            query_flight_by_number(flight_number);
        }

    }
    return 0;
}