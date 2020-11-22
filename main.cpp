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

//Main method
int main() {

    /*
     *
     *
     * HIER GEBEN WIR DIE EIGENSCHAFTEN DES EXPERIMENTS AN
     *
     *
     */

    //generate #goods
    int num_goods;
    cout << "Number Goods: ";
    cin >> num_goods;
    //int num_goods = 6;


    //vector<Bidder> bidders(5);

    //multiplier for valuation
    double i = 1.;

    //generate bidders with val, budget and spent_vec randomly
    int num_bidders;
    cout << "Number Bidders: ";
    cin >> num_bidders;
    //int num_bidders = 10;

    if (num_bidders > num_goods)
    {
        printf ("Error number bidders larger than number goods");
        exit (EXIT_FAILURE);
    }

    //num_iterations = Anzahl der Iterationen des Handels auf dem FM
    int num_iterations;
    cout << "Number Iterations: ";
    cin >> num_iterations;

    //Name der file, in die geschrieben wird
    string filename;
    cout << "Welchen Namen soll die File haben? ";
    cin >> filename;


    //num_iter_exp = Anzahl Ausführungen des Gesamtexperiments
    int num_iter_exp;
    cout << "Number Iterations Experiment: ";
    cin >> num_iter_exp;

    //FOR SCHLEIFE FÜR ANZAHL WIEDERHOLUNGEN DES GESAMTEXPERIMENTS
    for(int iter = 0; iter <= num_iter_exp; iter ++){


     /*
     *
     * BIS HIER GEBEN WIR DIE EIGENSCHAFTEN DES EXPERIMENTS AN
     *
     *
     *
     */

    vector<Bidder> bidders(num_bidders);

    for (int k = 0; k < num_bidders; ++k) {
        bidders[k].valuation.resize(num_goods);
        //valuation pro Gut und Bidder
        for (auto &v: bidders[k].valuation) v = (random_number(0, 11) + random_number(0, 15)) * i;
        bidders[k].budget = random_number(0, 11) + random_number(0, 31);
        bidders[k].spent.resize(num_goods, bidders[0].budget / (double) num_goods);
    }


    //int num_iterations = 2000;
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

        //print für jeden bidder und jede iteration dessen ???
        cout << "Iteration " << it << ":\n";
        for (int i = 0; i < bidders.size(); ++i) {
            cout << "Bidder " << i << ": " << bidders[i] << endl;
        }
        cout << endl;

        //writing to txt file

        /*ofstream myfile;
        myfile.open ("markets.txt", std::ios_base::app);
        myfile << "Iteration " << it << ":\n";
        for (int i = 0; i < bidders.size(); ++i) {
            myfile << "Bidder " << i << ": " << bidders[i] << endl;
        }
        myfile << endl;*/
    }

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
        //cout << "Demand: " << demand << endl;
        //cout << "Supply: " << prices[j] << endl;
    }

    //set precision
    int pre = 3;


    //macht das Sinn? Summe der Max_utils?
    //double max_util = 0;

    for (int i = 0; i < num_bidders; ++i) {
        cout << "Max Utility: " << std::setprecision(pre) << max_utility[i] << endl;
        //max_util = max_util + max_utility[i];
    }

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
        vector<vector<double>> integral_allocations(num_bidders, vector<double>(num_goods, 0.0));
        vector<vector<double>> final_allocations(num_bidders, vector<double>(num_goods, 0.0));


        //summe fraktionale Teile pro Gut über alle bidder
        vector<double> sum_frac(num_goods, 0.0);

        //Bieter 1 von Gut 1: 0,3 Bieter 2 v G1: 0,2 Bieter 3 v G1: 0,5
        //dann partial_sums = {0.3, 0.5, 1.0}
        //partial sums pro Gut (über alle Bidder)
        vector<double> partial_sums(num_bidders, 0.0);


        //die fraktionalen allokationen aus graph[i][j] werden auf fractional_allocations[i][j] addiert
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                fractional_allocations[i][j] = ((20 * (graph[i][j])) - (floor(20 * (graph[i][j]))));
                if (fractional_allocations[i][j] < 0.01 || fractional_allocations[i][j] > 0.99) {
                    fractional_allocations[i][j] = 0;
                }
                //fractional_allocation[i][j]+integral_allocation[i][j] = graph[i][j]
                integral_allocations[i][j] = round(20 * graph[i][j] - fractional_allocations[i][j]);
                final_allocations[i][j] = integral_allocations[i][j];
                //cout << "Bidder " << i << " has " << fractional_allocations[i][j] << " of good " << j << "\n";
            }
        }

// pro bidder berechne ich die summe der fraktionalen Teile pro (jeweils ein) gut j; wird für partial sums benötigt;
//sum_frac ist damit die summe der fraktionalen Teile (über alle Bidder) eines guts j
        for (int j = 0; j < num_goods; ++j) {
            for (int i = 0; i < num_bidders; ++i) {
                //jeder fraktionale Teil eines jeden bidders i des spezifischen gutes j wird aufaddiert:
                sum_frac[j] = sum_frac[j] + fractional_allocations[i][j];

            }
            //cout << "Gut " << j << " hat in Summe " << sum_frac[j] << " fraktionale Einheiten" << "\n";
        }


        //zuweisung der fraktionalen Teile auf den (per rndm number) gezogenen Bidder
        for (int j = 0; j < num_goods; ++j) {
            //sum_frac[j] = summe fraktionale teile Gut j. sum_frac[j]=0 => kein fraktionaler teil
            //Wird gut fraktional aufgeteilt? Wenn nicht -> continue
            if (sum_frac[j] < 0.01) continue;
            //intitialisierung der partial sums mit den fraktionalen werten (nur so ist rekursion in for schleife über partial sums möglich
            partial_sums[0] = fractional_allocations[0][j];

            //startet bei i=1 (wegen rekursion)
            for (int i = 1; i < num_bidders; ++i) {
                partial_sums[i] = partial_sums[i - 1] + fractional_allocations[i][j];
            }
            for (int i = 0; i < num_bidders; ++i) {
                //sum_frac[j] is die summe der fraktionalen Allokationen pro gut j über alle Bidder i
                partial_sums[i] /= sum_frac[j];
            }

            //zufallszahl zw. 0 und 1 (double)
            //srand(time(NULL)); funktioniert nicht so gut

            double rdm_number = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

            for (int i = 0; i < num_bidders; ++i) {
                //wenn zufallszahl <= partial_sums[i] => bieter i bekommt das fraktionale gut zugewiesen und break;
                if (rdm_number <= partial_sums[i]) {
                    final_allocations[i][j] += sum_frac[j];
                    break;
                }
            }
        }

        cout << "Original allocations:" << endl;
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                    if((20 * graph[i][j]) < 0.01) {
                        graph[i][j] = 0;
                    }
                cout << 20 * graph[i][j] << " ";
            }
            cout << "|";
        }
        cout << endl;

        cout << "Fractional allocations:" << endl;
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                cout << fractional_allocations[i][j] << " ";
            }
            cout << "|";
        }
        cout << endl;

        cout << "Integral allocations:" << endl;
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                cout << integral_allocations[i][j] << " ";
            }
            cout << "|";
        }
        cout << endl;

        cout << "Randomized rounding allocations: " << endl;
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                cout << final_allocations[i][j] << " ";
            }
            cout << "|";
        }
        cout << endl;

        /*
         * HIER WIRD IN DIE FILE GESCHRIEBEN
         */

    ofstream myfile;
    myfile.open (filename, std::ios_base::app);
    myfile << "Number Goods: " << num_goods << ", " << " Number Bidders: " << num_bidders << ", "  << " Number Iterations: " << num_iterations << "\n";
    myfile << "Original allocs: " << "\n";
    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            myfile << 20 * graph[i][j] << " ";
        }
        myfile << "|";
    }
    myfile << "\n";
    myfile << "fractional allocs: " << "\n";
    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            myfile << final_allocations[i][j] << " ";
        }
        myfile << "|";
    }
    myfile << "\n";

    //die neuen/upgedateten/update max_utils berechnen und ausdrucken

    //max_utility der gerundeten Alloks berechnen
    cout << "\n";
    cout << "max_utility for rounded alloc | max_utility: \n";
    myfile << "max_utility for rounded alloc | max_utility: \n";
    double rd_util = 0.0;
    vector<double> rd_max_utility(num_bidders);
    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            rd_util = rd_util + (((final_allocations[i][j]) / 20.0) * bidders[i].valuation[j]);
        }
        rd_max_utility[i] = rd_util;
        cout << rd_util << " | ";
        myfile << rd_util << " | ";
        cout << std::setprecision(pre)  << max_utility[i] << "\n";
        myfile << std::setprecision(pre)  << max_utility[i] << "\n";
        rd_util = 0.0;
    }
    myfile << "\n";




    }

    return 0;
}







