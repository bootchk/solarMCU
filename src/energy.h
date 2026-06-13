
class Energy {
public:
    static bool isEnoughToWork(void);

    static bool isEnoughToKeepWork(void);

    // For certain implementations, 
    // initialize the input pin used for monitoring energy availability.
    static void initPin(void);
};
