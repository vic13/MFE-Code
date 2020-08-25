
class Clock {
public:
    Clock(const char* message, bool average, int precision = 0) {
        this->message = message;
        this->precision = precision;
        this->average = average;
        this->t1 = std::chrono::high_resolution_clock::now();
    }

    void start() {
        this->t1 = std::chrono::high_resolution_clock::now();
    }

    void lap(bool display = false) {
        auto t2 = std::chrono::high_resolution_clock::now();
        this->duration += std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        this->nbLaps++;
        if (display) {
            this->display();
        }
    }

    void display() {
        if (this->average) duration /= nbLaps;
        if (precision == 0) cout << message << " : " << duration/1000000.0 << " s" << endl;
        if (precision == 1) cout << message << " : " << duration/1000.0 << " ms" << endl;
        if (precision == 2) cout << message << " : " << duration << " µs" << endl;
    }
private:
    std::chrono::steady_clock::time_point t1;
    long duration = 0;
    const char* message;
    int precision;
    bool average;
    int nbLaps = 0;
};