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
    
    vector<Pieza>& getGenes() { return genes; }

    Pieza getGene(int index) const {
        return (index >= 0 && index < genes.size()) ? genes[index] : Pieza();
    }

    const vector<Pieza>& getGenes() const {
        return genes;
    }

    void setFitness(double fit) {
        fitness = fit;
    }
    
    bool operator==(const Cromosoma& otro) const {
        return this->getGenes() == otro.getGenes(); // Compara los genes de los cromosomas
    }

    double getFitness() const {
        return fitness;
    }
    
    void imprimir() const {
        for (const Pieza& pieza : genes) {
            if(pieza.getID() == -1) break;
            pieza.imprimirPieza();
        }
        std::cout << " | Fitness: " << fitness << std::endl;
    }
};


#endif /* CLCROMOSOMA_H */

