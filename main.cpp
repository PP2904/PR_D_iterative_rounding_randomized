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
#include <ctime>

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


    //+++++++++++++++++++++++++++++++++++++++++++++++

    //hard gecoded:

    /*int num_goods = 40;

    int num_bidders = 10;

    int num_iterations = 100;

    int num_iter_exp = 1;*/

    //+++++++++++++++++++++++++++++++++++++++++++++++

    //generate #goods
    int num_goods;
    cout << "Number Goods: ";
    cin >> num_goods;

    //vector<Bidder> bidders(5);

    //multiplier for valuation
    double i = 1.;

    //generate bidders with val, budget and spent_vec randomly
    int num_bidders;
    cout << "Number Bidders: ";
    cin >> num_bidders;


    //num_iterations = Anzahl der Iterationen des Handels auf dem FM
    int num_iterations;
    cout << "Number Iterations: ";
    cin >> num_iterations;

    double quantItem;
    cout << "Quantität eines Guts: ";
    cin >> quantItem;


  /*  //Name der file, in die geschrieben wird
    string filename;
    cout << "Welchen Namen soll die File haben? ";
    cin >> filename;
*/

    //num_iter_exp = Anzahl Ausführungen des Gesamtexperiments
   int num_iter_exp;
    cout << "Number Iterations Experiment: ";
    cin >> num_iter_exp;

    //+++++++++++++++++++++++++++++++++++++++++++++++

    /*
    *
    * BIS HIER GEBEN WIR DIE EIGENSCHAFTEN DES EXPERIMENTS AN
    *
    *
    *
    */

   /* if (num_bidders > num_goods)
    {
        printf ("Error number bidders larger than number goods");
        exit (EXIT_FAILURE);
    }*/

    auto start = std::chrono::system_clock::now();

    //FOR SCHLEIFE FÜR ANZAHL WIEDERHOLUNGEN DES GESAMTEXPERIMENTS
    for(int iter = 0; iter < num_iter_exp; iter ++){

    vector<Bidder> bidders(num_bidders);

    for (int k = 0; k < num_bidders; ++k) {
        bidders[k].valuation.resize(num_goods);
        //valuation pro Gut und Bidder
        for (auto &v: bidders[k].valuation) v = (random_number(1, 11) + random_number(1, 15)) * i;
        bidders[k].budget = random_number(1, 11) + random_number(1, 31);
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

        //print für jeden bidder und jede iteration dessen allocation von Gut 0 bis n
        cout << "Iteration " << it << ":\n";
        for (int i = 0; i < bidders.size(); ++i) {
            cout << "Bidder " << i << ": " << bidders[i] << endl;
        }
        cout << endl;

    }

    //von Max utility und utility (im equilibrium sind diese gleich)

    vector<double> utility(num_bidders);
    vector<double> max_utility(num_bidders);
    for (int b = 0; b < num_bidders; ++b) {
        max_utility[b] = 0;
        for (int i = 0; i < num_goods; ++i) {
            if(prices[i]==0) {
                printf ("prices is 0");
                exit (EXIT_FAILURE);
            }
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



    for (int i = 0; i < num_bidders; ++i) {
        cout << "Max Utility: " << std::setprecision(pre) << max_utility[i] << endl;
        //max_util = max_util + max_utility[i];
    }

        /*** Write allocations to graph ***/
        vector<vector<double>> graph(num_bidders, vector<double>(num_goods));
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                graph[i][j] = bidders[i].spent[j] / prices[j]; //bidders.spent = nan?
                if(isnan(graph[i][j])) {
                    graph[i][j] = 0.0000001;
                }


                //checken auf nan
                if(isnan(graph[i][j])) {
                    printf ("graph value is nan");
                    exit (EXIT_FAILURE);
                }


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
                fractional_allocations[i][j] = ((quantItem * (graph[i][j])) - (floor(quantItem * (graph[i][j]))));
                if (fractional_allocations[i][j] < 0.01 || fractional_allocations[i][j] > 0.99) {
                    fractional_allocations[i][j] = 0;
                }
                //fractional_allocation[i][j]+integral_allocation[i][j] = graph[i][j]
                integral_allocations[i][j] = round(quantItem * graph[i][j] - fractional_allocations[i][j]);
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
                    if((quantItem * graph[i][j]) < 0.01) {
                        graph[i][j] = 0;
                    }
                cout << quantItem * graph[i][j] << " ";
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

        //ofstream myfile;
        ofstream myfile2;


    myfile2.open ("results.txt", std::ios_base::app);
        myfile2 << "max_utility for rounded alloc | max_utility" << "\n";
        myfile2 << "Number Goods: " << num_goods << ", " << " Number Bidders: " << num_bidders << ", "  << " Number Iterations: " << num_iterations << "\n";


    //die neuen/upgedateten/update max_utils berechnen und ausdrucken

    //max_utility der gerundeten Alloks berechnen
    cout << "\n";
    cout << "max_utility for rounded alloc | max_utility: | integrality gap: \n";
    //myfile << "max_utility for rounded alloc | max_utility: | integrality gap: \n";
    //myfile2 << ", \n";


    double rd_util = 0.0;
    vector<double> rd_max_utility(num_bidders);

    double int_gap = 0.0;
    double print_int_gap = 0.0;
    double avg_int_gap = 0.0;


    for (int i = 0; i < num_bidders; ++i) {
        for (int j = 0; j < num_goods; ++j) {
            rd_util = rd_util + (((final_allocations[i][j]) / quantItem) * bidders[i].valuation[j]);
        }
        //max_utility for rounded alloc
        rd_max_utility[i] = rd_util;
        cout << rd_util << " | ";
        //myfile << rd_util << " | ";
        myfile2 << rd_util << " | ";

        //max_utility:
        cout << std::setprecision(pre)  << max_utility[i] << " | ";
        //myfile << std::setprecision(pre)  << max_utility[i] << " | ";
        myfile2 << std::setprecision(pre)  << max_utility[i] << "\n";

        //integrality gap:
        if(rd_max_utility[i] <= max_utility[i]) {
            cout << std::setprecision(pre) << rd_max_utility[i] / max_utility[i] << "\n";
            // myfile << std::setprecision(pre)  << rd_max_utility[i]/max_utility[i] << "\n";
            int_gap = int_gap + (rd_max_utility[i] / max_utility[i]);
        }
        //integer sol > optimal sol
        else{
            cout << " \n";
        }
        /* if(rd_max_utility[i] > max_utility[i]){
            cout << std::setprecision(pre)  << max_utility[i]/rd_max_utility[i] << "\n";
            //myfile << std::setprecision(pre)  << max_utility[i]/rd_max_utility[i] << "\n";
            int_gap = int_gap + (max_utility[i]/rd_max_utility[i]);
            }*/
        print_int_gap = int_gap;
        if(i==(num_bidders-1)){
            avg_int_gap = print_int_gap; // /num_bidders;
        }
        int_gap = 0.0;
        rd_util = 0.0;
    }
    /*myfile << "Integrality gap is in average: " << avg_int_gap;
    myfile << "\n";
    myfile << "\n";
    myfile << "\n";*/


    }



    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    cout << "\n";
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s\n";


    return 0;
}







