
#include <math.h>
#include <cmath>

pair<int, int> operator -(const std::pair<int, int>& x, const std::pair<int, int>& y) {
    return make_pair(x.first - y.first, x.second - y.second);
}
pair<int, int> operator +(const std::pair<int, int>& x, const std::pair<int, int>& y) {
    return make_pair(x.first + y.first, x.second + y.second);
}
pair<int, int> operator *(const int x, const std::pair<int, int>& y) {
    return make_pair(x*y.first, x*y.second);
}

class TTF {
public:
    inline constexpr static int period = 1440;

    TTF(vector<pair<int,int>> points) {
        this->points = points;
        setExtrema();
    }

    TTF(int weight) {
        this->points = {make_pair(0, weight), make_pair(period, weight)};
        setExtrema();
    }

    int evaluate(int t) {
        // cout << "t : " <<  t << endl;
        t = fmod(t, period);
        // cout << "mod : " << t << endl;
        // logarithmic search
        auto it = std::lower_bound(this->points.begin(), this->points.end(), make_pair(t, 0));
        int index_up = std::distance(points.begin(), it);
        int result = 0;
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
        vector<pair<int,int>> points;
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
            // cout << endl << "indexes : " << j << " ; " << i << endl;
            int bend_x = 0;
            int bend_y = 0;
            if (j==f1.getPoints().size()) {cout << "problem j1" << endl; exit(0);}
            if (i==f2.getPoints().size()) {i=0; lap++;} // TODO : i=0 ou 1
            pair<int,int> p = f1.getPoints()[j];
            pair<int,int> q = f2.getPoints()[i];
            if (q.first+lap*period == p.first+p.second) {
                // cout << "1 : lap : " << lap << endl;
                bend_x = p.first;
                bend_y = q.second + p.second;
                // i++;
                j++;
            } else if (q.first+lap*period < p.first+p.second) {
                // cout << "2 : lap : " << lap << endl;
                if (j==0) {cout << "problem j2" << endl; exit(0);}
                pair<int,int> previous_p = f1.getPoints()[j-1];
                float m = (p.first+p.second-previous_p.first-previous_p.second)/(p.first-previous_p.first);
                bend_x = (1/m)*(q.first+lap*TTF::period - previous_p.first - previous_p.second) + previous_p.first;
                bend_y = q.first + q.second - bend_x +lap*TTF::period;
                i++;
            } else {
                // cout << "3 : lap : " << lap << endl;
                if (i==0) {cout << "problem i1" << endl; exit(0);}
                pair<int,int> previous_q = f2.getPoints()[i-1];
                float m = (q.second - previous_q.second)/(q.first - previous_q.first);
                bend_x = p.first;
                bend_y = previous_q.second + m*(p.first + p.second - previous_q.first - lap*period) + p.second;
                j++;
            }
            // cout << "point : " << bend_x << " ; " << bend_y << endl;
            f.tryToAddPoint(make_pair(modulo(bend_x, period), bend_y));
            if (bend_x>=period) break;
        }
        if (!f.respectsFIFO()) {cout << "FIFO chain" << endl; exit(0);}
        f.setExtrema();
        return f;
    }

    static int modulo(int a, int b) {
        if (a==b) return a;
        return a % b;
    }

    static TTF minimum(TTF f1, TTF f2) {
        vector<pair<int,int>> points;
        TTF f(points);
        int index1 = 0;
        int index2 = 0;
        if (f1.points[0].second <= f2.points[0].second) {
            f.tryToAddPoint(f1.points[0]);
        } else {
            f.tryToAddPoint(f2.points[0]);
        }
        while (index1<f1.points.size()-1 && index2<f2.points.size()-1) {
            pair<int,int> p = f1.points[index1];
            pair<int,int> q = f2.points[index2];
            pair<int,int> next_p = f1.points[index1+1];
            pair<int,int> next_q = f2.points[index2+1];
            pair<int,int> x = intersection(p, f1.points[index1+1], q, f2.points[index2+1]);
            if (x.first != -1) f.tryToAddPoint(x);
            int t1 = next_p.first;
            int t2 = next_q.first;
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
        int previous_first = -1;
        int previous_second = -1;
        int frontBackDif = points.front().second - points.back().second;
        if (frontBackDif < 0) {
            float eps = 0.001;
            if (abs(frontBackDif) < eps) {
                cout << points.back().first << endl;
                points.back().second = points.front().second; // Correct
                cout << points.back().first << endl;
                cout << "Corrected FIFO" << endl;
            } else {
                cout << "Problem FIFO back front" << endl;
                return false;
            }
        }
        for (auto& p : this->points) {
            if (previous_first != -1) {
                float slope = (p.second-previous_second)/(p.first-previous_first);
                if (slope < -1) {
                    cout << "Problem FIFO slope" << endl;
                    return false;
                }
            }
            previous_first = p.first;
            previous_second = p.second;
        }
        return true;
    }

    bool isTransitTTF() {
        int previous_first = -1;
        int previous_second = -1;
        for (pair<int,int> p : points) {
            if (previous_first != -1) {
                if (p.first == previous_first && p.second > previous_second) {
                    // step
                } else if (p.first > previous_first && p.first-previous_first == previous_second-p.second) {
                    // -1 slope
                } else {
                    cout << "Not transit TTF" << endl;
                    cout << (p.first > previous_first) << " ----- " << (p.first-previous_first == previous_second-p.second) << endl;
                    cout << "dx : " << p.first-previous_first << endl;
                    cout << "dy : " << previous_second-p.second << endl;
                    cout << "pr : (" << previous_first << "," << previous_second << ")" << endl;
                    cout << "p : (" << p.first << "," << p.second << ")" << endl;
                    return false;
                }
            }
            previous_first = p.first;
            previous_second = p.second;
        }
        if (points.back().second < points.back().second) return false;
        return true;
    }

    static TTF randomTransitTTF() {
        vector<pair<int,int>> points;
        int max = 60;
        int x = 0;
        points.push_back(make_pair(x,0));
        while (true) {
            int waitingTime = 1+Random::randomInt(max);
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

    vector<pair<int,int>> getPoints() {
        return this->points;
    }

    int getMaxima() {
        return this->maxima;
    }
    
    int getMinima() {
        return this->minima;
    }

private:
    vector<pair<int,int>> points;
    int minima = -1;
    int maxima = -1;

    void tryToAddPoint(pair<int,int> p) {
        if (p.first<0 || p.second<0) {
            cout << "problem0 : " << p.first << " : " << p.second << endl;
            exit(0);
        }
        if (std::isinf(p.first) || std::isinf(p.second)) {
            cout << "problem1 : " << p.first << " : " << p.second << endl;
            exit(0);
        }
        if (points.size() != 0 && p.first < points.back().first) {
            cout << "problem2 : " << p.first << " : " << points.back().first << endl;
            cout << "-------- : " << p.second << " : " << points.back().second << endl;
            exit(0);
        }
        if (points.size()!=0 && samePoint(p, points.back())) return;
        addPoint(p);
    }

    void addPoint(pair<int,int> p) {
        if (points.size()>=2 && colinear(points[points.size()-2], points[points.size()-1], p)) points.pop_back();
        points.push_back(p);
    }

    void setExtrema() {
        for (auto& p : this->points) setExtrema(p);
    }

    void setExtrema(pair<int,int> p) {
        if (p.second > maxima || maxima == -1) maxima = p.second;
        if (p.second < minima || minima == -1) minima = p.second;
    }

    static bool colinear(pair<int,int> p1, pair<int,int> p2, pair<int,int> p3) {
        if (p1.first==p2.first && p2.first==p3.first) return true;
        float eps = 0.00001;
        float potentialDifference = p2.second - interpolation(p1, p3, p2.first);
        return (abs(potentialDifference) < eps);
    }

    static bool samePoint(pair<int,int> p, pair<int,int> q) {
        return (p.first == q.first && p.second == q.second);
    }

    static float interpolation(pair<int,int> p1, pair<int,int> p2, int t) {
        float distance_down = (t - p1.first);
        float distance_up = (p2.first - t);
        float value_down = p1.second;
        float value_up = p2.second;
        return (distance_down * value_up + distance_up * value_down) / (distance_down + distance_up);
    }
    
    static pair<float,float> intersection(pair<int,int> p, pair<int,int> p2, pair<int,int> q, pair<int,int> q2) {
        pair<int,int> r = p2 - p;
        pair<int,int> s = q2 - q;
        int cp = cross(r,s);
        if (cp == 0) {
            // cout << "parallel : " << endl;
            return make_pair(-1,-1);                    // Parallel : no intersection
        } else {
            int t = cross(q-p, s) / cp;
            int u = cross(q-p, r) / cp;
            if (t>= 0 && t<= 1 && u>= 0 && u<= 1) {
                pair<int,int> x = p + (t*r);
                // x.first = correctFloat(x.first);
                // x.second = correctFloat(x.second);
                return x;                       // Intersection
            } else {
                // cout << "no intersection : " << endl;
                return make_pair(-1,-1);                // No intersection
            }
        }
    }

    // static int correctFloat(float a) {
    //     float deltaInt = a - (int)a;
    //     if (deltaInt != 0) {
    //         float eps = 0.0001;
    //         if (deltaInt > 0.5) {
    //             deltaInt = ceilf(a) - a;
    //             if (deltaInt < eps) {
    //                 a = ceilf(a);
    //             } else {
    //                 cout << "problem4 : " << deltaInt << endl;
    //                 exit(0);
    //             }
    //         } else {
    //             if (deltaInt < eps) {
    //                 a = (int)a;
    //             } else {
    //                 cout << "problem4 : " << deltaInt << endl;
    //                 exit(0);
    //             }
    //         }
    //         // cout << deltaInt << endl;
    //     }
    //     return a;
    // }

    static int cross(pair<int,int> v1, pair<int,int> v2) { // Returns the 2D cross product of the 2 specified vectors
        return (v1.first * v2.second - v1.second * v2.first);
    }
};

