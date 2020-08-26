
#include <math.h>
#include <cmath>

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
    inline constexpr static float period = 1440;

    TTF(vector<pair<float,float>> points) {
        this->points = points;
        setExtrema();
    }

    TTF(float weight) {
        this->points = {make_pair(0, weight), make_pair(period, weight)};
        setExtrema();
    }

    float evaluate(float t) {
        // cout << "t : " <<  t << endl;
        t = fmod(t, period);
        // cout << "mod : " << t << endl;
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

    static TTF chaining(TTF f1, TTF f2) {
        vector<pair<float,float>> points;
        TTF f(points);
        int lap = 0;
        int i = 0;
        // cout << f2.getPoints().size() << endl;
        while (f2.getPoints()[i].first + lap*period < f1.getPoints()[0].second) {
            i++;
            if (i==f2.getPoints().size()) {i=0; lap++;}
            // cout << i << endl;
        }
        int j = 0;
        while (true) {
            float bend_x = 0;
            float bend_y = 0;
            if (j==f1.getPoints().size()) {cout << "problem j1" << endl; exit(0);}
            if (i==f2.getPoints().size()) {i=0; lap++;} // TODO : i=0 ou 1
            pair<float,float> p = f1.getPoints()[j];
            pair<float,float> q = f2.getPoints()[i];
            if (q.first+lap*period == p.first+p.second) {
                // cout << "1 : lap : " << lap << endl;
                bend_x = p.first;
                bend_y = q.second + p.second;
                i++;
                j++;
            } else if (q.first+lap*period < p.first+p.second) {
                // cout << "2 : lap : " << lap << endl;
                if (j==0) {cout << "problem j2" << endl; exit(0);}
                pair<float,float> previous_p = f1.getPoints()[j-1];
                // bend_x = reverseChaining(previous_p, p, q.first+lap*TTF::period);
                float m = (p.first+p.second-previous_p.first-previous_p.second)/(p.first-previous_p.first);
                bend_x = (1/m)*(q.first+lap*TTF::period - previous_p.first - previous_p.second) + previous_p.first;
                bend_y = q.first + q.second - bend_x +lap*TTF::period;
                i++;
            } else {
                // cout << "3 : lap : " << lap << endl;
                if (i==0) {cout << "problem i1" << endl; exit(0);}
                pair<float,float> previous_q = f2.getPoints()[i-1];
                float m = (q.second - previous_q.second)/(q.first - previous_q.first);
                bend_x = p.first;
                bend_y = previous_q.second + m*(p.first + p.second - previous_q.first - lap*period) + p.second;
                j++;
            }
            f.tryToAddPoint(make_pair(modulo(bend_x, period), bend_y));
            if (bend_x>=period) break;
        }
        if (!f.respectsFIFO()) {cout << "FIFO chain" << endl; exit(0);}
        f.setExtrema();
        return f;
    }

    static float modulo(float a, float b) {
        if (a==b) return a;
        return a - b*floor(a/b);
    }

    // static TTF chaining2(TTF f1, TTF f2) {
    //     vector<pair<float,float>> points;
    //     TTF f(points);
    //     int index1 = 0;
    //     int index2 = 0;
    //     int lap = 0;
        
    //     float f_12 = f1.points[0].second + f2.evaluate(f1.points[0].first+f1.points[0].second);
    //     f.tryToAddPoint(make_pair(f1.points[0].first, f_12));
    //     while (index1<f1.points.size()-1 && index2<f2.points.size()-1) {
    //         pair<float,float> p = f1.points[index1];
    //         pair<float,float> q = f2.points[index2];
    //         pair<float,float> next_p = f1.points[index1+1];
    //         pair<float,float> next_q = f2.points[index2+1];
    //         float t = reverseChaining(p, next_p, next_q.first+lap*TTF::period);
    //         // if (t<p.first) cout << "HERE" << endl;
    //         if (t>next_p.first || p.first == next_p.first) {
    //             float f_12 = next_p.second + f2.evaluate(next_p.first+next_p.second);
    //             // float f_12_bis = next_p.second + interpolation(q, next_q, next_p.first+next_p.second-lap*TTF::period);
    //             // cout << f_12 - f_12_bis << endl;
    //             pair<float, float> newP = make_pair(next_p.first, f_12);
    //             f.tryToAddPoint(newP);
    //             index1++;
    //         } else {
    //             float f_12 = f1.evaluate(t) + next_q.second;
    //             // float f_12_bis = interpolation(p, next_p, t) + next_q.second;
    //             // cout << f_12 - f_12_bis << endl;
    //             pair<float, float> newP = make_pair(t, f_12);
    //             f.tryToAddPoint(newP);
    //             index2++;
    //             if (index2==f2.points.size()-1) {
    //                 index2 = 0;
    //                 lap++;   // repetition of TTF if exceed period
    //             }
    //         }
    //     }
    //     if (!f.respectsFIFO()) exit(0);
    //     f.setExtrema();
    //     return f;
    // }

    static TTF minimum(TTF f1, TTF f2) {
        vector<pair<float,float>> points;
        TTF f(points);
        int index1 = 0;
        int index2 = 0;
        if (f1.points[0].second <= f2.points[0].second) {
            f.tryToAddPoint(f1.points[0]);
        } else {
            f.tryToAddPoint(f2.points[0]);
        }
        while (index1<f1.points.size()-1 && index2<f2.points.size()-1) {
            pair<float,float> p = f1.points[index1];
            pair<float,float> q = f2.points[index2];
            pair<float,float> next_p = f1.points[index1+1];
            pair<float,float> next_q = f2.points[index2+1];
            pair<float,float> x = intersection(p, f1.points[index1+1], q, f2.points[index2+1]);
            if (x.first != -1) f.tryToAddPoint(x);
            float t1 = next_p.first;
            float t2 = next_q.first;
            if (t1 < t2) {
                if (next_p.second <= interpolation(q, next_q, t1)) {
                    f.tryToAddPoint(next_p);
                }
                index1++;
            } else if (t1 > t2) {
                if (next_q.second <= interpolation(p, next_p, t2)) {
                    f.tryToAddPoint(next_q);
                }
                index2++;
            } else {
                if (next_p.second <= next_q.second) {
                    f.tryToAddPoint(next_p);
                } else {
                    f.tryToAddPoint(next_q);
                }
                index1++;
            }
        }
        if (!f.respectsFIFO()) {cout << "FIFO min" << endl; exit(0);}
        f.setExtrema();
        return f;
    }

    bool respectsFIFO() {
        float previous_first = -1;
        float previous_second = -1;
        float frontBackDif = points.front().second - points.back().second;
        if (frontBackDif < 0) {
            float eps = 0.001;
            if (abs(frontBackDif) < eps) {
                cout << points.back().first << endl;
                points.back().second = points.front().second; // Correct
                cout << points.back().first << endl;
                cout << "Corrected FIFO" << endl;
            } else {
                return false;
            }
        }
        for (auto& p : this->points) {
            if (previous_first != -1) {
                float slope = (p.second-previous_second)/(p.first-previous_first);
                if (slope < -1) {
                    cout << slope << endl;
                    cout << p.first << endl;
                    // return false;
                }
            }
            previous_first = p.first;
            previous_second = p.second;
        }
        return true;
    }

    static TTF randomTransitTTF() {
        vector<pair<float,float>> points;
        int max = 60;
        int x = 0;
        points.push_back(make_pair(x,0));
        while (true) {
            int waitingTime = Random::randomInt(max);
            if (waitingTime > period-x) {
                waitingTime = period-x;
            }
            points.push_back(make_pair(x, waitingTime));
            points.push_back(make_pair(x + waitingTime, 0));
            x += waitingTime;
            if (x==period) break;
        }
        
        return TTF(points);
    }

    void print() {
        cout << "------Print------" << endl;
        for (auto& p : this->points) {
            cout << "make_pair(" << p.first << "," << p.second << "),";
        }
        cout << endl << "-----------------" << endl;
    }

    vector<pair<float,float>> getPoints() {
        return this->points;
    }

    float getMaxima() {
        return this->maxima;
    }
    
    float getMinima() {
        return this->minima;
    }

private:
    vector<pair<float,float>> points;
    float minima = -1;
    float maxima = -1;

    void tryToAddPoint(pair<float,float> p) {
        if (p.first<0 || p.second<0) {
            cout << "problem0 : " << p.first << " : " << p.second << endl;
            exit(0);
        }
        if (std::isinf(p.first) || std::isinf(p.second)) {
            cout << "problem1 : " << p.first << " : " << p.second << endl;
            exit(0);
        }
        // if (points.size() == 0) {
        //     addPoint(p);
        // } else {
        //     if (p.first < points.back().first && differentPoint(points.back(), p)) {
        //         cout << "problem2 : " << p.first << " : " << points.back().first << endl;
        //         cout << "-------- : " << p.second << " : " << points.back().second << endl;
        //         exit(0);
        //     }
        //     if (differentPoint(points.back(), p)) addPoint(p);
        // }
        addPoint(p);
    }

    void addPoint(pair<float,float> p) {
        if (points.size()>=2 && colinear(points[points.size()-2], points[points.size()-1], p)) points.pop_back();
        points.push_back(p);
    }

    void setExtrema() {
        for (auto& p : this->points) setExtrema(p);
    }

    void setExtrema(pair<float,float> p) {
        if (p.second > maxima || maxima == -1) maxima = p.second;
        if (p.second < minima || minima == -1) minima = p.second;
    }

    static bool colinear(pair<float,float> p1, pair<float,float> p2, pair<float,float> p3) {
        if (p1.first==p2.first && p2.first==p3.first) return true;
        float eps = 0.001;
        float potentialDifference = p2.second - interpolation(p1, p3, p2.first);
        // cout << p2.second << endl;
        // cout << interpolation(p1, p3, p2.first) << endl;
        // cout << potentialDifference << endl;
        return (abs(potentialDifference) < eps);
    }

    // static bool differentPoint(pair<float,float> previousPoint, pair<float,float> newPoint) {
    //     float eps = 0.001;
    //     return (abs(previousPoint.first - newPoint.first) > eps || abs(previousPoint.second - newPoint.second) > eps); 
    // }

    static float interpolation(pair<float,float> p1, pair<float,float> p2, float t) {
        float distance_down = (t - p1.first);
        float distance_up = (p2.first - t);
        float value_down = p1.second;
        float value_up = p2.second;
        return (distance_down * value_up + distance_up * value_down) / (distance_down + distance_up);
    }

    // f(t) = ((p2.second - p1.second)*t - p1.first*p2.second + p2.first*p1.second) / (p2.first - p1.first); (interpolation)
    // Solves t + f(t) = t2 for given t2 and f 
    static float reverseChaining(pair<float,float> p1, pair<float,float> p2, float t2) {
        cout << "_" << p1.first << endl;
        cout << "_" << p2.first << endl;
        cout << "_" << p1.second << endl;
        cout << "_" << p2.second << endl;
        cout << "_" << t2 << endl;
        float num = (t2*(p2.first - p1.first) + p1.first*p2.second - p2.first*p1.second);
        float den = (p2.second - p1.second + p2.first - p1.first);
        cout << "num : " << num << endl;
        cout << "den : " << num << endl;
        float t = num / den;
        return t;
    }
    
    static pair<float,float> intersection(pair<float,float> p, pair<float,float> p2, pair<float,float> q, pair<float,float> q2) {
        pair<float,float> r = p2 - p;
        pair<float,float> s = q2 - q;
        // cout << "p : " << p.first << " " << p.second << "q : " << q.first << " " << q.second << "r : " << r.first << " " << r.second << "s : " << s.first << " " << s.second << endl;
        float x = cross(r,s);
        if (x == 0) {
            // cout << "parallel : " << endl;
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

