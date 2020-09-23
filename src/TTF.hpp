
#include <math.h>
#include <cmath>

/// Define the pairwise substraction
pair<int, int> operator -(const std::pair<int, int>& x, const std::pair<int, int>& y) {
    return make_pair(x.first - y.first, x.second - y.second);
}
/// Define the pairwise addition
pair<int, int> operator +(const std::pair<int, int>& x, const std::pair<int, int>& y) {
    return make_pair(x.first + y.first, x.second + y.second);
}
/// Define the multiplication of a pair by a scalar
pair<int, int> operator *(const int x, const std::pair<int, int>& y) {
    return make_pair(x*y.first, x*y.second);
}

class TTF {
public:
    inline constexpr static int period = 60*60*24; // 1 day in seconds

    /// Initialize a TTF from a sequence of points
    TTF(vector<pair<int,int>> points) {
        this->points = points;
        setExtrema();
    }

    /// Initialize a TTF from a constant weight
    TTF(int weight) {
        this->points = {make_pair(0, weight), make_pair(period, weight)};
        setExtrema();
    }

    /// Implement the 'evaluate' operation : return the weight of the TTF at the specified time 't'
    int evaluate(int t) {
        t = fmod(t, period);
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

     /// Implement the 'chaining' operation : return the TTF corresponding to the subsequent evaluation of the specified TTFs
    static TTF chaining(TTF f1, TTF f2) {
        vector<pair<int,int>> points;
        TTF f(points);
        int lap = 0;
        int i = 0;
        while (f2.getPoints()[i].first + lap*period < f1.getPoints()[0].second) {
            i++;
            if (i==f2.getPoints().size()) {i=0; lap++;}
        }
        int j = 0;
        while (true) {
            int bend_x = 0;
            int bend_y = 0;
            if (j==f1.getPoints().size()) {cout << "problem j1" << endl; exit(0);}
            if (i==f2.getPoints().size()) {i=0; lap++;}
            pair<int,int> p = f1.getPoints()[j];
            pair<int,int> q = f2.getPoints()[i];
            if (q.first+lap*period == p.first+p.second) {
                bend_x = p.first;
                bend_y = q.second + p.second;
                j++;
            } else if (q.first+lap*period < p.first+p.second) {
                if (j==0) {cout << "problem j2" << endl; exit(0);}
                pair<int,int> previous_p = f1.getPoints()[j-1];
                if (p.first != previous_p.first) {
                    double m = (p.first+p.second-previous_p.first-previous_p.second)/(p.first-previous_p.first);
                    double bend_x_double = (1/m)*(q.first+lap*TTF::period - previous_p.first - previous_p.second) + previous_p.first;
                    bend_x = correctDouble(bend_x_double);
                } else {
                    bend_x = previous_p.first;
                }
                bend_y = q.first + q.second - bend_x +lap*TTF::period;
                i++;
            } else {
                if (i==0) {cout << "problem i1" << endl; exit(0);}
                pair<int,int> previous_q = f2.getPoints()[i-1];
                double m = (q.second - previous_q.second)/(q.first - previous_q.first);
                bend_x = p.first;
                double bend_y_double = previous_q.second + m*(p.first + p.second - previous_q.first - lap*period) + p.second;
                bend_y = correctDouble(bend_y_double);
                j++;
            }
            f.tryToAddPoint(make_pair(modulo(bend_x, period), bend_y));
            if (bend_x>=period) break;
        }
        if (!f.respectsFIFO()) {cout << "FIFO chain" << endl; exit(0);}
        f.setExtrema();
        return f;
    }

    /// Implement the 'minimum' operation : return the pointwise minimum of the specified TTFs
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

    /// Check if the TTF respect the FIFO property, i.e.: its slope is never lower than -1
    bool respectsFIFO() {
        int previous_first = -1;
        int previous_second = -1;
        if (points.front().second < points.back().second) {
            cout << "Problem FIFO back front" << endl;
            return false;
        }
        for (auto& p : this->points) {
            if (previous_first != -1) {
                if (p.second-previous_second < -(p.first-previous_first)) {
                    cout << "Problem FIFO slope" << endl;
                    return false;
                }
            }
            previous_first = p.first;
            previous_second = p.second;
        }
        return true;
    }

    /// Generate a TTF representing waiting times. Random waiting times are generated between 60 seconds and the specified maximum value.
    static TTF randomTransitTTF(int max) {
        vector<pair<int,int>> points;
        int min = 60; // min waiting time : 1min
        int x = 0;
        points.push_back(make_pair(x,0));
        while (true) {
            int waitingTime = min+Random::randomInt(max-min);
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

    vector<pair<int,int>>& getPoints() {
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

    /// Add the specified point if it satisfies validity constraints
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

    /// Add the specified point to the list of points composing the TTF. If the point is colinear with the last two points, first remove the last point.
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

    /// Check if the 3 specified points are colinear
    static bool colinear(pair<int,int> p1, pair<int,int> p2, pair<int,int> p3) {
        return ((p2.second-p1.second)*(p3.first-p2.first) == (p3.second-p2.second)*(p2.first-p1.first));
    }

    static bool samePoint(pair<int,int> p, pair<int,int> q) {
        return (p.first == q.first && p.second == q.second);
    }

    /// Return the linear interpolation at the specified time, considering the line segment passing by the two specified points
    static int interpolation(pair<int,int> p1, pair<int,int> p2, int t) {
        int distance_down = (t - p1.first);
        int distance_up = (p2.first - t);
        int value_down = p1.second;
        int value_up = p2.second;
        double result = (double)(distance_down * value_up + distance_up * value_down) / (double)(distance_down + distance_up);
        return correctDouble(result);
    }
    
    /// Return the intersection between the two line segments defined by the 4 specified points
    static pair<int,int> intersection(pair<int,int> p, pair<int,int> p2, pair<int,int> q, pair<int,int> q2) {
        pair<int,int> r = p2 - p;
        pair<int,int> s = q2 - q;
        int cp = cross(r,s);
        if (cp == 0) {
            return make_pair(-1,-1);                    // Parallel : no intersection
        } else {
            double t = (double)cross(q-p, s) / (double)cp;
            double u = (double)cross(q-p, r) / (double)cp;
            if (t>= 0 && t<= 1 && u>= 0 && u<= 1) {
                double x_first = p.first + (t*r.first);
                double x_second = p.second + (t*r.second);
                pair<int,int> x_correct = make_pair(correctDouble(x_first), correctDouble(x_second));
                return x_correct;                       // Intersection
            } else {
                return make_pair(-1,-1);                // No intersection
            }
        }
    }

    /// Check if the specified double is close enough to an integer value, and if yes return this integer.
    static int correctDouble(double a) {
        double deltaInt = a - (int)a;
        if (deltaInt != 0) {
            double eps = 1.0e-10;
            if (deltaInt > 0.5) {
                deltaInt = ceilf(a) - a;
                if (deltaInt < eps) {
                    a = ceilf(a);
                } else {
                    cout << "problem4 : " << deltaInt << endl;
                    exit(0);
                }
            } else {
                if (deltaInt < eps) {
                    a = (int)a;
                } else {
                    cout << "problem4 : " << deltaInt << endl;
                    exit(0);
                }
            }
        }
        return a;
    }

    static int modulo(int a, int b) {
        if (a==b) return a;
        return a % b;
    }

    /// Returns the 2D cross product of the 2 specified vectors
    static int cross(pair<int,int> v1, pair<int,int> v2) { 
        return (v1.first * v2.second - v1.second * v2.first);
    }
};

