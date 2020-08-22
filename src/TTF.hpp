
#include <math.h>

pair<float, float> operator -(const std::pair<float, float>& x, const std::pair<float, float>& y) {
    return make_pair(x.first - y.first, x.second - y.second);
}
pair<float, float> operator +(const std::pair<float, float>& x, const std::pair<float, float>& y) {
    return make_pair(x.first + y.first, x.second + y.second);
}
pair<float, float> operator *(const float x, const std::pair<float, float>& y) {
    return make_pair(x*y.first, x*y.second);
}

class TTF {
public:
    constexpr static float period = 1440;

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
            result = interpolation(points[index_down], points[index_up], t);
        }
        
        return result;
    }

    TTF chaining(TTF f1, TTF f2) {

        return TTF(vector<pair<float,float>>());
    }

    static TTF minimum(TTF f1, TTF f2) {
        vector<pair<float,float>> points;
        int index1 = 0;
        int index2 = 0;
        // float t = reverseChaining(f1.points[0], f1.points[1], f1.points[1].first+f1.points[1].second);
        // cout << t << endl;
        // cout << t + interpolation(f1.points[0], f1.points[1], t) << endl;
        if (f1.points[0].second <= f2.points[0].second) {
            points.push_back(f1.points[0]);
        } else {
            points.push_back(f2.points[0]);
        }
        while (index1<f1.points.size()-1 && index2<f2.points.size()-1) {
            pair<float,float> p = f1.points[index1];
            pair<float,float> q = f2.points[index2];
            pair<float,float> next_p = f1.points[index1+1];
            pair<float,float> next_q = f2.points[index2+1];
            pair<float,float> x = intersection(p, f1.points[index1+1], q, f2.points[index2+1]);
            if (x.first != -1) points.push_back(x);
            float t1 = next_p.first;
            float t2 = next_q.first;
            if (t1 < t2) {
                if (next_p.second <= interpolation(q, next_q, t1)) {
                    points.push_back(next_p);
                }
                index1++;
            } else if (t1 > t2) {
                if (next_q.second <= interpolation(p, next_p, t2)) {
                    points.push_back(next_q);
                }
                index2++;
            } else {
                if (next_p.second <= next_q.second) {
                    points.push_back(next_p);
                } else {
                    points.push_back(next_q);
                }
                index1++;
            }
        }
        return TTF(points);
    }

    vector<pair<float,float>> getPoints() {
        return this->points;
    }
    

private:
    vector<pair<float,float>> points;

    static float interpolation(pair<float,float> p1, pair<float,float> p2, float t) {
        float distance_down = (t - p1.first);
        float distance_up = (p2.first - t);
        float value_down = p1.second;
        float value_up = p2.second;
        return (distance_down * value_up + distance_up * value_down) / (distance_down + distance_up);
    }

    // f(t) = ((p2.second - p1.second)*t + p1.first*p2.second + p2.first*p1.second) / (p2.first - p1.first); (interpolation)
    // Solves t + f(t) = t2 for given t2 and f 
    static float reverseChaining(pair<float,float> p1, pair<float,float> p2, float t2) {
        float t = (t2*(p2.first - p1.first) - p1.first*p2.second - p2.first*p1.second) / (p2.second - p1.second + p2.first - p1.first);  
        return t;
    }
    

    static pair<float,float> intersection(pair<float,float> p, pair<float,float> p2, pair<float,float> q, pair<float,float> q2) {
        pair<float,float> r = p2 - p;
        pair<float,float> s = q2 - q;
        // cout << "p : " << p.first << " " << p.second << "q : " << q.first << " " << q.second << "r : " << r.first << " " << r.second << "s : " << s.first << " " << s.second << endl;
        float x = cross(r,s);
        if (x == 0) {
            cout << "parallel : " << endl;
            return make_pair(-1,-1);                    // Parallel : no intersection
        } else {
            float t = cross(q-p, s) / x;
            float u = cross(q-p, r) / x;
            if (t>= 0 && t<= 1 && u>= 0 && u<= 1) {
                return p + (t*r);                       // Intersection
            } else {
                // cout << "no intersection : " << endl;
                return make_pair(-1,-1);                // No intersection
            }
        }
    }

    static float cross(pair<float,float> v1, pair<float,float> v2) { // Returns the 2D cross product of the 2 specified vectors
        return (v1.first * v2.second - v1.second * v2.first);
    }
};

