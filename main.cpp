/* mros2 example
 * Copyright (c) 2021 smorita_emb
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <string>
#include "mbed.h"
#include "mros2.h"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/int16.hpp"
#include "custom_msgs/geometry_msgs/msg/twist.hpp"
#include "EthernetInterface.h"
#include <steering.hpp>

#define IP_ADDRESS ("192.168.11.2") /* IP address */
#define SUBNET_MASK ("255.255.255.0") /* Subnet mask */
#define DEFAULT_GATEWAY ("192.168.11.1") /* Default gateway */

Semaphore oneSlot(1);
static UnbufferedSerial console(USBTX, USBRX);

mros2::Subscriber sub;
mros2::Subscriber sub2;
mros2::Publisher pub;


// Steering steering(
//     Tire(DigitalOut(D1), DigitalOut(D2), PwmOut(D3), PwmOut(D3), 0),
//     Tire(DigitalOut(D1), DigitalOut(D2), PwmOut(D3), PwmOut(D3), 0),
//     Tire(DigitalOut(D1), DigitalOut(D2), PwmOut(D3), PwmOut(D3), 0)
// );

void userCallback(std_msgs::msg::String *msg)
{
  printf("subscribed msg: '%s'\r\n", msg->data.c_str());
}

void userTwistCallback(geometry_msgs::msg::Twist *msg)
{
  printf("subscribed msg: linear  x:'%f' y:'%f' z:'%f'\r\n", msg->linear.x, msg->linear.y, msg->linear.z);
  printf("subscribed msg: angular x:'%f' y:'%f' z:'%f'\r\n", msg->angular.x, msg->angular.y, msg->angular.z);

  // モータ指令の実装をすればいいはず。
}
int main() {
    console.baud(115200);
    EthernetInterface network;
    network.set_dhcp(false);
    network.set_network(IP_ADDRESS, SUBNET_MASK, DEFAULT_GATEWAY);

    nsapi_size_or_error_t   r = network.connect();
    if (r != 0) {
        printf("Error! net->connect() returned: %d\n", r);
        return r;
    }


    // Show the network address
    SocketAddress   ip;
    SocketAddress   netmask;
    SocketAddress   gateway;

    network.get_ip_address(&ip);
    network.get_netmask(&netmask);
    network.get_gateway(&gateway);

    const char*     ipAddr = ip.get_ip_address();
    const char*     netmaskAddr = netmask.get_ip_address();
    const char*     gatewayAddr = gateway.get_ip_address();

    printf("IP address: %s\r\n", ipAddr ? ipAddr : "None");
    printf("Netmask: %s\r\n", netmaskAddr ? netmaskAddr : "None");
    printf("Gateway: %s\r\n\r\n", gatewayAddr ? gatewayAddr : "None");

    printf("mbed mros2 start!\r\n");
    printf("app name: echoreply_string\r\n");
    mros2::init(0, NULL);
    MROS2_DEBUG("mROS 2 initialization is completed\r\n");

    mros2::Node logNode = mros2::Node::create_node("mros2_log_node");

    pub = logNode.create_publisher<std_msgs::msg::Int16>("to_linux", 10);
    sub = logNode.create_subscription<std_msgs::msg::String>("to_stm", 10, userCallback);
    // sub2 = twistNode.create_subscription<geometry_msgs::msg::Twist>("cmd_vel", 10, userTwistCallback);

    MROS2_INFO("ready to pub/sub message\r\n");


    int count = 0;
    DigitalIn button(BUTTON1);
    while(true){
        // std_msgs::msg::String msg;
        // msg.data = "hello world from stm!" + std::to_string(count);
        if(button){
            std_msgs::msg::Int16 msg;
            msg.data = count;
            pub.publish(msg);
            MROS2_INFO("hello world from stm! send: %d \r\n",msg.data);
            count ++;
            osDelay(1000);
        }
    }
    mros2::spin();
    return 0;
}