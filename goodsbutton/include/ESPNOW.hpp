/*
 * @Description: 
 * @Author: qingmeijiupiao
 * @Date: 2024-05-19 14:44:23
 */
#ifndef esp_now_hpp
#define esp_now_hpp
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <map>
#include <array>
#include <vector>
#define secret_key 0xFEFE
#define MAX_RETRY 5
std::map<int,uint8_t*> receive_MACs;//接收MAC
esp_now_peer_info_t peerInfo;
bool is_conect = false;
//数据包格式
struct data_package {
  uint16_t header_code=secret_key;//数据包头,作为密钥使用
  uint8_t name_len;
  String package_name;
  std::vector<uint8_t> data;
  void add_name(String _name){
    package_name=_name;
    name_len=package_name.length();
  }
  void add_data(uint8_t* _data,int _datalen){
    for(int i=0;i<_datalen;i++){
      data.push_back(_data[i]);
    }
  }
  void decode(uint8_t* _data,int _datalen){
    data.clear();
    header_code=data[0]+data[1]*256;
    name_len=data[2];
    package_name=String((char*)data.data()+3,name_len);
    for(int i=3+name_len;i<_datalen;i++){
      data.push_back(data[i]);
    }
  }
  void get_data(uint8_t* data_array){
    data_array[0]=header_code/256;
    data_array[1]=header_code%256;
    data_array[2]=name_len;
    for(int i=0;i<name_len;i++){
      data_array[3+i]=package_name[i];
    }
    for(int i=0;i<data.size();i++){
      data_array[3+name_len+i]=data[i];
    }
  }
  int get_len(){
    return 3+name_len+data.size();
  }

};

//回调函数
using callback_func =std::function<void(data_package)>;

std::map<String, callback_func> callback_map;

data_package re_data;

TaskHandle_t  callback_task_handle=nullptr;
void callback_task(void * pvParameters ){
  if(callback_map.count( re_data.package_name )!=0){
    callback_map[re_data.package_name](re_data);
  }
  callback_task_handle=nullptr;
  vTaskDelete(NULL);
};
//接收数据时的回调函数，收到数据时自动运行
void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  //检查是否是数据包
  if(len<2) return;
  if(*(uint16_t*)data!=secret_key) return;
  if(callback_task_handle==nullptr){
    xTaskCreate(callback_task, "callback_task", 8192, NULL, 5, &callback_task_handle);
  }
  is_conect=true;
}



//广播地址
uint8_t receive_MACAddress[] ={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
//ESP-NOW初始化
void esp_now_setup() {
  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
      Serial.println("ESP-NOW initialization failed");
      return;
  }

  peerInfo.ifidx = WIFI_IF_STA;
  memcpy(peerInfo.peer_addr, receive_MACAddress, 6);

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
      return;
  }
  esp_now_register_recv_cb(OnDataRecv);
} 


//通过espnow发送数据
void esp_now_send_package(String name,uint8_t* data,int datalen,uint8_t* receive_MAC=receive_MACAddress){
  data_package send_data;
  send_data.add_name(name);
  send_data.add_data(data,datalen);
  uint8_t send_data_array[send_data.get_len()];
  send_data.get_data(send_data_array);
  //发送
  for(int i=0;i<MAX_RETRY;i++){
    auto err = esp_now_send(receive_MAC,send_data_array,send_data.get_len());
    if (err == ESP_OK)  break;
    delay(20);
  }
}

#endif