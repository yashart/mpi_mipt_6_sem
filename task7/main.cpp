#include "sha256.h"
#include <string>
#include <iostream>
#include <sstream>
#include <mutex>
#include <thread>
#include <stdlib.h>



std::string to_hex(BYTE * in, int len)
{
    std::stringstream ss;
    for (int i = 0; i < len; i++)
    {
        unsigned char c = in[i];
        ss << std::hex << (int)(c >> 4) << (int)(c & 7);
    }
    return ss.str();
}

std::string hash_string(const std::string & in)
{
    SHA256_CTX hash;
    sha256_init(&hash);
    sha256_update(&hash, (BYTE*)in.c_str(), in.size());

    BYTE buf[32];
    sha256_final(&hash, buf);
    return to_hex(buf, 32);
}

volatile int i;
volatile int complete = 0;
std::mutex m;
std::string transaction;

int calc_one_hash (std::string strTrans) {
  std::string out;
  out = hash_string(strTrans);
  if(!out.compare(0,6,"f132e4")){
    std::cout << strTrans << " " << out << std::endl;
    complete = 1;
    return 1;
  }else {
    return 0;
  }
}


void calc_manager() {
  do {
    m.lock();
    int localCurrentI = i;
    i++;
    m.unlock();
    std::string in = transaction;
    in.append(std::to_string(localCurrentI));
    if ((calc_one_hash(in) == 1) || (complete == 1)) {
      break;
    }
  }while(1);
}

int main(int argc, char* argv[])
{
    transaction = (argv[1]);

    int threadNum = atoi(argv[2]);
    std::thread workers[threadNum];

    for (int k = 0; k < threadNum; k++) {
      workers[k] = std::thread(calc_manager);
    }
    for (int k = 0; k < threadNum; k++) {
      workers[k].join();
    }
//    std::cout << i << std::endl << out << std::endl;
    return 0;
}
