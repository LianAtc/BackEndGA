/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ClPieza.h
 * Author: 20180954 Lian Abril Tume Cusi
 * Created on 24 de octubre de 2024, 09:15 PM
 */

#ifndef CLPIEZA_H
#define CLPIEZA_H
#include <fstream>
using namespace std;

class Pieza {
private:
    int id;
    float x, y;
    float w, h;
    bool r; 

public:
    Pieza(int id, float x, float y, float w, float h, bool r) 
        : id(id), x(x), y(y), w(w), h(h), r(r) {}
    Pieza() : id(-1), x(0), y(0), w(0), h(0), r(false) {}

    void imprimirPieza() const {
        cout << "Pieza ID: " << id << ", Posición: (" << x << ", " << y << "), "
             << "Dimensiones: " << w << "x" << h << ", Rotada: " 
             << (r ? "Sí" : "No") << endl;
    }
    float calcularArea() const {return w * h;}
    int getID() const { return id; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getW() const { return w; }
    float getH() const { return h; }
    bool getRot() const { return r; }
    void setW(float nuevoW) { w = nuevoW; }
    void setH(float nuevoH) { h = nuevoH; }
    void setX(int nuevoX) { x = nuevoX; }
    void setY(int nuevoY) { y = nuevoY; }
    void rotarPieza() { 
        swap(w, h); 
        r = !r; 
    }
    bool operator==(const Pieza& otra) const {
        return (this->w == otra.w && this->h == otra.h && this->id == otra.id);
    }
};


#endif /* CLPIEZA_H */

