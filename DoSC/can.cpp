#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "can.h"

#define TIMEOUT 0
#define CAN_ERR -1

void can::RunCanRx()
{
    struct can_frame cFrame;
    tCanFrame frame;
    fd_set selectSet;
    ssize_t retval;
    struct timeval timeout;

    while (m_running == true)
    {
        FD_ZERO(&selectSet);
        FD_SET(m_socket, &selectSet);
        timeout.tv_sec = m_timeoutS;
        timeout.tv_usec = m_timeoutUs;
        retval = select(m_socket+1, &selectSet, nullptr, nullptr, &timeout);
        if (retval == CAN_ERR)
        {
            perror("Select failed\n");
            return;
        }
        else if (retval == TIMEOUT)
        {
            continue;
        }

        if (FD_ISSET(m_socket, &selectSet))
        {
            retval = recv(m_socket, &cFrame, sizeof(struct can_frame), 0);
            if (retval != SOCKET_FAIL && m_rxCb != nullptr)
            {
                frame.id = cFrame.can_id;
                frame.size = cFrame.can_dlc;
                std::copy(&cFrame.data[0],
                          &cFrame.data[frame.size],
                          frame.data.begin());
                m_rxCb(frame);
            }
            else
            {
                perror("recv failed\n");
                return;
            }
        }
    }
}

int can::SendCanFrame(tCanFrame &frame)
{
    struct can_frame cFrame;
    ssize_t retval;

    if (m_socket == EMPTY_SOCKET)
    {
        return CAN_SOCKET_NOT_OPEN;
    }

    cFrame.can_id = frame.id;
    cFrame.can_dlc = frame.size;
    std::copy(frame.data.begin(), frame.data.begin()+frame.size, cFrame.data);

    retval = write(m_socket, &cFrame, sizeof(struct can_frame));
    if (retval == SOCKET_FAIL)
    {
        return CAN_SEND_FAILED;
    }

    return CAN_OK;
}

void can::SetRxCb(tCanRxCb &cb)
{
    m_rxCb = cb;
}

int can::AssignInterface(std::string &interface)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    m_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (m_socket == SOCKET_FAIL)
    {
        m_socket = EMPTY_SOCKET;
        return CAN_SOCKET_NOT_OPEN;
    }

    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ);
    ioctl(m_socket, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    struct sockaddr *pSockAddr = reinterpret_cast<struct sockaddr*>(&addr);
    if (bind(m_socket, pSockAddr, sizeof(addr)) == SOCKET_FAIL)
    {
       close(m_socket);
       m_socket = EMPTY_SOCKET;
       return CAN_SOCKET_BIND_FAIL;
    }

    return CAN_OK;
}

void can::UnassignInterface()
{
    if (m_running == true)
    {
        StopCan();
    }

    if(m_socket != EMPTY_SOCKET)
    {
        shutdown(m_socket, SHUT_RDWR);
        close(m_socket);
        m_socket = EMPTY_SOCKET;
    }
}

int can::StartCan()
{
    if (m_socket == EMPTY_SOCKET)
    {
        return CAN_SOCKET_NOT_OPEN;
    }

    m_running = true;
    m_canThread = std::thread(&can::RunCanRx, this);

    return CAN_OK;
}

void can::StopCan()
{
    m_running = false;
    if (m_canThread.joinable() == true)
    {
        m_canThread.join();
    }
}

can::can()
{
    m_socket = EMPTY_SOCKET;
    m_running = false;

    m_timeoutS = DEFAULT_TIMEOUT_S;
    m_timeoutUs = DEFAULT_TIMEOUT_US;

    m_rxCb = nullptr;
}

can::~can()
{
    StopCan();
    UnassignInterface();
}
