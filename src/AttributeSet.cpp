#include "AttributeSet.h"

#define SAFE_SET(x, y, max) do                                                \
                       {                                                      \
                         if (y < 0)                                           \
                         {                                                    \
                           x = 0;                                             \
                           return false;                                      \
                         }                                                    \
                         if (y > max)                                         \
                         {                                                    \
                           x = max;                                           \
                           return false;                                      \
                         }                                                    \
                         x = (y);                                             \
                         return true;                                         \
                       } while(0)                                             \

AttributeSet::AttributeSet()
{
  stats.hp_ = 10;
  stats.mh_ = 10;
  stats.fd_ = 512;
  stats.dr_ = 512;
  stats.st_ = 15;
  stats.en_ = 15;
  stats.vi_ = 15;
  stats.ag_ = 15;
  stats.iq_ = 15;
  stats.md_ = 15;
  stats.ch_ = 15;
  stats.al_ = 15;
  stats.at_ = 15;
  stats.lu_ = 15;
  stats.dummy = 0;
}

AttributeSet::~AttributeSet()
{
  //dtor
}

bool AttributeSet::addHP(int value) { SAFE_SET(stats.hp_, stats.hp_ + value, stats.mh_); }
bool AttributeSet::addMH(int value) { SAFE_SET(stats.mh_, stats.mh_ + value, 1023); }
bool AttributeSet::addFD(int value) { SAFE_SET(stats.fd_, stats.fd_ + value, 1023); }
bool AttributeSet::addDR(int value) { SAFE_SET(stats.dr_, stats.dr_ + value, 1023); }
bool AttributeSet::addST(int value) { SAFE_SET(stats.st_, stats.st_ + value, 31); }
bool AttributeSet::addEN(int value) { SAFE_SET(stats.en_, stats.en_ + value, 31); }
bool AttributeSet::addVI(int value) { SAFE_SET(stats.vi_, stats.vi_ + value, 31); }
bool AttributeSet::addAG(int value) { SAFE_SET(stats.ag_, stats.ag_ + value, 31); }
bool AttributeSet::addIQ(int value) { SAFE_SET(stats.iq_, stats.iq_ + value, 31); }
bool AttributeSet::addMD(int value) { SAFE_SET(stats.md_, stats.md_ + value, 31); }
bool AttributeSet::addCH(int value) { SAFE_SET(stats.ch_, stats.ch_ + value, 31); }
bool AttributeSet::addAL(int value) { SAFE_SET(stats.al_, stats.al_ + value, 31); }
bool AttributeSet::addAT(int value) { SAFE_SET(stats.at_, stats.at_ + value, 31); }
bool AttributeSet::addLU(int value) { SAFE_SET(stats.lu_, stats.lu_ + value, 31); }

bool AttributeSet::setHP(unsigned int value) { SAFE_SET(stats.hp_, value, stats.mh_); }
bool AttributeSet::setMH(unsigned int value) { SAFE_SET(stats.mh_, value, 1023); }
bool AttributeSet::setFD(unsigned int value) { SAFE_SET(stats.fd_, value, 1023); }
bool AttributeSet::setDR(unsigned int value) { SAFE_SET(stats.dr_, value, 31); }
bool AttributeSet::setST(unsigned int value) { SAFE_SET(stats.st_, value, 31); }
bool AttributeSet::setEN(unsigned int value) { SAFE_SET(stats.en_, value, 31); }
bool AttributeSet::setVI(unsigned int value) { SAFE_SET(stats.vi_, value, 31); }
bool AttributeSet::setAG(unsigned int value) { SAFE_SET(stats.ag_, value, 31); }
bool AttributeSet::setIQ(unsigned int value) { SAFE_SET(stats.iq_, value, 31); }
bool AttributeSet::setMD(unsigned int value) { SAFE_SET(stats.md_, value, 31); }
bool AttributeSet::setCH(unsigned int value) { SAFE_SET(stats.ch_, value, 31); }
bool AttributeSet::setAL(unsigned int value) { SAFE_SET(stats.al_, value, 31); }
bool AttributeSet::setAT(unsigned int value) { SAFE_SET(stats.at_, value, 31); }
bool AttributeSet::setLU(unsigned int value) { SAFE_SET(stats.lu_, value, 31); }

unsigned int AttributeSet::getHP() { return stats.hp_; }
unsigned int AttributeSet::getMH() { return stats.mh_; }
unsigned int AttributeSet::getFD() { return stats.fd_; }
unsigned int AttributeSet::getDR() { return stats.dr_; }
unsigned int AttributeSet::getST() { return stats.st_; }
unsigned int AttributeSet::getEN() { return stats.en_; }
unsigned int AttributeSet::getVI() { return stats.vi_; }
unsigned int AttributeSet::getAG() { return stats.ag_; }
unsigned int AttributeSet::getIQ() { return stats.iq_; }
unsigned int AttributeSet::getMD() { return stats.md_; }
unsigned int AttributeSet::getCH() { return stats.ch_; }
unsigned int AttributeSet::getAL() { return stats.al_; }
unsigned int AttributeSet::getAT() { return stats.at_; }
unsigned int AttributeSet::getLU() { return stats.lu_; }
