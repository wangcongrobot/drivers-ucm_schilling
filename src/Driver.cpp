#include "Driver.hpp"
#include "UcmRaw.hpp"
#include <iostream>

using namespace ucm_schilling;
using namespace std;


Driver::Driver(const Config& config)
  : base_schilling::Driver(64),
    mConfig(config)
{
  mData.percentage = 0;
  setAddress(ADDR_UCM);  
  std::vector<uint8_t> regs(2);
  regs[0] = raw::UCM_AD2_HIGH;
  regs[1] = raw::UCM_AD2_LOW;
  setRegs(regs);  
}

void Driver::collectData()
{
  cout <<"collectData" <<endl;
  sendReadMsg();
  read(2000);
  mData.time = base::Time::now();
  uint16_t val =  (uint16_t)getReg(raw::UCM_AD2_LOW);
  val |= getReg(raw::UCM_AD2_HIGH) << 8;
  //cout <<"value: " <<val <<endl;
  //calculate percentage
  int count = val;
  int offset = 0;
  float coeff = 0;
  if(count<=mConfig.count50){
      offset=mConfig.count0;
      if(mConfig.count50 != mConfig.count0){
          coeff=50/((float)(mConfig.count50-mConfig.count0));
      }
      else{
          coeff=1000;
      }
  }
  else{
      offset=2*mConfig.count50-mConfig.count100;
      if(mConfig.count100 != mConfig.count50){
          coeff=50/((float)(mConfig.count100-mConfig.count50));
      }
      else{
          coeff=1000;
      }
  }
  count -= offset;
  float fval = coeff*((float)count);
  mData.percentage = fval;
}

ucm_schilling::UcmData Driver::getData() const
{
  return mData;
}

void Driver::dumpData() const
{
    cout << setprecision(3) << mData.time << " "
         << mData.percentage<< endl;
}
