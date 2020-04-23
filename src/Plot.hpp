#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

using std::cout;
using std::endl;

using std::vector;

class Plot {
    public:
        static vector<float> plotDistribution(vector<float> y, bool loglog=false) {
            vector<float> x(y.size(), 0.0);
            for (int i=0; i<x.size(); i++) {
                x[i] = i;
            }

            int nonZeroIndex = 0;
            for (int i=x.size()-1; i>=0; i--) {
                if (y[i] != 0) {
                    nonZeroIndex = i;
                    break;
                }
            }
            vector<float>::const_iterator yfirst = y.begin();
            vector<float>::const_iterator ylast = y.begin() + nonZeroIndex + 1;
            vector<float> newY(yfirst, ylast);

            vector<float>::const_iterator xfirst = x.begin();
            vector<float>::const_iterator xlast = x.begin() + nonZeroIndex + 1;
            vector<float> newX(xfirst, xlast);

            if (loglog) {
                plt::loglog(newX, newY);
            } else {
                plt::scatter(newX, newY, 20);
            }
            plt::title("Degree Distribution");
            plt::show();

            return newX;
        }

        static void plotCoopLevel(vector<float> y) {
            vector<float> x(y.size(), 0.0);
            for (int i=0; i<x.size(); i++) {
                x[i] = i;
            }
            plt::scatter(x, y, 10);
            plt::title("Coop level");
            plt::show();
        }

        static float getGaussian(float mean, float sd, float x) {
            float a = -pow(x-mean, 2);
            float b = a/(2*pow(mean, 2));
            float c = exp(b);
            float d = mean*pow(2*M_PI, 0.5);
            return c/d;
        }

        static void plotGaussian(float mean, float sd, vector<float> x) {
            int gran = 5;
            vector<float> newX(x.size()*gran, 0.0);
            vector<float> y(newX.size(), 0.0);
            //cout << newX.size() << endl;
            //cout << y.size() << endl;
            for (float i=0; i<newX.size(); i+=1) {
                y[i] = getGaussian(mean, sd, float(i)/float(gran));
                newX[i] = i/gran;
            }
            plt::scatter(newX, y, 20);
            plt::show();
        }

        static float getExp(float lambda, float x) {
            return lambda*exp(-lambda*x);
        }

        static void plotExp(float lambda, vector<float> x, bool loglog=false) {
            int gran = 5;
            vector<float> newX(x.size()*gran, 0.0);
            vector<float> y(newX.size(), 0.0);
            //cout << newX.size() << endl;
            //cout << y.size() << endl;
            for (float i=0; i<newX.size(); i+=1) {
                y[i] = getExp(lambda, float(i)/float(gran));
                newX[i] = i/gran;
            }
            if (loglog) {
                plt::loglog(newX, y);
            } else {
                plt::scatter(newX, y, 20);
            }
            plt::show();
        }

        static float distributionSd(vector<float> distribution, float mean, int n) {
            float variance = 0;
            for (int i=0; i<distribution.size(); i++) {
                variance += distribution[i]*pow(i - mean, 2);
            }
            variance /= float(n);
            float sd = pow(variance, 0.5);
            return sd;
        }

        static float distributionMean(vector<float> distribution, int n) {
            float mean = 0;
            for (int i=0; i<distribution.size(); i++) {
                mean += i*distribution[i];
            }
            mean /= float(n);
            return mean;
        }
};
