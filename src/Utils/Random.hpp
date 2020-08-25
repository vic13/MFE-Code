class Random {
public:
    static void initRandom() {
        int seed = PARAMS_SEED;
        if (seed == -1) {
            seed = time(NULL);
        }
        srand(seed);
        cout << "Seed : " << seed << endl;
    }

    static float random01() {
        return ((float) rand() / (RAND_MAX));
    }

    static int randomInt(int moduloValue) {
        return rand() % moduloValue;
    }
};