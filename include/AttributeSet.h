#ifndef ATTRIBUTESET_H
#define ATTRIBUTESET_H

/// Class representing the attributes an Entity can have.
/// It can also represent modifiers granted by something else.
class AttributeSet
{
  public:
    AttributeSet();
    virtual ~AttributeSet();

    bool addHP(int value);
    bool addMH(int value);
    bool addST(int value);
    bool addEN(int value);
    bool addFD(int value);
    bool addDR(int value);
    bool addVI(int value);
    bool addAG(int value);
    bool addIQ(int value);
    bool addMD(int value);
    bool addCH(int value);
    bool addAL(int value);
    bool addAT(int value);
    bool addLU(int value);

    bool setHP(unsigned int value);
    bool setMH(unsigned int value);
    bool setST(unsigned int value);
    bool setEN(unsigned int value);
    bool setFD(unsigned int value);
    bool setDR(unsigned int value);
    bool setVI(unsigned int value);
    bool setAG(unsigned int value);
    bool setIQ(unsigned int value);
    bool setMD(unsigned int value);
    bool setCH(unsigned int value);
    bool setAL(unsigned int value);
    bool setAT(unsigned int value);
    bool setLU(unsigned int value);

    unsigned int getHP();
    unsigned int getMH();
    unsigned int getFD();
    unsigned int getDR();
    unsigned int getST();
    unsigned int getEN();
    unsigned int getVI();
    unsigned int getAG();
    unsigned int getIQ();
    unsigned int getMD();
    unsigned int getCH();
    unsigned int getAL();
    unsigned int getAT();
    unsigned int getLU();

  protected:
  private:
    struct _stats
    {
      unsigned int hp_ : 10;   ///< Hit points      (1023)
      unsigned int mh_ : 10;   ///< Max hit points  (1023)
      unsigned int fd_ : 10;   ///< Food timeout    (1023)
      unsigned int dr_ : 10;   ///< Drink timeout   (1023)
      unsigned int st_ : 5;    ///< Strength        (31)
      unsigned int en_ : 5;    ///< Endurance       (31)
      unsigned int vi_ : 5;    ///< Vitality        (31)
      unsigned int ag_ : 5;    ///< Agility         (31)
      unsigned int iq_ : 5;    ///< IQ              (31)
      unsigned int md_ : 5;    ///< Magic Defense   (31)
      unsigned int ch_ : 5;    ///< Charisma        (31)
      unsigned int al_ : 5;    ///< Allure          (31)
      unsigned int at_ : 5;    ///< Attentiveness   (31)
      unsigned int lu_ : 5;    ///< Luck            (31)
      unsigned int dummy : 6; ///< Dummy to align to 32-bit boundaries
    } stats;
};

#endif // ATTRIBUTESET_H
