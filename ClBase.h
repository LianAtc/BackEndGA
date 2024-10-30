/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ClBase.h
 * Author: 20180954 Lian Abril Tume Cusi
 * Created on 24 de octubre de 2024, 09:16 PM
 */

#ifndef CLBASE_H
#define CLBASE_H
#include <fstream>
using namespace std;

class Stock {
private:
    int w, h;
    list<Pieza> listaDefectos; // Solo contiene piezas defectuosas

public:
    Stock(int w, int h) : w(w), h(h) {}
    Stock() : w(0), h(0) {}

    void agregarPieza(const Pieza& pieza) {
        listaDefectos.push_back(pieza);
    }
    float getW() const { return w; }
    float getH() const { return h; }
    void setH(int nuevoAlto) {h = nuevoAlto;}
    void imprimirPiezas() const {
        cout << "Piezas en stock (" << w << "x" << h << "):\n";
        for (const Pieza& pieza : listaDefectos) {
            pieza.imprimirPieza();
        }
    }
    int calcularArea() const {return w * h;}
  
    int getArea() const { return (w*h); }
    int getAncho() const { return w; }
    int getAlto() const { return h; }
    
    void imprimirStock() const { // Método para imprimir detalles del stock
        cout << "Stock de dimensiones: " << w << "x" << h << endl;
        //cout << "Total de piezas en stock: " << listaDefectos.size() << endl;
    }
};


#endif /* CLBASE_H */

