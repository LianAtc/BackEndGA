/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ClPoblacion.h
 * Author: 20180954 Lian Abril Tume Cusi
 * Created on 4 de noviembre de 2024, 10:24 PM
 */

#ifndef CLPOBLACION_H
#define CLPOBLACION_H
#include <fstream>
using namespace std;

#include "ClCromosoma.h"

class Poblacion {
private:
    std::vector<Cromosoma> cromosomas;

public:
    Poblacion(int tamanoPoblacion, int numGenes) {
        for (int i = 0; i < tamanoPoblacion; ++i) {
            cromosomas.push_back(Cromosoma(numGenes));
        }
    }

    std::vector<Cromosoma>& getCromosomas() {
        return cromosomas;
    }

    void setCromosoma(int index, const Cromosoma& cromosoma) {
        if (index >= 0 && index < cromosomas.size()) {
            cromosomas[index] = cromosoma;
        }
    }

    Cromosoma getBestCromosoma() const {
        double bestFitness = cromosomas[0].getFitness();
        Cromosoma bestCromosoma = cromosomas[0];

        for (const Cromosoma& cromosoma : cromosomas) {
            if (cromosoma.getFitness() > bestFitness) {
                bestFitness = cromosoma.getFitness();
                bestCromosoma = cromosoma;
            }
        }
        return bestCromosoma;
    }

    void print() const {
        for (const Cromosoma& cromosoma : cromosomas) {
            cromosoma.print();
        }
    }
};


#endif /* CLPOBLACION_H */
