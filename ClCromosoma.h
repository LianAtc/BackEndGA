/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ClCromosoma.h
 * Author: 20180954 Lian Abril Tume Cusi
 * Created on 4 de noviembre de 2024, 10:14 PM
 */

#ifndef CLCROMOSOMA_H
#define CLCROMOSOMA_H
#include <fstream>

#include "ClPieza.h"

using namespace std;

class Cromosoma {
private:
    vector<Pieza> genes;
    double fitness;

public:
    Cromosoma(int numGenes = 0) : genes(numGenes), fitness(0.0) {}

    void setGene(int index, const Pieza& pieza) {
        if (index >= 0 && index < genes.size()) {
            genes[index] = pieza;
        }
    }

    Pieza getGene(int index) const {
        return (index >= 0 && index < genes.size()) ? genes[index] : Pieza();
    }

    std::vector<Pieza>& getGenes() {
        return genes;
    }

    void setFitness(double fit) {
        fitness = fit;
    }

    double getFitness() const {
        return fitness;
    }

    void print() const {
        for (const Pieza& pieza : genes) {
            pieza.imprimirPieza();
        }
        std::cout << " | Fitness: " << fitness << std::endl;
    }
};


#endif /* CLCROMOSOMA_H */

