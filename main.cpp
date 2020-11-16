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

        }

    /***
    * Proportional Response Dynamics
    * Algorithm
    *
    * ENDE
    *
    ***/

    ////////////////////////////////////////////////////////

        //Optimales Ergebnis//

        cout << endl;
        cout << "Fraktionales/optimales Ergebnis: ";
        cout << endl;
        for (int j = 0; j < num_goods; ++j) {
            double demand = 0;
            double supply = 0;
            for (int i = 0; i < bidders.size(); ++i) {
                demand += bidders[i].spent[j] / prices[j];
            }
        }

        //set precision
        int pre = 3;


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


        //for-schleifen hier getauscht, da wir pro Gut (über alle Bidder) die kummulierte Anzahl wollen//

        //frac enthält die fraktionalen Teile der Allokation

        //fraktionale allokation von Bieter i und gut j
        //***
        vector<vector<double>> fractional_allocations(num_bidders, vector<double>(num_goods, 0.0));
        //**

        //summe fraktionale Teile pro Gut
        vector<double> frac(num_goods, 0.0);
        cout << "Summe der fraktionalen Teile Gut 1 bis " << num_goods << ": \n";
        //vector<int> fracVec(num_goods);
        for (int j = 0; j < num_goods; ++j) {
            for (int i = 0; i < num_bidders; ++i) {
                if (20 * graph[i][j] < 0.01) graph[i][j] = 0;
                //frac_update := ??
                double frac_update = (20 * (graph[i][j])) - (floor(20 * (graph[i][j])));
                if (frac_update > 0.01 && frac_update < 0.99) {
                    frac[j] = frac[j] + frac_update;
                }
            }
            if (frac[j] < 0.01) {
                frac[j] = 0;
            }
            if (j == (num_goods - 1)) {
                cout << std::setprecision(pre) << frac[j] << "\n";
                break;
            }
            cout << std::setprecision(pre) << frac[j] << " ";
            cout << " | ";
        }


        cout << endl;
        cout << "fraktionalen Teile Gut 1 bis " << num_goods << " pro Bidder: \n";


        for (int j = 0; j < num_goods; ++j) {
            cout << "## " << " Gut " << j << " ## \n";
            for (int i = 0; i < num_bidders; ++i) {
                if ((20 * graph[i][j]) < 0.001) {
                    graph[i][j] = 0;
                }
                if (i == (num_bidders - 1)) {
                    cout << " " << "Bidder " << i << " : " << (20 * (graph[i][j]) - floor(20 * (graph[i][j])));
                    break;
                }
                cout << " " << "Bidder " << i << " : " << (20 * (graph[i][j]) - floor(20 * (graph[i][j]))) << " | ";
            }
            cout << endl;
        }


        //print für jeden bidder und jede iteration dessen Allokation des Guts 1 bis n

        for (int j = 0; j < num_goods; ++j) {
            for (i = 0; i < num_bidders; ++i) {

            }
            //frac[j] = summe fraktionale teile Gut j. frac[j] => kein fraktionaler teil
            if (frac[j] == 0) {
                continue;
            }
            //Wird gut fraktional aufgeteilt? Wenn nicht -> continue
            vector<double> partial_sums(num_bidders, 0.0);

            //BSP: Bieter 1: 0,3 Bieter 2: 0,2 Bieter 3: 0,5
            //partial_sums = {0.3, 0.5, 1.0}

            //warum diese zuweisung: ?
            partial_sums[0] = fractional_allocations[0][j];

            for (int i = 1; i < num_bidders; ++i) {
                partial_sums[i] = partial_sums[i - 1] + fractional_allocations[i][j];
            }
            for (int i = 0; i < num_bidders; ++i) {
                partial_sums[i] /= frac[j];
            }
            //zufallszahl zw. 0 und 1 (double)
            double rdm_number = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            for (int i = 0; i < num_bidders; ++i) {
                //wenn zufallszahl <= partial_sums[i] => bieter i bekommt das fraktionale gut zugewiesen und break;
                if (rdm_number <= partial_sums[i]) {
                    //graph[i][j] ist die allokation, also addiere ich die fraktionalen Teile eines Guts hier auf
                    graph[i][j] = frac[j] + graph[i][j];
                    frac[j] = 0;
                    break;
                }
            }
        }

        cout << endl;
        for (int j = 0; j < num_goods; ++j) {
            cout << "Gut: " << j << "\n";
            for (int i = 0; i < num_bidders; ++i) {
                cout << "Bidder " << i << " bekommt " << graph[i][j] << " des Guts " << j << "\n";
            }
        }


        return 0;

    }







/* Kantengewichte sind die Allokationen der Güter auf die Bieter!! nicht die valuations */




