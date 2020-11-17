#include <iostream>
#include <vector>
#include <numeric>
#include <fstream>
#include <chrono>
#include <random>
#include <list>
#include <iomanip>
#include <cmath>
#include <stdlib.h>

//Proportional Response Dynamics

// returned:
// - Equilibrium prices
// - equilibrium allocations

//Paper: Distributed Algorithms via Gradient Descent for Fisher Markets

//floor function eingefügt


// gibt es pro Gut genau eine Einheit??? -> Annahme ja! (22.10.20)

using namespace std;

class Bidder {
public:
    vector<double> valuation; //was mir ein gut wert ist
    double budget;
    vector<double> spent; //für welches gut gibt bidder was aus (summe aller elem in spent ist budget)

    friend ostream &operator<<(ostream &os, const Bidder &b);
};

ostream &operator<<(ostream &os, const Bidder &b) {
    for (int j = 0; j < b.spent.size(); ++j) {
        os << b.spent[j] << " ";
    }
    return os;
}

int random_number(int lb, int ub) {
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 engine(seed);
    return (engine() % (ub - lb + 1)) + lb;
}

/***
 *
 * Main method
 *
 ***/

int main() {

    /*** NUMBER OF GOODs
     * BIDDERS
     * AND ITERATIONS
     *
     * SET PROPERTIES ANFANG
     *
     ***/


    //generate #goods
    int num_goods;
    cout << "Number Goods: ";
    cin >> num_goods;
    //int num_goods = 6;

    //multiplier for valuation
    double i = 1.;

    //generate bidders with val, budget and spent_vec randomly
    int num_bidders;
    cout << "Number Bidders: ";
    cin >> num_bidders;
    //int num_bidders = 10;

    //exception_exit
    if (num_bidders > num_goods) {
        printf("Error number bidders larger than number goods");
        exit(EXIT_FAILURE);
    }

    int num_iterations;
    cout << "Number Iterations: ";
    cin >> num_iterations;
    //int num_iterations = 2000;

    /***
    * SET PROPERTIES ENDE
    ***/

    ////////////////////////////////////////////////////////

    /***
     * Proportional Response Dynamics
     * Algorithm
     *
     * ANFANG
     ****/

    //set precision
    int pre = 3;

    vector<Bidder> bidders(num_bidders);

    for (int k = 0; k < num_bidders; ++k) {
        bidders[k].valuation.resize(num_goods);
        //valuation pro Gut und Bidder
        for (auto &v: bidders[k].valuation) v = (random_number(0, 11) + random_number(0, 15)) * i;
        bidders[k].budget = random_number(0, 11) + random_number(0, 31);
        bidders[k].spent.resize(num_goods, bidders[0].budget / (double) num_goods);
    }

    vector<double> prices(num_goods);
    for (int it = 0; it < num_iterations; ++it) {

        //in jeder iteration werden die preise des guts i auf die menge der preise,
        // die jeder bidder ausgegeben hat, gesetzt
        for (int j = 0; j < num_goods; ++j) {
            prices[j] = 0;
            for (int i = 0; i < bidders.size(); ++i)
                prices[j] += bidders[i].spent[j];

        }
        //update der valuations und spents pro bidder
        vector<vector<double>> update(bidders.size(), vector<double>(num_goods)); //
        for (int i = 0; i < bidders.size(); ++i) {
            for (int j = 0; j < num_goods; ++j) {
                update[i][j] = bidders[i].valuation[j] * bidders[i].spent[j] / prices[j];

            }
        }

        //new bid vector for next iteration
        for (int i = 0; i < bidders.size(); ++i) {
            for (int j = 0; j < num_goods; ++j) {
                bidders[i].spent[j] =
                        bidders[i].budget * update[i][j] / accumulate(update[i].begin(), update[i].end(), 0.0);

            }
        }

        //print für jeden bidder und jede iteration dessen Allokation des Guts 1 bis n
        cout << "Iteration " << it << ":\n";
        for (int i = 0; i < bidders.size(); ++i) {
            cout << "Bidder " << i << ": " << bidders[i] << endl;
        }
        cout << endl;


        //von Max utility und utility (im equilibrium sind diese gleich)

        vector<double> utility(num_bidders);
        vector<double> max_utility(num_bidders);
        for (int b = 0; b < num_bidders; ++b) {
            max_utility[b] = 0;
            for (int i = 0; i < num_goods; ++i) {
                utility[b] += bidders[b].valuation[i] * bidders[b].spent[i] / prices[i]; //Aufpassen wenn prices[i] = 0!
                if (max_utility[b] < bidders[b].valuation[i] / prices[i]) {
                    max_utility[b] = bidders[b].valuation[i] / prices[i];
                }
            }

            max_utility[b] *= bidders[b].budget;
        }

        // save utility from start
        vector<double> val_start(num_bidders);
        for (int b = 0; b < num_bidders; ++b) {
            for (int i = 0; i < num_goods; ++i) {
                val_start[b] = bidders[b].valuation[i];
            }
        }



    /***
    * Proportional Response Dynamics
    * Algorithm
    *
    * ENDE
    *
    ***/


    cout << endl;
    cout << "Fraktionales/optimales Ergebnis: ";
    cout << endl;
    for (int i = 0; i < num_bidders; ++i) {
        cout << "Max Utility: " << setprecision(pre) << max_utility[i] << endl;
    }

}



    //graph ist der graph mit den Einträgen, welche die Kantengewichte (=Alloks) sind

    /*** Write allocations to graph ***/
    vector<vector<double>> graph(num_bidders, vector<double>(num_goods));
    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            graph[i][j] = bidders[i].spent[j] / prices[j];
        }
    }

    /***
     * fractional and integral parts
    ***/

    //fraktionale allokation von Bieter i und gut j
    vector<vector<double>> fractional_allocations(num_bidders, vector<double>(num_goods, 0.0));

    //summe fraktionale Teile pro Gut
    vector<double> sum_frac(num_goods, 0.0);


    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            if (20 * graph[i][j] < 0.01) graph[i][j] = 0;
            fractional_allocations[i][j] = ((20 * (graph[i][j])) - (floor(20 * (graph[i][j]))));
            cout << "Bidder " << i << " has " << fractional_allocations[i][j] << " of good " << j << "\n";
        }
    }


    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            sum_frac[j] = 0.0;
            if (20 * graph[i][j] < 0.01) graph[i][j] = 0;
            sum_frac[j] = sum_frac[j] + fractional_allocations[i][j];
        }
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                cout << "Bidder " << i << " hat in Summe " << sum_frac[j] << " fraktionale Einheiten des Guts " << j
                     << "\n";
            }
        }
    }






    return 0;

}






