extern "C" { 
    #include "tx_api.h" 
}

class LCDThread {
    public:
        static void entry(ULONG arg);
    private:
        static constexpr ULONG sleepTicks = 250;
        static constexpr uint8_t testvalue = 25;
};