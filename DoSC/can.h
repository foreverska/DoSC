#ifndef CAN_H
#define CAN_H

#include <functional>
#include <array>
#include <atomic>
#include <thread>

#define CAN_OK                  0
#define CAN_SOCKET_NOT_OPEN     -1
#define CAN_SOCKET_BIND_FAIL    -2
#define CAN_SEND_FAILED         -3

#define EMPTY_SOCKET            -1

#define SOCKET_FAIL             -1

#define DEFAULT_TIMEOUT_S       0
#define DEFAULT_TIMEOUT_US      500000

typedef struct
{
    uint32_t id;
    uint8_t size;
    std::array<uint8_t, 8> data;
} tCanFrame;

typedef std::function<void(tCanFrame&)> tCanRxCb;

class can
{
private:
    int m_socket;
    std::atomic<bool> m_running;
    std::thread m_canThread;

    int m_timeoutS;
    int m_timeoutUs;

    tCanRxCb m_rxCb;

    void RunCanRx();

protected:

public:
    int AssignInterface(std::string &interface);
    void UnassignInterface();
    int StartCan();
    void StopCan();

    void SetRxCb(tCanRxCb &cb);
    int SendCanFrame(tCanFrame &frame);

    can();
    ~can();
};

typedef std::shared_ptr<can> tSharedCan;

#endif // CAN_H
