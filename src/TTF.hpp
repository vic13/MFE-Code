
#include <math.h>

class TTF {
public:
    TTF(vector<pair<float,float>> points) {
        this->points = points;
    }

    float evaluate(float t) {
        t = fmod(t, period);
        // logarithmic search
        auto it = std::lower_bound(this->points.begin(), this->points.end(), make_pair(t, 0.0f));
        int index_up = std::distance(points.begin(), it);
        float result = 0.0f;
        if (points[index_up].first == t) {
            result = points[index_up].second;
        } else {
            // interpolation
            int index_down = index_up - 1;
            float distance_down = (t - points[index_down].first);
            float distance_up = (points[index_up].first - t);
            float value_down = points[index_down].second;
            float value_up = points[index_up].second;
            cout << value_down << "  " << value_up << endl;
            result = (distance_down * value_up + distance_up * value_down) / (distance_down + distance_up);
        }
        
        return result;
    }

    TTF chaining(TTF f1, TTF f2) {

        return TTF(vector<pair<float,float>>());
    }

    TTF minimum(TTF f1, TTF f2) {

        return TTF(vector<pair<float,float>>());
    }

private:
    vector<pair<float,float>> points;
    float period = 1440;

};

